#include "stdafx.h"

#include "common.hpp"
#include "launcher_skips_and_starts.hpp"

#include "config.hpp"
#include "logging.hpp"

namespace
{
    // parse resolution / upscale / movie settings from launcher_sv json
    std::optional<std::string> GetLauncherSvValue(const std::string& json, const std::string& key)
    {
        auto extractArray = [&](const std::string& arrayKey) -> std::vector<std::string>
        {
            std::vector<std::string> result;
            size_t pos = json.find("\"" + arrayKey + "\":[");
            if (pos == std::string::npos)
            {
                return result;
            }
            pos = json.find('[', pos) + 1;
            size_t end = json.find(']', pos);
            std::string arrayContent = json.substr(pos, end - pos);

            size_t i = 0;
            while (i < arrayContent.size())
            {
                size_t startQuote = arrayContent.find('"', i);
                if (startQuote == std::string::npos)
                {
                    break;
                }
                size_t endQuote = arrayContent.find('"', startQuote + 1);
                if (endQuote == std::string::npos)
                {
                    break;
                }
                result.push_back(arrayContent.substr(startQuote + 1, endQuote - startQuote - 1));
                i = endQuote + 1;
            }
            return result;
        };

        const std::vector<std::string> keys = extractArray("keyList");
        const std::vector<std::string> values = extractArray("valueList");

        for (size_t i = 0; i < keys.size() && i < values.size(); ++i)
        {
            if (keys[i] == key)
            {
                return values[i];
            }
        }
        return std::nullopt;
    }

    std::filesystem::path FindNewestLauncherSvPath()
    {
        std::filesystem::path bestPath;
        std::filesystem::file_time_type bestTime{};

        if (!std::filesystem::exists(sGameSavePath))
        {
            return {};
        }

        for (const auto& entry : std::filesystem::directory_iterator(sGameSavePath))
        {
            if (!entry.is_directory())
            {
                continue;
            }

            std::filesystem::path candidate = entry.path() / "launcher" / "launcher_sv";
            if (!std::filesystem::exists(candidate))
            {
                continue;
            }

            auto mtime = std::filesystem::last_write_time(candidate);
            if (bestPath.empty() || mtime > bestTime)
            {
                bestPath = candidate;
                bestTime = mtime;
            }
        }
        return bestPath;
    }

    // nullopt if the user's never actually run the launcher.
    std::optional<std::wstring> BuildMGSPWExtraArgs()
    {
        const std::filesystem::path svPath = FindNewestLauncherSvPath();
        if (svPath.empty())
        {
            spdlog::warn("LauncherSkipsAndStarts: No launcher_sv found under {}, cannot Skip Launcher yet - run the launcher normally at least once first.", sGameSavePath.string());
            return std::nullopt;
        }

        std::ifstream file(svPath, std::ios::binary);
        std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        const std::string resolution = GetLauncherSvValue(json, "GameResolution").value_or("0");
        const std::string upscale = GetLauncherSvValue(json, "GameUpscale").value_or("0");
        const std::string movie = GetLauncherSvValue(json, "GameMovie").value_or("1");

        spdlog::info("LauncherSkipsAndStarts: Read from {}: resolution={}, upscale={}, movie={}", svPath.string(), resolution, upscale, movie);

        return L" -resolution " + Util::UTF8toWide(resolution) + L" -upscale " + Util::UTF8toWide(upscale) + L" -movie " + Util::UTF8toWide(movie);
    }

