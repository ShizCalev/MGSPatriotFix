#include "stdafx.h"

#include "common.hpp"
#include "launcher_skips_and_starts.hpp"

#include "config.hpp"
#include "logging.hpp"

namespace
{
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
        if (FindNewestLauncherSvPath().empty())
        {
            spdlog::warn("LauncherSkipsAndStarts: No launcher_sv found under {}, cannot Skip Launcher yet - run the launcher normally at least once first.", sGameSavePath.string());
            return std::nullopt;
        }

        const std::string resolution = std::to_string(LauncherSkipsAndStarts::iGameResolution);
        const std::string upscale = std::to_string(LauncherSkipsAndStarts::iGameUpscale);
        const std::string movie = std::to_string(LauncherSkipsAndStarts::iGameMovie);

        spdlog::info("LauncherSkipsAndStarts: resolution={}, upscale={}, movie={}", resolution, upscale, movie);

        return L" -resolution " + Util::UTF8toWide(resolution) + L" -upscale " + Util::UTF8toWide(upscale) + L" -movie " + Util::UTF8toWide(movie);
    }

    bool SkipLauncherEntirely()
    {
        if (!LauncherSkipsAndStarts::bSkipLauncher)
        {
            return false;
        }

        // game relaunched us (quit to launcher) -> wait for it to fully exit, then close
        // parent-process detection isn't reliable under Proton/Linux, so key off the game's own relaunch line instead
        const std::filesystem::path gameDir = sGameRootPath / game->GameSubfolder;
        const bool isMGS4Launcher = (game->ExeName == kGames.at(MGS4).ExeName);
        const std::string commandLineArgs = Util::GetCommandLineArgs();

        bool gameReturnedControl;
        if (isMGS4Launcher)
        {
            // "<sGameRootPath>\<GameSubfolder>\../launcher/launcher.exe" -jump gamestart
            const std::string expectedRelaunchLine = "\"" + (gameDir / "../launcher/launcher.exe").string() + "\" -jump gamestart";
            gameReturnedControl = commandLineArgs.find(expectedRelaunchLine) != std::string::npos;
        }
        else
        {
            gameReturnedControl = commandLineArgs.find("-jump gamestart") != std::string::npos;
        }

        if (gameReturnedControl)
        {
            spdlog::info("LauncherSkipsAndStarts: Launcher was started by the game returning to main menu.");
            spdlog::info("LauncherSkipsAndStarts: Waiting for {} to exit before closing.", game->ExeName);

            const std::filesystem::path gameExePath = gameDir / game->ExeName;
            while (Util::IsProcessRunning(gameExePath))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            spdlog::info("LauncherSkipsAndStarts: {} exited, closing launcher.", game->ExeName);
            bForcedLauncherShutdown = true;
            spdlog::shutdown();
            ExitProcess(EXIT_SUCCESS);
        }

        const std::filesystem::path gameExePath = gameDir / game->ExeName;

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
