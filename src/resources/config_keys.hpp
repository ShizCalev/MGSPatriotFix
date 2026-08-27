#pragma once
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <initializer_list>

namespace ConfigKeys
{
    // Graphics

    constexpr const char* DisableFullscreenOptimization_Section = "System Specific Fixes";
    constexpr const char* DisableFullscreenOptimization_Setting = "Disable Windows Fullscreen Optimization";
    constexpr const char* DisableFullscreenOptimization_Help =    "(Fixes brief freezes when alt-tabbing on some systems.)";
    constexpr const char* DisableFullscreenOptimization_Tooltip = "Sets Windows compatibility settings to disable Fullscreen Optimization for the game process.\n"
                                                                  "\n"
                                                                  "Performance impact is unknown.\n"
                                                                  "Leave this OFF unless you are experiencing framerate issues when alt-tabbing and want to experiment.\n"
                                                                  "\n"
                                                                  "This writes to:\n"
                                                                  "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\n"
                                                                  "\n"
                                                                  "Equivalent to: Right-click the game's .exe -> Properties -> Compatibility -> check \"Disable Fullscreen Optimizations\"";

    constexpr const char* ForceDedicatedGPU_Section = "System Specific Fixes";
    constexpr const char* ForceDedicatedGPU_Setting = "Force Dedicated GPU";
    constexpr const char* ForceDedicatedGPU_Help = "";
    constexpr const char* ForceDedicatedGPU_Tooltip = "Sets Windows power preference settings to force the\n"
                                                      "game to use your dedicated GPU.\n"
                                                      "\n"
                                                      "This writes \"GpuPreference=2;\" for the game's executable to:\n"
                                                      "HKEY_CURRENT_USER\\Software\\Microsoft\\DirectX\\UserGpuPreferences\n"
                                                      "\n"
                                                      "Equivalent to:\n"
                                                      "Right-click desktop -> Display settings -> Graphics settings ->\n"
                                                      "Browse -> Select the game's .exe -> Options -> Prefer High performance";




    constexpr const char* Language_Section = "Language Settings";
    constexpr const char* Language_Setting = "Game Language";
    constexpr const char* Language_Help = "";
    constexpr const char* Language_Tooltip = "Selects in-game language.";

    constexpr const char* Region_Section = "Language Settings";
    constexpr const char* Region_Setting = "Game Region";
    constexpr const char* Region_Help = "";
    constexpr const char* Region_Tooltip = "Selects game region.";


    //constexpr const char* DisableMouseCursor_Section = "Bugfixes";
    //constexpr const char* DisableMouseCursor_Setting = "Fix Mouse Cursor Showing";
    //constexpr const char* DisableMouseCursor_Help = "";
    //constexpr const char* DisableMouseCursor_Tooltip = "Stops the mouse cursor from showing in the launcher and game.";
    //
    //
    //constexpr const char* ShowSpeedrunnerOverlay_Section = "Speedrunner Settings";
    //constexpr const char* ShowSpeedrunnerOverlay_Setting = "Gameplay Stats Overlay";
    //constexpr const char* ShowSpeedrunnerOverlay_Help = "";
    //constexpr const char* ShowSpeedrunnerOverlay_Tooltip = "Displays an overlay with current stage time / in-game timer (IGT) / elasped time.\n"
    //                                                       "\n"
    //                                                       "Extended stats are also shown while the game is paused.";
    //constexpr const char* ShowSpeedrunnerOverlay_Option_Disabled = "Disabled";
    //constexpr const char* ShowSpeedrunnerOverlay_Option_TopLeft = "Top Left";
    //constexpr const char* ShowSpeedrunnerOverlay_Option_TopRight = "Top Right";
    //constexpr const char* ShowSpeedrunnerOverlay_Option_BottomLeft = "Bottom Left";
    //constexpr const char* ShowSpeedrunnerOverlay_Option_BottomRight = "Bottom Right";



