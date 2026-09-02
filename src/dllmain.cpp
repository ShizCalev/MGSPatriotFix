#include "stdafx.h"


#include "common.hpp"
#include "logging.hpp"
#include "submodule_initiailization.hpp"
#include "config.hpp"

///Resources

#include "version_checking.hpp"

///Features
//#include "custom_resolution_and_borderless.hpp"
//#include "intro_skip.hpp"

#include "graphics_tuning.hpp"
#include "launcher_skips_and_starts.hpp"
#include "pressure_inputs.hpp"
#include "pad_motion.hpp"
#include "ds3_rumble.hpp"
#include "skip_splashscreens.hpp"
#include "various_tweaks.hpp"

//Warnings
#include "asi_loader_checks.hpp"

//#include "d3d11_text_overlay.hpp"






#if !defined(RELEASE_BUILD)
#include "unit_tests.hpp"
#endif


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

    bool DetectGame()
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

    void InitializeSubsystems()
    {
        //Initialization order (these systems initialize vars used by following ones.)
        INITIALIZE(g_Logging.LogSysInfo());            //0
        INITIALIZE(DetectGame());                      //1
        INITIALIZE(CreateSteamAppIdFile());
        INITIALIZE(ASILoaderCompatibility::Check());   //2
        INITIALIZE(Config::Read());                    //3
        //INITIALIZE(g_GameVars.Initialize());           //4
        //INITIALIZE(g_D3D11Hooks.Initialize());         //5 Caches the D3DDevice, DXGIFactory, and D3DContext from D3DCreateDevice/DXGICreateFactory
        
        if (eGameType & LAUNCHER)
        {
            INITIALIZE(LauncherSkipsAndStarts::Apply());
        }



        INITIALIZE(GraphicsTuning::Apply());
        INITIALIZE(VariousTweaks::Apply());

        if (eGameType & MGS4)
        {

            //INITIALIZE(ResolutionScalingFixes::ApplyFixes());
            INITIALIZE(PressureInputs::Initialize());
            INITIALIZE(PadMotion::Initialize());
            INITIALIZE(Ds3Rumble::Initialize());
        	INITIALIZE(SkipSplashscreens::Apply());


        }
        else if (eGameType & MGSPW)
        {
            //INITIALIZE(ResolutionScalingFixes::ApplyFixes());

        }

        //INITIALIZE(D3D11TextOverlay::Setup());


            //Warnings


        INITIALIZE(CheckForUpdates());

#if !defined(RELEASE_BUILD)
        INITIALIZE(UnitTests::runAllTests());

#endif

    //INITIALIZE(Util::ShutdownSHA1Provider());
    }





    void RunInit()
    {
        static std::once_flag flag;
        std::call_once(flag, []
        {
            g_Logging.initStartTime = std::chrono::high_resolution_clock::now();
            Logging::Initialize();

            INITIALIZE(InitializeSubsystems());
        });
    }

}


// Ultimate ASI Loader calls this right after DllMain returns
extern "C" __declspec(dllexport) void InitializeASI()
{
    RunInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (bForcedLauncherShutdown)
    {
        return TRUE;
    }
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
    }
    return TRUE;
}
