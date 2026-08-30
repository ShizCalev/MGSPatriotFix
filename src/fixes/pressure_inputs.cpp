#include "stdafx.h"
#include "pressure_inputs.hpp"

#include "common.hpp"
#include "logging.hpp"

#include <atomic>
#include <cmath>
#include <mutex>
#include <hidsdi.h>
#pragma comment(lib, "hid.lib")

// libgv's pad converter flattens GV_PAD.pressure[12] to 0xFF/0x00 for every pad it reads, passing
// only the analogue triggers through. Refill it from a DualShock 3, via DsHidMini in SDF or SXS mode.
namespace
{
    constexpr size_t kSlots = 12;
    constexpr size_t kCircle = 5;
    constexpr size_t kTriangle = 4;
    constexpr size_t kL1 = 8;           // libgv.h PAD_PRESS_L1
    constexpr size_t kR1 = 9;           // libgv.h PAD_PRESS_R1
    constexpr size_t kL2 = 10;          // libgv.h PAD_PRESS_L2
    constexpr size_t kR2 = 11;          // libgv.h PAD_PRESS_R2
    constexpr size_t kSquare = 7;

    // Where each of our slots (R L U D TRI CIR CRO SQU L1 R1 L2 R2) sits in the pad's report.
    constexpr int kSdfOrder[kSlots] = { 3, 5, 2, 4, 8, 9, 10, 11, 6, 7, 0, 1 };
    constexpr int kNativeOrder[kSlots] = { 1, 3, 0, 2, 8, 9, 10, 11, 6, 7, 4, 5 };

    constexpr size_t kNone = static_cast<size_t>(-1);

    struct Profile
    {
        const char* name;
        bool feature;           // poll HidD_GetFeature rather than read input reports
        size_t length;
        size_t base;            // first pressure byte
        const int* order;
        size_t accel;           // first accelerometer byte, kNone in a report too short to carry one
        int accelCentre;        // ten unsigned bits, so rest sits around the middle of the range
        int countsPerG;         // what the accelerometer calls one g; no two pads agree on it
    };

    // Three little-endian accelerometer values centred on 512. SDF's report is too short for them.
    constexpr Profile kProfiles[] = {
        { "SXS",    true,  50, 15, kNativeOrder, 42,    512, 115 },
        { "SDF",    false, 39,  8, kSdfOrder,    kNone, 512, 115 },
        { "native", false, 49, 14, kNativeOrder, 41,    512, 115 },
    };
    const Profile* gProfile = nullptr;

    void NoteMotion(const uint8_t* report);   // defined with the shake handling further down

    std::atomic<bool> gHavePad = false;
    std::atomic<uint8_t> gCirclePressure = 0;
    uint8_t gPressure[kSlots] {};
    std::mutex gPressureLock;

    // The open device's path and kProfiles index, for rumble's own write handle.
    std::wstring gDevicePath;
    int gDeviceMode = -1;

    void PublishDevice(const std::wstring& path, int mode)
    {
        const std::lock_guard<std::mutex> guard(gPressureLock);
        gDevicePath = path;
        gDeviceMode = mode;
    }

    constexpr DWORD kReadGone = ~0u;    // disconnected, as opposed to merely quiet

    constexpr DWORD kRetryFirst = 2000;
    constexpr DWORD kRetryMax = 16000;

    // Overlapped: a silent interface must not wedge discovery or the reader.
    DWORD ReadReport(HANDLE h, HANDLE ev, uint8_t* buf, DWORD len, DWORD timeoutMs)
    {
        OVERLAPPED ov {};
        ov.hEvent = ev;
        ResetEvent(ev);

        if (!ReadFile(h, buf, len, nullptr, &ov) && GetLastError() != ERROR_IO_PENDING)
        {
            return kReadGone;
        }
        if (WaitForSingleObject(ev, timeoutMs) != WAIT_OBJECT_0)
        {
            CancelIo(h);
            WaitForSingleObject(ev, 200);
            return 0;
        }

        DWORD read = 0;
        return GetOverlappedResult(h, &ov, &read, FALSE) ? read : kReadGone;
    }

