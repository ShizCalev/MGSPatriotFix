#include "stdafx.h"
#include "pad_motion.hpp"
#include "pressure_inputs.hpp"

#include "common.hpp"
#include "helper.hpp"
#include "logging.hpp"

#include <algorithm>
#include <atomic>

namespace
{
    struct MotionData
    {
        float rotQuatX, rotQuatY, rotQuatZ, rotQuatW;
        float posAccelX, posAccelY, posAccelZ;
        float rotVelX, rotVelY, rotVelZ;
    };

    // Steam reports 16384 counts per g, so scale the pad's counts to match.
    constexpr float kStepsPerG = 16384.0f;
    constexpr float kLimit = 32767.0f;

    // The DualShock 3 and the DualSense disagree on which way up z is.
    constexpr float kAxisSign[3] { 1.0f, 1.0f, -1.0f };

    // The game only looks for a shake if it thinks it is holding a PlayStation pad.
    constexpr int kPlayStationPad = 2;

    constexpr size_t kGetMotionData = 29;    // ISteamInput006

    SafetyHookInline gMotionHook {};
    SafetyHookMid gDecideHook {};
    void* gInput = nullptr;
    void** gVtable = nullptr;
    bool gGaveUp = false;
    std::atomic<bool> gSaidSo = false;

    // Zero means read the motion from the accelerometer, anything else from the orientation.
    uint8_t* gFromQuaternion = nullptr;

    bool Empty(const MotionData& m)
    {
        return m.posAccelX == 0.0f && m.posAccelY == 0.0f && m.posAccelZ == 0.0f;
    }

    void* HookedGetMotionData(void* self, void* into, uint64_t input)
    {
        void* result = gMotionHook.stdcall<void*>(self, into, input);
        if (!PadMotion::bEnabled || !Memory::IsWritable(into, sizeof(MotionData)))
        {
            return result;
        }
        auto& m = *static_cast<MotionData*>(into);

        int axis[3] {};
        if (!Empty(m) || !PressureInputs::RawMotion(axis))
        {
            return result;
        }

        const float scale = kStepsPerG / static_cast<float>(PressureInputs::MotionCountsPerG());
        for (int i = 0; i < 3; i++)
        {
            (&m.posAccelX)[i] = std::clamp(static_cast<float>(axis[i]) * scale * kAxisSign[i],
                -kLimit, kLimit);
        }

        m.rotQuatX = m.rotQuatY = m.rotQuatZ = 0.0f;
        m.rotQuatW = 1.0f;
        m.rotVelX = m.rotVelY = m.rotVelZ = 0.0f;

        // The orientation loses how hard the pad was shaken, so force the accelerometer path.
        if (gFromQuaternion != nullptr)
        {
            *gFromQuaternion = 0;
        }

        if (!gSaidSo.exchange(true))
        {
            spdlog::info("MGS 4: Pad Motion - feeding the pad's accelerometer to the game, "
                "{} counts to the g scaled to {}.", PressureInputs::MotionCountsPerG(),
                static_cast<int>(kStepsPerG));
        }
        return result;
    }

    // The game re-decides the pad type on every input, so answer where that decision is made.
    void HookTypeDecision()
    {
        constexpr ptrdiff_t kCompare = 12;      // the cmp eax,ecx the pattern ends on

        if (uint8_t* at = Memory::PatternScanUnique(baseModule,
            "8B 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 3B C1 75 08 39 15",
            "MGS 4: Pad Motion - controller type | where the game decides it"))
        {
            gDecideHook = safetyhook::create_mid(at + kCompare, [](SafetyHookContext& ctx)
            {
                if (PressureInputs::HavePad())
                {
                    ctx.rcx = kPlayStationPad;
                }
            });
            LOG_HOOK(gDecideHook, "MGS 4: Pad Motion - controller type")
        }
    }

    void Hook()
    {
        const HMODULE steam = GetModuleHandleA("steam_api64.dll");
        if (steam == nullptr)
        {
            return;
        }
        using UserFn = int (*)();
        using FindFn = void* (*)(int, const char*);
        auto* user = reinterpret_cast<UserFn>(GetProcAddress(steam, "SteamAPI_GetHSteamUser"));
        auto* find = reinterpret_cast<FindFn>(
            GetProcAddress(steam, "SteamInternal_FindOrCreateUserInterface"));
        if (user == nullptr || find == nullptr)
        {
            return;
        }
        gInput = find(user(), "SteamInput006");
        if (gInput == nullptr || !Memory::IsReadable(gInput, sizeof(void*)))
        {
            return;
        }
        gVtable = *reinterpret_cast<void***>(gInput);

        if (!Memory::IsExecutable(gVtable[kGetMotionData]))
        {
            spdlog::warn("MGS 4: Pad Motion - Steam Input's table does not look like code; "
                "leaving it alone.");
            gGaveUp = true;
            gVtable = nullptr;
            return;
        }
        gMotionHook = safetyhook::create_inline(gVtable[kGetMotionData],
            reinterpret_cast<void*>(HookedGetMotionData));
        LOG_HOOK(gMotionHook, "MGS 4: Pad Motion - ISteamInput::GetMotionData")

        if (uint8_t* test = Memory::PatternScanUnique(baseModule,
            "48 8B 43 10 48 89 43 18 48 8B 43 08 48 89 43 10 48 8B 03 48 89 43 08 80 3D",
            "MGS 4: Pad Motion - Sixaxis source | accelerometer or orientation"))
        {
            constexpr ptrdiff_t kCmp = 23;      // cmp byte ptr [rip+disp32], imm8
            auto* flag = reinterpret_cast<uint8_t*>(Memory::GetRipRelativeAddress(test + kCmp, 2, 7));

            // The byte is a selector, so it holds 0 or 1. Anything else is not the byte we resolved
            // for and is not worth writing to.
            if (Memory::IsWritable(flag, sizeof(uint8_t)) && *flag <= 1)
            {
                gFromQuaternion = flag;
            }
            else
            {
                spdlog::warn("MGS 4: Pad Motion - the Sixaxis source byte resolved to {:s}+{:X}, "
                    "which is not writable; leaving it alone.", sExeName.c_str(),
                    reinterpret_cast<uintptr_t>(flag) - reinterpret_cast<uintptr_t>(baseModule));
            }
        }
        HookTypeDecision();
    }

    DWORD WINAPI WaitThread(LPVOID)
    {
        for (int tries = 0; tries < 60 && gVtable == nullptr && !gGaveUp; ++tries)
        {
            Sleep(1000);
            Hook();
        }
        if (gVtable == nullptr && !gGaveUp)
        {
            spdlog::info("MGS 4: Pad Motion - Steam Input never appeared; the pad's motion has "
                "nowhere to go.");
        }
        return 0;
    }
}

void PadMotion::Initialize()
{
    if (!bEnabled || !(eGameType & MGS4) || !PressureInputs::bEnabled)
    {
        return;
    }
    if (HANDLE thread = CreateThread(nullptr, 0, WaitThread, nullptr, 0, nullptr))
    {
        CloseHandle(thread);
    }
}
