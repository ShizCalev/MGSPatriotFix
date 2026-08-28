#include "pch.h"
#include "windows_fullscreen_optimization.hpp"

#include "helper.hpp"
#include <wx/log.h>

namespace
{
    void DisableFullscreenOptimization(const std::filesystem::path& exePath)
    {
        HKEY hKey;
        const char* subKey = R"(Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers)";
        LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ | KEY_WRITE, &hKey);
        if (result != ERROR_SUCCESS)
        {
            wxLogDebug("Fullscreen optimization fix: failed to open registry key: %s", subKey);
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

        if (modified)
        {
            const DWORD valueSize = static_cast<DWORD>(value.size() + 1);
            if (RegSetValueExA(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), valueSize) == ERROR_SUCCESS)
            {
                wxLogDebug("Fullscreen optimization fix: wrote registry entry for %s", valueName.c_str());
            }
            else
            {
                wxLogDebug("Fullscreen optimization fix: failed to write registry entry for %s", valueName.c_str());
            }
        }

        RegCloseKey(hKey);
    }
}

void FixFullscreenOptimizationMgs1::Fix()
{
    const std::filesystem::path root = Helper::FindGameRoot();
    const std::filesystem::path dir = Helper::FindSubfolderCaseInsensitive(root, "MGS1");
    if (dir.empty())
    {
        return;
    }

    const std::filesystem::path exePath = dir / "mgs1.exe";
    if (std::filesystem::exists(exePath))
    {
        DisableFullscreenOptimization(exePath);
    }
}
