#include "stdafx.h"

#include "common.hpp"
#include "logging.hpp"
#include "submodule_initiailization.hpp"
#include "config.hpp"
#include "config_keys.hpp"

///Resources

#include "version_checking.hpp"

///Features
//#include "custom_resolution_and_borderless.hpp"
//#include "intro_skip.hpp"

#include "windows_fullscreen_optimization.hpp"
#include "resolution_scaling_fixes.hpp"

//Warnings
#include "asi_loader_checks.hpp"

#include "background_shuffle_warning.hpp"
#include "check_gamesave_folder.hpp"
#include "d3d11_text_overlay.hpp"
//#include "color_correction.hpp"






#if !defined(RELEASE_BUILD)
#include "unit_tests.hpp"
#endif

static void Init_Miscellaneous()
{
 //   if (eGameType & (MG|MGS2|MGS3|LAUNCHER))
 //   {
 //       if (bDisableCursor)
 //       {
 //           // Launcher | MG/MG2 | MGS 2 | MGS 3: Disable mouse cursor
 //           if (uint8_t* MGS2_MGS3_MouseCursorScanResult = Memory::PatternScan(eGameType & LAUNCHER ? unityPlayer : baseModule, "BA ?? ?? ?? ?? 33 C9 FF 15", "Launcher | MG/MG2 | MGS 2 | MGS 3: Mouse Cursor"))
 //           {
 //               // The game enters 32512 in the RDX register for the function USER32.LoadCursorA to load IDC_ARROW (normal select arrow in windows)
 //               // Set this to 0 and no cursor icon is loaded
 //               Memory::PatchBytes((uintptr_t)MGS2_MGS3_MouseCursorScanResult + 0x2, "\x00", 1);
 //               spdlog::info("Launcher | MG/MG2 | MGS 2 | MGS 3: Mouse Cursor: Patched instruction.");
 //           }
 //       }
 //   }
 //
 //   if ((bDisableTextureFiltering || iAnisotropicFiltering > 0) && (eGameType & (MGS4|MGSPW)))
 //   {
 //       if (uint8_t* MGS3_SetSamplerStateInsnScanResult = Memory::PatternScan(baseModule, "48 8B 05 ?? ?? ?? ?? 44 39 8C 01 ?? ?? ?? ?? 74 ?? 44 89 8C 01 ?? ?? ?? ?? EB ?? 48 63 C2 48 6B C8 ?? 48 8B 05 ?? ?? ?? ?? 44 39 8C 01 ?? ?? ?? ?? 74 ?? 44 89 8C 01 ?? ?? ?? ?? EB", "MGS 2 | MGS 3: Texture Filtering"))
 //       {
 //           static SafetyHookMid SetSamplerStateInsnXMidHook{};
 //           SetSamplerStateInsnXMidHook = safetyhook::create_mid(MGS3_SetSamplerStateInsnScanResult + 0x7,
 //               [](SafetyHookContext& ctx)
 //               {
 //                   // [rcx+rax+0x438] = D3D11_SAMPLER_DESC, +0x14 = MaxAnisotropy
 //                   *reinterpret_cast<int*>(ctx.rcx + ctx.rax + 0x438 + 0x14) = iAnisotropicFiltering;
 //
 //                   // Override filter mode in r9d with aniso value and run compare from orig game code
 //                   // Game code will then copy in r9d & update D3D etc when r9d is different to existing value
 //                   //0x1 = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR (Linear mips is essentially perspective correction.) 0x55 = D3D11_FILTER_ANISOTROPIC
 //                   ctx.r9 = bDisableTextureFiltering ? 0x1 : 0x55;
 //               });
 //           LOG_HOOK(SetSamplerStateInsnXMidHook, "MGS 2 | MGS 3: Texture Filtering")
 //       }
 //
 //   }

}

//static bool forcedLauncherShutdown = false;

