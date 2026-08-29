#include "stdafx.h"
#include "ds3_rumble.hpp"
#include "pressure_inputs.hpp"

#include "common.hpp"
#include "logging.hpp"

#include <Xinput.h>
#include <hidsdi.h>
#include <hidpi.h>
#pragma comment(lib, "hid.lib")

// The game rumbles through XInput, which cannot see a DsHidMini DualShock 3. Take the motor values
// on their way to XInput and write them to the pad directly.
namespace
{
    // High byte small motor 0/1, low byte big motor 0-255.
    std::atomic<uint16_t> gMotors = 0;
    HANDLE gWake = nullptr;
    SafetyHookMid gSinkHook {};

    HANDLE gDev = nullptr;
    USHORT gReportLen = 0;
    int gMode = -1;     // kProfiles index: 0 SXS, 1 SDF, 2 native

    // The DS3's big motor stalls below ~1/3 throttle; lift nonzero levels onto the band that spins.
    constexpr int kBigMotorFloor = 96;

    uint8_t BigMotorCurve(uint8_t value)
    {
        const int scaled = std::min(255, value * Ds3Rumble::iStrength / 100);
        return scaled ? static_cast<uint8_t>(kBigMotorFloor + scaled * (255 - kBigMotorFloor) / 255) : 0;
    }

    void CloseDevice()
    {
        if (gDev != nullptr)
        {
            CloseHandle(gDev);
            gDev = nullptr;
        }
    }

    bool EnsureDevice()
    {
        std::wstring path;
        const int mode = PressureInputs::Ds3DeviceMode(path);
        if (mode < 0)
        {
            CloseDevice();
            return false;
        }
        if (gDev != nullptr && mode == gMode)
        {
            return true;
        }
        CloseDevice();
        gMode = mode;
        if (mode == 2)
        {
            static bool warned = false;
            if (!warned)
            {
                warned = true;
                spdlog::warn("DS3 Rumble - the native report mode has no rumble path; use DsHidMini SDF or SXS.");
            }
            return false;
        }

        gDev = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (gDev == INVALID_HANDLE_VALUE)
        {
            gDev = nullptr;
            return false;
        }

        // Windows wants full-length writes, so use the driver's fixed report sizes.
        PHIDP_PREPARSED_DATA pp = nullptr;
        HIDP_CAPS caps {};
        USHORT length = 0;
        if (HidD_GetPreparsedData(gDev, &pp))
        {
            if (HidP_GetCaps(pp, &caps) == HIDP_STATUS_SUCCESS && caps.OutputReportByteLength <= 64)
            {
                length = caps.OutputReportByteLength;
            }
            HidD_FreePreparsedData(pp);
        }
        gReportLen = length ? length : (mode == 0 ? 49 : 19);
        spdlog::info("DS3 Rumble - write handle open, {} mode, {}-byte output reports.",
            mode == 0 ? "SXS" : "SDF", gReportLen);
        return true;
    }

    bool WriteReport(const uint8_t* src, size_t n)
    {
        uint8_t buf[64] {};
        memcpy(buf, src, n);
        DWORD wrote = 0;
        return WriteFile(gDev, buf, gReportLen, &wrote, nullptr) && wrote == gReportLen;
    }

    // SDF speaks the standard force-feedback protocol: one constant force per motor.
    bool SendSdf(uint8_t smallMotor, uint8_t bigMotor)
    {
        if (smallMotor == 0 && bigMotor == 0)
        {
            const uint8_t stop[2] = { 0x19, 0x03 };
            return WriteReport(stop, sizeof(stop));
        }
        const auto lg = static_cast<uint16_t>(BigMotorCurve(bigMotor) * 10000 / 255);
        const uint8_t left[4] = { 0x14, 0x01, static_cast<uint8_t>(lg & 0xFF), static_cast<uint8_t>(lg >> 8) };
        const uint16_t sm = smallMotor ? 10000 : 0;
        const uint8_t right[4] = { 0x14, 0x02, static_cast<uint8_t>(sm & 0xFF), static_cast<uint8_t>(sm >> 8) };
        const uint8_t start[4] = { 0x18, 0x01, 0x01, 0x01 };
        return WriteReport(left, sizeof(left)) && WriteReport(right, sizeof(right)) &&
               WriteReport(start, sizeof(start));
    }

