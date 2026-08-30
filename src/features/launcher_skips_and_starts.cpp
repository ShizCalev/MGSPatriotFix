#include "stdafx.h"

#include "common.hpp"
#include "launcher_skips_and_starts.hpp"

#include "logging.hpp"

void LauncherSkipsAndStarts::Apply()
{
    if (!(eGameType & LAUNCHER) || eJumpMode == JumpMode::Disabled)
    {
        return;
    }

    const char* jumpArg = (eJumpMode == JumpMode::DatabaseStart) ? "-jump dbstart" : "-jump gamestart";

    const std::string commandLineArgs = Util::GetCommandLineArgs();
    if (commandLineArgs.find("-jump gamestart") != std::string::npos || commandLineArgs.find("-jump dbstart") != std::string::npos)
    {
        // A jump target is already on the command line (we're coming back from mgs4 or the mgs4 db) - don't overwrite it with our configured mode.
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