static void Init_LauncherConfigOverride()
{
    // If we know games steam appid, try creating steam_appid.txt file, so that game EXE can be launched directly in future runs
    if (game)
    {
        const std::filesystem::path steamAppidPath = sExePath.parent_path() / "steam_appid.txt";

        try
        {
            if (!std::filesystem::exists(steamAppidPath))
            {
                spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: Creating steam_appid.txt to allow direct EXE launches.");
                std::ofstream steamAppidOut(steamAppidPath);
                if (steamAppidOut.is_open())
                {
                    steamAppidOut << game->SteamAppId;
                    steamAppidOut.close();
                }
                if (std::filesystem::exists(steamAppidPath))
                {
                    spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: steam_appid.txt created successfully.");
                }
                else
                {
                    spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: steam_appid.txt creation failed.");
                }
            }
        }
        catch (const std::exception& ex)
        {
            spdlog::error("MG/MG2 | MGS 2 | MGS 3: Launcher Config: Launcher Config: steam_appid.txt creation failed (exception: %s)", ex.what());
        }
    }

    LPWSTR commandLine = GetCommandLineW();

    if (eGameType & LAUNCHER)
    {
    //    bool hasJumpstart = wcsstr(commandLine, L"-jump gamestart");
    //
    //    if (bLauncherConfigSkipLauncher)
    //    {
    //
    //        if (!hasJumpstart || Util::IsProcessParent("steam.exe"))
    //        {
    //            auto gameExePath = sExePath.parent_path() / game->ExeName;
    //
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: SkipLauncher set, attempting game launch");
    //
    //            PROCESS_INFORMATION processInfo {};
    //            STARTUPINFO startupInfo {};
    //            startupInfo.cb = sizeof(STARTUPINFO);
    //
    //            std::wstring commandLine = L"\"" + gameExePath.wstring() + L"\"";
    //
    //            if (game->ExeName == kGames.at(MG).ExeName)
    //            {
    //                // Add launch parameters for MG MSX
    //                auto transformString = [](const std::string& input, int (*transformation)(int)) -> std::wstring
    //                    {
    //                        std::string transformedString = input;
    //                        std::transform(transformedString.begin(), transformedString.end(), transformedString.begin(), transformation);
    //                        return Util::UTF8toWide(transformedString);
    //                    };
    //
    //                commandLine += L" -mgst " + std::wstring(sLauncherConfigMSXGame == ConfigKeys::SkipLauncherMSX_Option_MG1 ? L"mg1" : L"mg2"); // -mgst must be lowercase
    //            }
    //
    //            commandLine += L" -region " + Util::UTF8toWide(sSkipLauncherRegion) + L" -lan " + Util::UTF8toWide(sSkipLauncherLanguage) + L" -selfregion EU -launcherpath launcher.exe" + L" -ctrltype " + Util::UTF8toWide(Util::GetUppercaseNameAtIndex(kLauncherConfigCtrlTypesInternal, iLauncherConfigCtrlType));
    //            std::string sCommandLine = Util::WideToUTF8(commandLine);
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: Launch command line: {}", sCommandLine);
    //
    //            // Call CreateProcess to start the game process
    //            if (CreateProcessW(nullptr, commandLine.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo))
    //            {
    //                // Successfully started the process
    //                CloseHandle(processInfo.hProcess);
    //                CloseHandle(processInfo.hThread);
    //
    //                // Force launcher to exit
    //                forcedLauncherShutdown = true;
    //                spdlog::shutdown();
    //                ExitProcess(EXIT_SUCCESS);
    //            }
    //            else
    //            {
    //                spdlog::error("MG/MG2 | MGS 2 | MGS 3: Launcher Config: SkipLauncher failed to create game EXE process");
    //            }
    //
    //        }
    //        else //hasJumpstart && bLauncherConfigSkipLauncher -> we reentered the launcher from the main game. lets terminate once the game finishes closing.
    //        {
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher jumpstart detected on commandline.");
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Waiting for companion game to exit before terminating launcher.");
    //            while (Util::IsProcessRunning(sExePath / game->ExeName))
    //            {
    //                std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //            }
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Companion game exited, exiting launcher.");
    //            forcedLauncherShutdown = true;
    //            spdlog::shutdown();
    //            ExitProcess(EXIT_SUCCESS);
    //        }
    //    }
    //    else if (bLauncherJumpStart)
    //    {
    //        if (!hasJumpstart)
    //        {
    //
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: JumpStart set, attempting to restart launcher with -jump gamestart");
    //            std::filesystem::path gameExePath = sExePath.parent_path() / "launcher.exe";
    //
    //            PROCESS_INFORMATION processInfo = {};
    //            STARTUPINFO startupInfo = {};
    //            startupInfo.cb = sizeof(STARTUPINFO);
    //            std::wstring commandLine = L"\"" + gameExePath.wstring() + L"\"";
    //            commandLine += L" -jump gamestart";
    //            if (CreateProcess(nullptr, (LPWSTR)commandLine.c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo))
    //            {
    //                // Successfully started the process
    //                CloseHandle(processInfo.hProcess);
    //                CloseHandle(processInfo.hThread);
    //
    //                // Force launcher to exit
    //                forcedLauncherShutdown = true;
    //                spdlog::shutdown();
    //                ExitProcess(EXIT_SUCCESS);
    //            }
    //            spdlog::error("MG/MG2 | MGS 2 | MGS 3: Launcher Config: Failed to restart launcher with jumpstart.");
    //        }
    //        else
    //        {
    //            spdlog::info("MG/MG2 | MGS 2 | MGS 3: Launcher Config: Launcher Jumpstarted.");
    //        }
    //    }
    //
    //    return;
    }



}