    // DsHidMini stamps 00 3F over the first two bytes of the feature report. That pair is SXS.
    bool ReadFeature(HANDLE h, uint8_t* buf, size_t len)
    {
        memset(buf, 0, len);
        return HidD_GetFeature(h, buf, static_cast<ULONG>(len)) != FALSE;
    }

    // Match on exact length: a 49-byte report would otherwise parse as SDF at the wrong offsets.
    const Profile* ProbeInputProfile(HANDLE h)
    {
        uint8_t buf[96];
        HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (ev == nullptr)
        {
            return nullptr;
        }
        const DWORD read = ReadReport(h, ev, buf, sizeof(buf), 500);
        CloseHandle(ev);

        for (const Profile& p : kProfiles)
        {
            if (!p.feature && read == p.length && buf[0] == 0x01)
            {
                return &p;
            }
        }
        return nullptr;
    }

    // Steam Input hides the pad from SetupDi, so take the interface paths from the registry.
    HANDLE OpenDs3()
    {
        GUID hid {};
        HidD_GetHidGuid(&hid);

        wchar_t guid[40];
        swprintf_s(guid, L"{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            hid.Data1, hid.Data2, hid.Data3, hid.Data4[0], hid.Data4[1], hid.Data4[2],
            hid.Data4[3], hid.Data4[4], hid.Data4[5], hid.Data4[6], hid.Data4[7]);

        HKEY cls = nullptr;
        const std::wstring devices =
            L"SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\" + std::wstring(guid);
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, devices.c_str(), 0, KEY_READ, &cls) != ERROR_SUCCESS)
        {
            return nullptr;
        }

        HANDLE found = nullptr;
        size_t candidates = 0;
        DWORD lastError = 0;
        wchar_t name[512];
        for (DWORD i = 0; found == nullptr; ++i)
        {
            DWORD len = static_cast<DWORD>(std::size(name));
            if (RegEnumKeyExW(cls, i, name, &len, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
            {
                break;
            }
            std::wstring key = name;
            std::wstring lower = key;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
            if (lower.find(L"vid_054c") == std::wstring::npos
                || lower.find(L"pid_0268") == std::wstring::npos
                || lower.rfind(L"##?#", 0) != 0
                || lower.find(L"ig_00") != std::wstring::npos)   // XInput node, no pressure
            {
                continue;
            }
            ++candidates;
            // the registry spells the symbolic link with '#' for the leading separators only.
            const std::wstring path = L"\\\\?\\" + key.substr(4);
            HANDLE h = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
            if (h == INVALID_HANDLE_VALUE)
            {
                lastError = GetLastError();     // 2 = stale entry, 5 = something is hiding it
                continue;
            }

            uint8_t probe[96] {};
            if (ReadFeature(h, probe, sizeof(probe)) && probe[1] == 0x00 && probe[2] == 0x3F)
            {
                found = h;
                gProfile = &kProfiles[0];
                PublishDevice(path, 0);
                continue;
            }

            // SDF and native need input reports, so reopen for read.
            CloseHandle(h);
            h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
            if (h == INVALID_HANDLE_VALUE)
            {
                lastError = GetLastError();
                continue;
            }
            // A dead node would block the reader and hide a working pad behind it.
            if (const Profile* profile = ProbeInputProfile(h))
            {
                found = h;
                gProfile = profile;
                PublishDevice(path, static_cast<int>(profile - kProfiles));
            }
            else
            {
                CloseHandle(h);
            }
        }
        RegCloseKey(cls);

        static bool reported = false;
        if (found == nullptr && !reported)
        {
            reported = true;
            if (candidates == 0)
            {
                spdlog::info("Pressure Inputs - No DualShock 3 registered.");
            }
            else if (lastError == ERROR_ACCESS_DENIED)
            {
                spdlog::warn("Pressure Inputs - {} DualShock 3 interface(s) found but access was "
                    "denied. HidHide must allow the game.", candidates);
            }
            else
            {
                spdlog::warn("Pressure Inputs - {} DualShock 3 interface(s) found, none reporting "
                    "pressure. DsHidMini must be in SDF or SXS mode.", candidates);
            }
        }
        return found;
    }

    DWORD WINAPI ReaderThread(LPVOID)
    {
        uint8_t report[96];
        HANDLE pad = nullptr;
        DWORD retry = kRetryFirst;
        HANDLE ev = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        for (;;)
        {
            if (pad == nullptr)
            {
                pad = OpenDs3();
                if (pad == nullptr)
                {
                    gHavePad = false;
                    Sleep(retry);
                    retry = std::min<DWORD>(retry * 2, kRetryMax);
                    continue;
                }
                retry = kRetryFirst;
                spdlog::info("Pressure Inputs - DualShock 3 opened, {} mode, {}-byte reports.",
                    gProfile->name, gProfile->length);
            }

            bool lost = false;
            if (gProfile->feature)
            {
                // Synchronous IOCTL, so keep it off the render thread and poll once a frame.
                lost = !ReadFeature(pad, report, gProfile->length);
                if (!lost)
                {
                    Sleep(8);
                }
            }
            else
            {
                const DWORD read = ReadReport(pad, ev, report, sizeof(report), 5000);
                if (read == 0)
                {
                    continue;      // quiet pad, not a lost one
                }
                lost = read == kReadGone || read < gProfile->length;
            }

            if (lost)
            {
                CloseHandle(pad);
                pad = nullptr;
                gProfile = nullptr;
                gHavePad = false;
                PublishDevice({}, -1);
                Sleep(retry);
                retry = std::min<DWORD>(retry * 2, kRetryMax);
                continue;
            }

            NoteMotion(report);

            const std::lock_guard<std::mutex> guard(gPressureLock);
            for (size_t s = 0; s < kSlots; ++s)
            {
                gPressure[s] = report[gProfile->base + gProfile->order[s]];
            }
            gCirclePressure = gPressure[kCircle];
            gHavePad = true;
        }
    }

    // ---- the pad's motion ---------------------------------------------------------------------
    // Keep the latest accelerometer reading. pad_motion.cpp hands it to the game, which finds the
    // shake itself.
    std::atomic<int> gMotion[3] {};
    std::atomic<bool> gHaveMotion = false;

    void NoteMotion(const uint8_t* report)
    {
        if (gProfile == nullptr || gProfile->accel == kNone)
        {
            return;
        }
        const size_t at = gProfile->accel;
        if (at + 6 > gProfile->length)
        {
            return;
        }
        for (int i = 0; i < 3; i++)
        {
            const int low = report[at + i * 2];
            const int high = report[at + i * 2 + 1];
            gMotion[i] = (low | (high << 8)) - gProfile->accelCentre;
        }
        gHaveMotion = true;
    }

    void ReadPad(uint8_t (&out)[kSlots])
    {
        const std::lock_guard<std::mutex> guard(gPressureLock);
        memcpy(out, gPressure, sizeof(out));
    }


    // ---- MGS 4 ------------------------------------------------------------------------------
    // Every pad reader in the game works from copies of one buffer entry, so filling that entry in
    // covers all of them. Entry layout: sticks at 4, twelve pressure values at 8, motion at 0x14.
    constexpr size_t kUp = 2;                   // libgv.h PAD_PRESS_U
    constexpr size_t kEntrySize = 0x2C;
    constexpr size_t kEntryPressure = 0x08;
    constexpr size_t kPadsPerRing = 4;          // entries interleave as (slot + sample * 4) * 0x2C

    uintptr_t gRingBase = 0;                    // the raw pad ring, so only pad slot 0 is fed
    const float* gDemoZoomMax = nullptr;        // the clamp the cutscene zoom uses (2.0 as shipped)
    std::atomic<bool> gFedOnce = false;

    // Nothing is written unless the entry is demonstrably one of the ring's, so a hook that landed
    // somewhere unexpected writes nowhere rather than over whatever rdi happened to hold.
    void FeedEntry(uintptr_t entry)
    {
        if (entry == 0 || gRingBase == 0 || entry < gRingBase)
        {
            return;
        }

        const uintptr_t into = entry - gRingBase;
        if (into % kEntrySize != 0 || (into / kEntrySize) % kPadsPerRing != 0)
        {
            return;
        }

        if (!Memory::IsWritable(reinterpret_cast<void*>(entry + kEntryPressure), kSlots))
        {
            return;
        }

        if (!gHavePad.load())
        {
            return;         // no pressure to feed, but the entry above was still worth reading
        }
        uint8_t now[kSlots];
        ReadPad(now);

        memcpy(reinterpret_cast<void*>(entry + kEntryPressure), now, kSlots);

        if (!gFedOnce.exchange(true))
        {
            spdlog::info("MGS 4: Pressure Inputs - Feeding DualShock 3 pressure into pad slot 0.");
        }
    }

    // The port zooms cutscenes by a fixed step. On PS3 how hard you pressed was the amount.
    float DemoZoomTarget(uint8_t up)
    {
        const float max = (gDemoZoomMax != nullptr) ? *gDemoZoomMax : 2.0f;
        return 1.0f + (max - 1.0f) * (static_cast<float>(up) / 255.0f);
    }

    void InitializeMGS4()
    {
        // The raw ring's base, from the lea that indexes it right before the converter call.
        if (uint8_t* address = Memory::PatternScanUnique(baseModule,
            "48 6B C9 2C F2 0F 11 84 24 ?? ?? ?? ?? 48 8D 05 ?? ?? ?? ?? 48 8B D3 48 03 C8",
            "MGS 4: Pressure Inputs - Pad Ring | GV_PAD thread -> converter call"))
        {
            constexpr ptrdiff_t kLea = 13;      // lea rax,[ring]
            const uintptr_t ring = Memory::GetRipRelativeAddress(address + kLea, 3, 7);
            if (!Memory::IsReadable(reinterpret_cast<const void*>(ring), sizeof(void*)))
            {
                spdlog::error("MGS 4: Pressure Inputs - the pad ring resolved to {:s}+{:X}, which is "
                    "not there; leaving the ring alone.", sExeName.c_str(),
                    ring - reinterpret_cast<uintptr_t>(baseModule));
            }
            else
            {
                gRingBase = ring;
                if (g_Logging.bVerboseLogging)
                {
                    spdlog::info("MGS 4: Pressure Inputs - pad ring at {:s}+{:X}", sExeName.c_str(),
                        gRingBase - reinterpret_cast<uintptr_t>(baseModule));
                }
            }
        }

        constexpr ptrdiff_t kAfterR2 = 3;       // just past the mov [rdi+13h],al that stores R2

        if (uint8_t* address = Memory::PatternScanUnique(baseModule,
            "88 47 13 0F 29 74 24 40 4C 8B 74 24 78",
            "MGS 4: Pressure Inputs - Pad Writer | GV_PAD thread -> XINPUT_STATE to PADBUF"))
        {
            static SafetyHookMid writerHook {};
            writerHook = safetyhook::create_mid(address + kAfterR2, [](SafetyHookContext& ctx)
            {
                FeedEntry(ctx.rdi);
            });
            LOG_HOOK(writerHook, "MGS 4: Pressure Inputs - Pad Writer")
        }

        // The game ignores pressure unless the controls are bound its way. Answer both of its
        // checks so the pad's real values get used.
        constexpr ptrdiff_t kTypeCheck = 0x16;  // the dec eax on the controller type

        if (uint8_t* address = Memory::PatternScanUnique(baseModule,
            "48 83 EC 28 8D 81 00 FF FF FF A9 FF FE FF FF 75 ?? E8 ?? ?? ?? ?? FF C8 83 F8 06 77",
            "MGS 4: Pressure Inputs - Firm Press Gate | analogue-trigger binding test"))
        {
            static SafetyHookMid gateBindingHook {};
            gateBindingHook = safetyhook::create_mid(address, [](SafetyHookContext& ctx)
            {
                if (gHavePad.load())
                {
                    ctx.rcx = 0x200;
                }
            });
            LOG_HOOK(gateBindingHook, "MGS 4: Pressure Inputs - Firm Press Gate | binding test")

            static SafetyHookMid gateTypeHook {};
            gateTypeHook = safetyhook::create_mid(address + kTypeCheck, [](SafetyHookContext& ctx)
            {
                if (gHavePad.load())
                {
                    ctx.rax = 0x7F;    // past the switch, straight to "true"
                }
            });
            LOG_HOOK(gateTypeHook, "MGS 4: Pressure Inputs - Firm Press Gate | controller type test")
        }

        if (uint8_t* address = Memory::PatternScanUnique(baseModule,
            "F3 0F 10 1D ?? ?? ?? ?? 0F 2F C3 73 0D 41 0F 2F C6",
            "MGS 4: Pressure Inputs - Cutscene Zoom Clamp | demo input"))
        {
            const auto* max = reinterpret_cast<const float*>(Memory::GetRipRelativeAddress(address, 4, 8));
            if (Memory::IsReadable(max, sizeof(float)))
            {
                gDemoZoomMax = max;
            }
            else
            {
                spdlog::warn("MGS 4: Pressure Inputs - the zoom clamp did not resolve to anything "
                    "readable; falling back to the built-in maximum.");
            }
        }

        // Where the zoom target is stored: replace the fixed step with how hard UP is pressed.
        constexpr ptrdiff_t kZoomStore = 8;     // the movss [rbx+0D8h],xmm6 that stores the target

        if (uint8_t* address = Memory::PatternScanUnique(baseModule,
            "F3 0F 58 B3 D8 00 00 00 F3 0F 11 B3 D8 00 00 00 EB",
            "MGS 4: Pressure Inputs - Cutscene Zoom | demo input -> CinematicZoomIn"))
        {
            static SafetyHookMid zoomHook {};
            zoomHook = safetyhook::create_mid(address + kZoomStore, [](SafetyHookContext& ctx)
            {
                if (!gHavePad.load())
                {
                    return;
                }
                uint8_t now[kSlots];
                ReadPad(now);
                if (now[kUp] == 0)
                {
                    return;
                }
                ctx.xmm6.f32[0] = DemoZoomTarget(now[kUp]);
            });
            LOG_HOOK(zoomHook, "MGS 4: Pressure Inputs - Cutscene Zoom")
        }
    }
}