    bool SkipLauncherEntirely()
    {
        if (!LauncherSkipsAndStarts::bSkipLauncher)
        {
            return false;
        }

        // game relaunched us (quit to launcher) -> wait for it to fully exit, then close
        if (Util::IsProcessParent(game->ExeName))
        {
            spdlog::info("LauncherSkipsAndStarts: Launcher was started by the game returning to main menu.");
            spdlog::info("LauncherSkipsAndStarts: Waiting for {} to exit before closing.", game->ExeName);

            const std::filesystem::path gameExePath = sGameRootPath / game->GameSubfolder / game->ExeName;
            while (Util::IsProcessRunning(gameExePath))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            spdlog::info("LauncherSkipsAndStarts: {} exited, closing launcher.", game->ExeName);
            bForcedLauncherShutdown = true;
            spdlog::shutdown();
            ExitProcess(EXIT_SUCCESS);
        }

        const std::filesystem::path gameExePath = sGameRootPath / game->GameSubfolder / game->ExeName;
        const bool isMGS4Launcher = (game->ExeName == kGames.at(MGS4).ExeName);

        std::wstring extraArgs = L" -resolution 0";
        if (!isMGS4Launcher)
        {
            std::optional<std::wstring> mgspwArgs = BuildMGSPWExtraArgs();
            if (!mgspwArgs)
            {
                return false;
            }
            extraArgs = *mgspwArgs;
        }

        std::wstring commandLine = L"-region " + Util::UTF8toWide(LauncherSkipsAndStarts::sRegion) + L" -lan " + Util::UTF8toWide(LauncherSkipsAndStarts::sLanguage) + L" -selfregion EU" + extraArgs;

        std::wstring launcherRoot = sExePath.wstring();
        if (!launcherRoot.empty() && launcherRoot.back() == L'\\')
        {
            launcherRoot.pop_back();
        }

        const auto& ctrlTypesInternal = isMGS4Launcher ? kMGS4LauncherConfigCtrlTypesInternal : kMGSPWLauncherConfigCtrlTypesInternal;

        commandLine += L" -launcherpath launcher.exe";
        commandLine += L" -ctrltype " + Util::UTF8toWide(Util::GetNameAtIndex(ctrlTypesInternal, LauncherSkipsAndStarts::iCtrlType));
        commandLine += L" -launcherroot \"" + launcherRoot + L"\"";

        spdlog::info("LauncherSkipsAndStarts: Skip Launcher set, launching {} directly.", game->ExeName);
        spdlog::info("LauncherSkipsAndStarts: Launch command line: {}", Util::WideToUTF8(commandLine));

        PROCESS_INFORMATION processInfo{};
        STARTUPINFOW startupInfo{};
        startupInfo.cb = sizeof(startupInfo);

        if (CreateProcessW(gameExePath.c_str(), commandLine.data(), nullptr, nullptr, FALSE, 0, nullptr, gameExePath.parent_path().wstring().c_str(), &startupInfo, &processInfo))
        {
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);

            bForcedLauncherShutdown = true;
            spdlog::shutdown();
            ExitProcess(EXIT_SUCCESS);
        }

        spdlog::error("LauncherSkipsAndStarts: Failed to launch {} directly.", game->ExeName);
        return true;
    }
}

void LauncherSkipsAndStarts::Apply()
{
    if (!(eGameType & LAUNCHER))
    {
        return;
    }

    if (SkipLauncherEntirely())
    {
        return;
    }

    if (eJumpMode == JumpMode::Disabled)
    {
        return;
    }

    const char* jumpArg = (eJumpMode == JumpMode::DatabaseStart) ? "-jump dbstart" : "-jump gamestart";

    const std::string commandLineArgs = Util::GetCommandLineArgs();
    if (commandLineArgs.find("-jump gamestart") != std::string::npos || commandLineArgs.find("-jump dbstart") != std::string::npos)
    {
        spdlog::info("LauncherSkipsAndStarts: Launcher already jumpstarted, leaving command line as-is.");
        return;
    }

    spdlog::info("LauncherSkipsAndStarts: Relaunching launcher.exe with '{}'.", jumpArg);

    std::wstring relaunchCommandLine = L"\"" + (sExePath / "launcher.exe").wstring() + L"\" " + Util::UTF8toWide(jumpArg);

    PROCESS_INFORMATION processInfo{};
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    if (CreateProcessW(nullptr, relaunchCommandLine.data(), nullptr, nullptr, FALSE, 0, nullptr, sExePath.wstring().c_str(), &startupInfo, &processInfo))
    {
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        bForcedLauncherShutdown = true;
        spdlog::shutdown();
        ExitProcess(EXIT_SUCCESS);
    }

    spdlog::error("LauncherSkipsAndStarts: Failed to relaunch launcher.exe with jumpstart.");
}