static bool DetectGame()
{
    eGameType = UNKNOWN;
    // Special handling for launcher.exe
    if (bIsLauncher)
    {
        for (const auto& [type, info] : kGames)
        {
            const std::filesystem::path gameSubfolderPath = Util::FindSubfolderCaseInsensitive(sGameRootPath, info.GameSubfolder);
            if (gameSubfolderPath.empty())
            {
                continue;
            }

            auto gamePath = gameSubfolderPath / info.ExeName;
            if (std::filesystem::exists(gamePath))
            {
                spdlog::info("Detected launcher for game: {} (app {})", info.GameTitle.c_str(), info.SteamAppId);
                eGameType = LAUNCHER;
                unityPlayer = GetModuleHandleA("UnityPlayer.dll");
                game = &info;
                sGameSavePath = sGameRootPath / ((game->ExeName == kGames.at(MGS4).ExeName) ? "mgs4_savedata_win" : "mgspw_savedata_win");
                return true;
            }
        }

        spdlog::error("Failed to detect supported game, unknown launcher");
        FreeLibraryAndExitThread(baseModule, 1);
    }

    for (const auto& [type, info] : kGames)
    {
        if (info.ExeName == sExeName)
        {
            spdlog::info("Detected game: {} (app {})", info.GameTitle.c_str(), info.SteamAppId);
            eGameType = type;
            game = &info;

            sGameSavePath = sGameRootPath / ((game->ExeName == kGames.at(MGS4).ExeName) ? "mgs4_savedata_win" : "mgspw_savedata_win");
            spdlog::info("Game Save Path: {}", sGameSavePath.string());
            //if (engineModule = GetModuleHandleA("Engine.dll"); !engineModule)
            //{
            //    spdlog::error("Failed to get Engine.dll module handle");
            //}

            return true;
        }
    }

    spdlog::error("Failed to detect supported game, {} isn't supported by MGSPatriotFix", sExeName.c_str());
    FreeLibraryAndExitThread(baseModule, 1);
}



void afterPresent()
{
    static bool bInitialized = false;
    if (bInitialized)
    {
        spdlog::warn("afterPresent() called multiple times, skipping initialization.");
        return;
    }
    bInitialized = true;
    spdlog::info("afterPresent() started");


    D3D11TextOverlay::Init();
    spdlog::info("afterPresent() completed");
}

