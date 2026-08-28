#include "pch.h"
#include "windows_fullscreen_optimization.hpp"

#include "helper.hpp"
#include <wx/log.h>

namespace
{
    void SetFullscreenOptimizationState(const std::filesystem::path& exePath, bool shouldApply)
    {
        HKEY hKey;
        const char* subKey = R"(Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers)";
        LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ | KEY_WRITE, &hKey);
        if (result != ERROR_SUCCESS)
        {
            wxLogError("Fullscreen optimization fix: failed to open registry key: %s", subKey);
            return;
        }

        const std::string valueName = exePath.string();

        DWORD type = 0;
        DWORD dataSize = 0;
        RegQueryValueExA(hKey, valueName.c_str(), nullptr, &type, nullptr, &dataSize);

        std::string value;
        if (dataSize > 0)
        {
            std::vector<char> data(dataSize);
            if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(data.data()), &dataSize) == ERROR_SUCCESS)
            {
                value.assign(data.begin(), data.end());
                while (!value.empty() && value.back() == '\0')
                    value.pop_back();
            }
        }

        bool modified = false;

        if (shouldApply)
        {
            if (!value.empty() && value[0] != '~')
            {
                value = "~ " + value;
                modified = true;
            }
            if (value.find("DISABLEDXMAXIMIZEDWINDOWEDMODE") == std::string::npos)
            {
                if (!value.empty() && value.back() != ' ')
                    value.push_back(' ');
                value += "DISABLEDXMAXIMIZEDWINDOWEDMODE";
                modified = true;
            }
        }
        else
        {
            size_t pos = value.find("DISABLEDXMAXIMIZEDWINDOWEDMODE");
            if (pos != std::string::npos)
            {
                value.erase(pos, strlen("DISABLEDXMAXIMIZEDWINDOWEDMODE"));
                while (!value.empty() && value.back() == ' ')
                    value.pop_back();
                if (value == "~")
                    value.clear();
                modified = true;
            }
        }

        if (!modified)
        {
            RegCloseKey(hKey);
            return;
        }

        if (value.empty())
        {
            if (RegDeleteValueA(hKey, valueName.c_str()) == ERROR_SUCCESS)
            {
                wxLogDebug("Fullscreen optimization fix: deleted registry entry for %s", valueName.c_str());
            }
            else
            {
                wxLogError("Fullscreen optimization fix: failed to delete registry entry for %s", valueName.c_str());
            }
        }
        else
        {
            const DWORD valueSize = static_cast<DWORD>(value.size() + 1);
            if (RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), valueSize) == ERROR_SUCCESS)
            {
                wxLogDebug("Fullscreen optimization fix: wrote registry entry for %s", valueName.c_str());
            }
            else
            {
                wxLogError("Fullscreen optimization fix: failed to write registry entry for %s", valueName.c_str());
            }
        }

        RegCloseKey(hKey);
    }
}

void FixFullscreenOptimizationMgs1::Fix()
{
    if (Helper::IsSteamOS())
    {
        return;
    }

    const std::filesystem::path root = Helper::FindGameRoot();
    const std::filesystem::path dir = Helper::FindSubfolderCaseInsensitive(root, "MGS1");
    if (dir.empty())
    {
        return;
    }

    const std::filesystem::path exePath = dir / "mgs1.exe";
    if (std::filesystem::exists(exePath))
    {
        SetFullscreenOptimizationState(exePath, true);
    }
}

void FixFullscreenOptimization::Fix(bool disableFullscreenOptimization)
{
    if (Helper::IsSteamOS())
    {
        return;
    }

    const std::filesystem::path root = Helper::FindGameRoot();

    struct FixTarget
    {
        const char* subfolder;
        const char* exeName;
    };

    constexpr FixTarget kTargets[] = {
        { "MGS4",  "mgs4.exe" },
        { "mgspw", "METAL GEAR SOLID PEACE WALKER.exe" },
    };

    for (const auto& target : kTargets)
    {
        const std::filesystem::path dir = Helper::FindSubfolderCaseInsensitive(root, target.subfolder);
        if (dir.empty())
        {
            continue;
        }

        const std::filesystem::path exePath = dir / target.exeName;
        if (std::filesystem::exists(exePath))
        {
            SetFullscreenOptimizationState(exePath, disableFullscreenOptimization);
        }
    }
}
