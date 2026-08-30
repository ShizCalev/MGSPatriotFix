#include "stdafx.h"


#include "common.hpp"
#include "logging.hpp"
#include "submodule_initiailization.hpp"
#include "config.hpp"
#include "config_keys.hpp"
#include "version.h"

///Resources

#include "version_checking.hpp"

///Features
//#include "custom_resolution_and_borderless.hpp"
//#include "intro_skip.hpp"

#include "resolution_scaling_fixes.hpp"
#include "graphics_tuning.hpp"
#include "launcher_skips_and_starts.hpp"

//Warnings
#include "asi_loader_checks.hpp"

#include "background_shuffle_warning.hpp"
#include "check_gamesave_folder.hpp"
#include "d3d11_text_overlay.hpp"
//#include "color_correction.hpp"






#if !defined(RELEASE_BUILD)
#include "unit_tests.hpp"
#endif

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
                spdlog::info("UnityPlayer.dll module handle: 0x{0:X}", (uintptr_t)unityPlayer);
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

namespace
{
    // Lets us direct launch the main game's .exe
    void CreateSteamAppIdFile()
    {
        if (!game)
        {
            return;
        }

        const std::filesystem::path steamAppidPath = sExePath / "steam_appid.txt";
        if (std::filesystem::exists(steamAppidPath))
        {
            return;
        }

        try
        {
            spdlog::info("Creating steam_appid.txt to allow direct EXE launches.");
            std::ofstream steamAppidOut(steamAppidPath);
            if (steamAppidOut.is_open())
            {
                steamAppidOut << game->SteamAppId;
                steamAppidOut.close();
            }

            if (std::filesystem::exists(steamAppidPath))
            {
                spdlog::info("steam_appid.txt created successfully.");
            }
            else
            {
                spdlog::error("steam_appid.txt creation failed.");
            }
        }
        catch (const std::exception& ex)
        {
            spdlog::error("steam_appid.txt creation failed (exception: {})", ex.what());
        }
    }
}

static void InitializeSubsystems()
{
    //Initialization order (these systems initialize vars used by following ones.)
    INITIALIZE(g_Logging.LogSysInfo());            //0
    INITIALIZE(DetectGame());                      //1
    INITIALIZE(CreateSteamAppIdFile());
    INITIALIZE(ASILoaderCompatibility::Check());   //2
    INITIALIZE(Config::Read());                    //3
    //INITIALIZE(g_GameVars.Initialize());           //4
    //INITIALIZE(g_D3D11Hooks.Initialize());         //5 Caches the D3DDevice, DXGIFactory, and D3DContext from D3DCreateDevice/DXGICreateFactory
    INITIALIZE(LauncherSkipsAndStarts::Apply());   //7

    

    INITIALIZE(GraphicsTuning::ApplyHooks());

    if (eGameType & MGS4)
    {

        //INITIALIZE(ResolutionScalingFixes::ApplyFixes());

    }
    else if (eGameType & MGSPW)
    {
        //INITIALIZE(ResolutionScalingFixes::ApplyFixes());

    }

    //INITIALIZE(D3D11TextOverlay::Setup());


        //Warnings
    INITIALIZE(BackgroundShuffleWarning::Check());
    //INITIALIZE(CheckGamesaveFolderWritable::CheckStatus());


    //INITIALIZE(CheckForUpdates());

#if !defined(RELEASE_BUILD)
    INITIALIZE(UnitTests::runAllTests());

#endif

    //INITIALIZE(Util::ShutdownSHA1Provider());
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

static HANDLE hInitMutex = nullptr;

static bool IsAlreadyInitialized()
{
    const std::wstring mutexName = std::format(L"Local\\{}_Init_{}", Util::UTF8toWide(sFixName), GetCurrentProcessId());

    hInitMutex = CreateMutexW(nullptr, FALSE, mutexName.c_str());

    if (!hInitMutex)
    {
        return false;
    }

    return GetLastError() == ERROR_ALREADY_EXISTS;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (bForcedLauncherShutdown)
    {
        return TRUE;
    }
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        if (IsAlreadyInitialized())
        {
            return TRUE;
        }

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
    return TRUE;
}