static void InitializeSubsystems()
{
    //Initialization order (these systems initialize vars used by following ones.)
    INITIALIZE(g_Logging.LogSysInfo());            //0
    INITIALIZE(DetectGame());                      //1
    //INITIALIZE(ASILoaderCompatibility::Check());   //2
    INITIALIZE(Config::Read());                    //3
    //INITIALIZE(g_GameVars.Initialize());           //4
    //INITIALIZE(g_D3D11Hooks.Initialize());         //5 Caches the D3DDevice, DXGIFactory, and D3DContext from D3DCreateDevice/DXGICreateFactory
    //INITIALIZE(Init_LauncherConfigOverride());     //7

    
    //INITIALIZE(ColorCorrection::Setup());

    if (eGameType & MGS4)
    {

        //INITIALIZE(ResolutionScalingFixes::ApplyFixes());

    }
    else if (eGameType & MGSPW)
    {
        //INITIALIZE(ResolutionScalingFixes::ApplyFixes());

    }

    INITIALIZE(FixFullscreenOptimization::Fix());
    //INITIALIZE(D3D11TextOverlay::Setup());


        //Warnings
    INITIALIZE(BackgroundShuffleWarning::Check());
    //INITIALIZE(CheckGamesaveFolderWritable::CheckStatus());


    //INITIALIZE(CheckForUpdates());

#if !defined(RELEASE_BUILD)
    INITIALIZE(UnitTests::runAllTests());

#endif

    INITIALIZE(Util::ShutdownSHA1Provider());
}

std::mutex mainThreadFinishedMutex;
std::condition_variable mainThreadFinishedVar;
bool mainThreadFinished = false;

DWORD __stdcall Main(void*)
{
    g_Logging.initStartTime = std::chrono::high_resolution_clock::now();
    Logging::Initialize();

    INITIALIZE(InitializeSubsystems());

    // Signal any threads (e.g., the memset hook) that are waiting for initialization to finish.
    {
        std::lock_guard lock(mainThreadFinishedMutex);
        mainThreadFinished = true;
    }
    mainThreadFinishedVar.notify_all();

    return TRUE;
}

std::mutex memsetHookMutex;
bool memsetHookCalled = false;
static void* (__cdecl* memset_Fn)(void* Dst, int Val, size_t Size); // Pointer to the next function in the memset chain (could be another hook or the real CRT memset).
static void* __cdecl memset_Hook(void* Dst, int Val, size_t Size) // Our memset hook, which blocks the game's main thread until initialization is complete.
{
    std::lock_guard lock(memsetHookMutex);

    if (!memsetHookCalled)
    {
        memsetHookCalled = true;

        // Restore the original (or previously-hooked) memset in the IAT.
        // This ensures future memset calls bypass our hook and run at full speed.
        Memory::WriteIAT(baseModule, "VCRUNTIME140.dll", "memset", memset_Fn);

        // Block the current thread here until our main initialization is complete.
        std::unique_lock finishedLock(mainThreadFinishedMutex);
        mainThreadFinishedVar.wait(finishedLock, []
            {
                return mainThreadFinished;
            });
    }

    // Forward the memset call to the next function (another hook or the real memset).
    return reinterpret_cast<decltype(memset_Fn)>(memset_Fn)(Dst, Val, Size);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    //if (forcedLauncherShutdown)
    //{
    //    return TRUE;
    //}
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        if (GetModuleHandleA("VCRUNTIME140.dll"))
        {
            // Read the current IAT entry for memset in the base module.
            // Note: it may already point to another mod's hook if they loaded first.
            void* currentIATMemset = Memory::ReadIAT(baseModule, "VCRUNTIME140.dll", "memset");

            // Save the current pointer so we can call it later (chaining to the next hook or real memset).
            memset_Fn = reinterpret_cast<decltype(memset_Fn)>(currentIATMemset);

            // Overwrite the IAT entry with our memset_Hook, so our code intercepts memset calls.
            // We always overwrite unconditionally to ensure our hook is active.
            // This will prevent other mods that also hook memset from unpausing the main thread before our Main() has finished.
            Memory::WriteIAT(baseModule, "VCRUNTIME140.dll", "memset", &memset_Hook);
        }

        // Create our main thread, which runs the initialization logic.
        if (const HANDLE mainHandle = CreateThread(nullptr, 0, Main, nullptr, CREATE_SUSPENDED, nullptr))
        {
            SetThreadPriority(mainHandle, THREAD_PRIORITY_TIME_CRITICAL); // Give our thread higher priority than the game's.
            ResumeThread(mainHandle);
            CloseHandle(mainHandle);
        }

        // Prevent monitor or system sleep while the game is running.
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        spdlog::shutdown();
    }
    return TRUE;
}