void PressureInputs::ReadPad(uint8_t (&out)[kPadSlots])
{
    static_assert(kPadSlots == kSlots, "pad slot count out of step with libgv");
    const std::lock_guard<std::mutex> guard(gPressureLock);
    memcpy(out, gPressure, sizeof(out));
}

bool PressureInputs::RawMotion(int (&axis)[3])
{
    if (!gHaveMotion.load())
    {
        return false;
    }
    for (int i = 0; i < 3; i++)
    {
        axis[i] = gMotion[i].load();
    }
    return true;
}

int PressureInputs::MotionCountsPerG()
{
    const Profile* profile = gProfile;
    return profile != nullptr ? profile->countsPerG : 113;
}

bool PressureInputs::HavePad()
{
    return gHavePad.load();
}

int PressureInputs::Ds3DeviceMode(std::wstring& path)
{
    if (!gHavePad.load())
    {
        return -1;
    }
    const std::lock_guard<std::mutex> guard(gPressureLock);
    if (gDeviceMode < 0)
    {
        return -1;
    }
    path = gDevicePath;
    return gDeviceMode;
}

void PressureInputs::Initialize()
{
    if (!bEnabled)
    {
        return;
    }
    if (!(eGameType & MGS4))
    {
        return;
    }

    InitializeMGS4();

    if (HANDLE reader = CreateThread(nullptr, 0, ReaderThread, nullptr, 0, nullptr))
    {
        CloseHandle(reader);
    }
}