    // SXS takes the raw 49-byte sixaxis.sys packet; it owns the LED too, so that rides along.
    bool SendSxs(uint8_t smallMotor, uint8_t bigMotor)
    {
        uint8_t r[49] {};
        r[1] = 0x02;
        r[5] = 0xFF;
        r[6] = smallMotor ? 0x01 : 0x00;
        r[7] = 0xFF;
        r[8] = BigMotorCurve(bigMotor);
        r[13] = 0x02;
        constexpr uint8_t led[5] = { 0xFF, 0x27, 0x10, 0x00, 0x32 };
        for (int i = 0; i < 4; ++i)
        {
            memcpy(&r[14 + 5 * i], led, sizeof(led));
        }
        return WriteReport(r, sizeof(r));
    }

    // The pad holds its last state, so only changes are sent.
    DWORD WINAPI WriterThread(LPVOID)
    {
        uint16_t lastSent = 0;
        for (;;)
        {
            WaitForSingleObject(gWake, 1000);
            const uint16_t want = gMotors.load(std::memory_order_relaxed);
            if (want == lastSent)
            {
                continue;
            }
            if (!EnsureDevice())
            {
                continue;
            }
            const auto smallMotor = static_cast<uint8_t>(want >> 8);
            const auto bigMotor = static_cast<uint8_t>(want & 0xFF);
            if (gMode == 0 ? SendSxs(smallMotor, bigMotor) : SendSdf(smallMotor, bigMotor))
            {
                lastSent = want;
            }
            else
            {
                CloseDevice();
            }
        }
    }

    void Publish(uint16_t state)
    {
        if (gMotors.exchange(state, std::memory_order_relaxed) != state)
        {
            SetEvent(gWake);
        }
    }
}


namespace
{
    SafetyHookInline gSetStateHook {};

    // XInput names its motors by frequency: the left is the heavy one, the right the buzz.
    DWORD WINAPI HookedXInputSetState(DWORD user, XINPUT_VIBRATION* vibration)
    {
        if (user == 0 && vibration != nullptr && PressureInputs::HavePad())
        {
            // "small" is a Windows macro, so the motors are named for what they do instead.
            const auto heavy = static_cast<uint8_t>(vibration->wLeftMotorSpeed >> 8);
            const auto buzz = static_cast<uint8_t>(vibration->wRightMotorSpeed >> 8);
            Publish(static_cast<uint16_t>((buzz << 8) | heavy));
        }
        return gSetStateHook.stdcall<DWORD>(user, vibration);
    }
}

void Ds3Rumble::Shutdown()
{
    // Quitting mid-rumble buzzes forever unless the exit path sends one silence.
    Publish(0);
    if (gDev != nullptr)
    {
        if (gMode == 0) { SendSxs(0, 0); } else { SendSdf(0, 0); }
        CloseDevice();
    }
}

void Ds3Rumble::Initialize()
{
    if (!bEnabled || !(eGameType & MGS4))
    {
        return;
    }
    if (!PressureInputs::bEnabled)
    {
        spdlog::info("DS3 Rumble - needs Dualshock 3 Controller Support to be on.");
        return;
    }

    gWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (gWake == nullptr)
    {
        return;
    }

    // XInput is imported by ordinal; 3 is XInputSetState, which carries the motor values.
    const HMODULE xinput = GetModuleHandleA("XINPUT1_4.dll");
    if (xinput == nullptr)
    {
        spdlog::warn("DS3 Rumble - XINPUT1_4.dll is not loaded; no rumble.");
        return;
    }
    auto* setState = reinterpret_cast<void*>(GetProcAddress(xinput, MAKEINTRESOURCEA(3)));
    if (setState == nullptr)
    {
        spdlog::warn("DS3 Rumble - XInputSetState was not found.");
        return;
    }

    gSetStateHook = safetyhook::create_inline(setState, reinterpret_cast<void*>(HookedXInputSetState));
    LOG_HOOK(gSetStateHook, "MGS 4: DS3 Rumble - XInputSetState")
    if (!gSetStateHook)
    {
        return;
    }

    if (HANDLE thread = CreateThread(nullptr, 0, WriterThread, nullptr, 0, nullptr))
    {
        CloseHandle(thread);
    }
}
