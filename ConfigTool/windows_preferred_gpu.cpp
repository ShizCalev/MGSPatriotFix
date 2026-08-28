#include "pch.h"
#include "windows_preferred_gpu.hpp"

#include "helper.hpp"
#include <wx/log.h>

namespace
{
    struct GpuFixTarget
    {
        const char* subfolder;
        const char* exeName;
    };

    constexpr GpuFixTarget kGpuFixTargets[] = {
        { "MGS1",  "mgs1.exe" },
        { "mgspw", "METAL GEAR SOLID PEACE WALKER.exe" },
    };

    void SetHighPerformanceGpuPreference(const std::filesystem::path& exePath)
    {
        HKEY hKey;
        const char* subKey = R"(Software\Microsoft\DirectX\UserGpuPreferences)";
        LONG result = RegCreateKeyExA(
            HKEY_CURRENT_USER,
            subKey,
            0,
            nullptr,
            REG_OPTION_NON_VOLATILE,
            KEY_READ | KEY_WRITE,
            nullptr,
            &hKey,
            nullptr);

        if (result != ERROR_SUCCESS)
        {
            wxLogDebug("High-performance GPU fix: failed to open registry key: %s", subKey);
            return;
        }

        const std::string valueName = exePath.string();
        const std::string value = "GpuPreference=2;";
        const DWORD valueSize = static_cast<DWORD>(value.size() + 1);

        if (RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), valueSize) == ERROR_SUCCESS)
        {
            wxLogDebug("High-performance GPU fix: wrote registry entry for %s", valueName.c_str());
        }
        else
        {
            wxLogDebug("High-performance GPU fix: failed to write registry entry for %s", valueName.c_str());
        }

        RegCloseKey(hKey);
    }
}

void HighPerformanceGpu::Fix()
{
    if (Helper::IsSteamOS())
    {
        return;
    }

    const std::filesystem::path root = Helper::FindGameRoot();

    for (const auto& target : kGpuFixTargets)
    {
        const std::filesystem::path dir = Helper::FindSubfolderCaseInsensitive(root, target.subfolder);
        if (dir.empty())
        {
            continue;
        }

        const std::filesystem::path exePath = dir / target.exeName;
        if (std::filesystem::exists(exePath))
        {
            SetHighPerformanceGpuPreference(exePath);
        }
    }
}