    // Internal
    constexpr const char* CheckForUpdates_Section = "Update Notifications";
    constexpr const char* CheckForUpdates_Setting = "Check For MGSPatriotFix Updates";
    constexpr const char* CheckForUpdates_Help = "";
    constexpr const char* CheckForUpdates_Tooltip = "If MGSPatriotFix should notify you when launching the game if a new MGSPatriotFix update is available for download.";

    constexpr const char* UpdateConsoleNotifications_Section = "Update Notifications";
    constexpr const char* UpdateConsoleNotifications_Setting = "In-Game Update Notifications";
    constexpr const char* UpdateConsoleNotifications_Help = "";
    constexpr const char* UpdateConsoleNotifications_Tooltip = "If you want a visible notification when starting the game if an MGSPatriotFix update is available.\n"
                                                               "\n"
                                                               "Notifications will still be printed to the log file while disabled.";


    constexpr const char* WindowsSlideshowWarning_Section = "Enable Game Warnings";
    constexpr const char* WindowsSlideshowWarning_Setting = "Warn When Windows Slideshow Enabled";
    constexpr const char* WindowsSlideshowWarning_Help = "";
    constexpr const char* WindowsSlideshowWarning_Tooltip = "Having Windows wallpaper set to Slideshow / Window Spotlight mode is known to cause stuttering while in DirectX games.\n"
                                                            "\n"
                                                            "This will provide a warning when the Windows setting is enabled.";


    constexpr const char* SaveFolderWriteWarning_Section = "Enable Game Warnings";
    constexpr const char* SaveFolderWriteWarning_Setting = "Warn When Save Folders Not Writable";
    constexpr const char* SaveFolderWriteWarning_Help = "";
    constexpr const char* SaveFolderWriteWarning_Tooltip = "Warn the user when the save folder is not writable by the game, which breaks the game's ability to save.";

    constexpr const char* SaveFileReadOnlyWarning_Section = "Enable Game Warnings";
    constexpr const char* SaveFileReadOnlyWarning_Setting = "Warn When Save Files Are Read-Only";
    constexpr const char* SaveFileReadOnlyWarning_Help = "";
    constexpr const char* SaveFileReadOnlyWarning_Tooltip = "Warn the user when individual save files are set to read only, which breaks the game's ability to save.";

    constexpr const char* VerboseLogging_Section = "Debugging";
    constexpr const char* VerboseLogging_Setting = "Debug Logging";
    constexpr const char* VerboseLogging_Help = "";
    constexpr const char* VerboseLogging_Tooltip = "Enables verbose logging for debugging purposes.";


}


struct Game_Language_Pair_View
{
    std::string_view Region_Name;
    std::string_view Language_Name;
    std::string_view Game_Region;
    std::string_view Game_Language;
};

//Config Tool -> iTargetGame = TARGET_GAME_MGS3;
inline constexpr std::array<Game_Language_Pair_View, 9> MGSPW_LanguagePairs =
{ {
    { "North America", "English",   "us", "en" },

    { "Japan",         "Japanese",  "jp", "jp" }
} };

//Config Tool -> iTargetGame = TARGET_GAME_MG1 
//Config Tool -> iTargetGame = TARGET_GAME_MGS2
inline constexpr std::array<Game_Language_Pair_View, 6> MGS4_LanguagePairs =
{ {
    { "US / EU", "English",  "eu", "en" },

    { "Japan",   "Japanese", "jp", "jp" }
} };

template <size_t N>
static bool IsValidRegionLanguagePair(const std::array<Game_Language_Pair_View, N>& pairs, std::string_view region, std::string_view language)
{
    for (const auto& p : pairs)
    {
        if (p.Game_Region == region && p.Game_Language == language) return true;
    }
    return false;
}

template <size_t N>
static bool ResolveRegionLanguageNames(const std::array<Game_Language_Pair_View, N>& pairs, std::string_view game_region, std::string_view game_language, std::string& out_region_name, std::string& out_language_name)
{
    for (const auto& p : pairs)
    {
        if (p.Game_Region != game_region)
        {
            continue;
        }

        if (p.Game_Language != game_language)
        {
            continue;
        }

        out_region_name.assign(p.Region_Name);
        out_language_name.assign(p.Language_Name);
        return true;
    }

    return false;
}

