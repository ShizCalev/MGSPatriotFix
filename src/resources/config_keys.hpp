#pragma once
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif


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


    constexpr const char* AnisotropicFiltering_Section = "Enhancements && Tweaks";
    constexpr const char* AnisotropicFiltering_Setting = "Anisotropic Filtering Level";
    constexpr const char* AnisotropicFiltering_Help = "";
    constexpr const char* AnisotropicFiltering_Tooltip = "Controls the level of anisotropic filtering applied to textures.\n"
                                                          "\n"
                                                          "Higher values improve texture detail while far away or at oblique angles.";

    constexpr const char* ForceDynamicResolutionOff_Section = "Enhancements && Tweaks";
    constexpr const char* ForceDynamicResolutionOff_Setting = "Disable Dynamic Resolution";
    constexpr const char* ForceDynamicResolutionOff_Help = "";
    constexpr const char* ForceDynamicResolutionOff_Tooltip = "The game agressively lowers its resolution to maintain framerate, even when it doesn't need to.\n"
                                                              "\n"
                                                              "Enabling this forces it to always render at full resolution instead.";

    constexpr const char* DisableMotionBlur_Section = "Enhancements && Tweaks";
    constexpr const char* DisableMotionBlur_Setting = "Disable Motion Blur";
    constexpr const char* DisableMotionBlur_Help = "";
    constexpr const char* DisableMotionBlur_Tooltip = "Disables the game's motion blur post-processing effect.";

    constexpr const char* Ds3Support_Section = "Controller Settings";
    constexpr const char* Ds3Support_Setting = "Enable DualShock 3 Support";
    constexpr const char* Ds3Support_Help = "(Pressure Sensitive Buttons)";
    constexpr const char* Ds3Support_Tooltip = "Restores pressure controls, rumble and shake for Dualshock 3 controllers.\n"
                                               "\n"
                                               "Any pressure supporting button bound will use analog values.";

    constexpr const char* Language_Section = "Language Settings";
    constexpr const char* Language_Setting = "Game Language";
    constexpr const char* Language_Help = "";
    constexpr const char* Language_Tooltip = "Selects in-game language.";

    constexpr const char* Region_Section = "Language Settings";
    constexpr const char* Region_Setting = "Game Region";
    constexpr const char* Region_Help = "";
    constexpr const char* Region_Tooltip = "Selects game region.";

    constexpr const char* CtrlType_Section = "Controller Settings";
    constexpr const char* CtrlType_Setting = "Button Icons";
    constexpr const char* CtrlType_Help = "";
    constexpr const char* CtrlType_Tooltip = "Selects which controller button icons to display in-game.";

    constexpr const char* CtrlType_Setting_PW = "Button Icons (PW)";


    constexpr const char* LauncherSkip_Section = "Launcher and Splashscreens";
    constexpr const char* LauncherSkip_Setting = "Skip Launcher Splashscreens";
    constexpr const char* LauncherSkip_Help = "";
    constexpr const char* LauncherSkip_Tooltip = "Automatically skips the launcher's splashscreens.\n"
                                                  "\n"
                                                  "Game Start = the Game Start menu\n"
                                                  "\n"
                                                  "Main Menu = the main menu w/ cursor on the MGS4 DB option.";
    constexpr const char* LauncherSkip_Option_Disabled = "Disabled";
    constexpr const char* LauncherSkip_Option_GameStart = "Game Start";
    constexpr const char* LauncherSkip_Option_DatabaseStart = "Main Menu";

    constexpr const char* LauncherSkip_Setting_PW = "Skip Launcher Splashscreens (PW)";
    constexpr const char* LauncherSkip_Tooltip_PW = "Automatically skips the launcher's splashscreens.\n"
                                                     "\n"
                                                     "Game Start = the Game Start menu.";

    constexpr const char* SkipLauncher_Section = LauncherSkip_Section;
    constexpr const char* SkipLauncher_Setting = "Skip Launcher";
    constexpr const char* SkipLauncher_Help = "";
    constexpr const char* SkipLauncher_Tooltip = "Skips the launcher app and runs the game directly.";

    constexpr const char* SkipSplashscreens_Section = LauncherSkip_Section;
    constexpr const char* SkipSplashscreens_Setting = "Skip In-Game Splashscreens";
    constexpr const char* SkipSplashscreens_Help = "";
    constexpr const char* SkipSplashscreens_Tooltip = "Skips the game's startup splashscreens and jumps straight to the main menu.";


    constexpr const char* GameResolution_PW_Setting = "Internal Resolution (PW)";
    constexpr const char* GameResolution_PW_Help = "";
    constexpr const char* GameResolution_PW_Tooltip = "The game's internal rendering resolution.";
    constexpr const char* GameResolution_PW_Option_Original = "Original";
    constexpr const char* GameResolution_PW_Option_FullHD = "FHD";

    constexpr const char* GameUpscale_PW_Setting = "Internal Upscaling (PW)";
    constexpr const char* GameUpscale_PW_Help = "";
    constexpr const char* GameUpscale_PW_Tooltip = "The resolution the game's output is upscaled to.";
    constexpr const char* GameUpscale_PW_Option_Original = "Original";
    constexpr const char* GameUpscale_PW_Option_FullHD = "FHD";
    constexpr const char* GameUpscale_PW_Option_WQHD = "WQHD";
    constexpr const char* GameUpscale_PW_Option_4K = "4K";

    constexpr const char* GameMovie_PW_Setting = "Cutscenes (PW)";
    constexpr const char* GameMovie_PW_Help = "";
    constexpr const char* GameMovie_PW_Tooltip = "Use original FMVs, or the Master Collection's remastered, higher-resolution ones.\n"
                                                 "\n"
                                                 "(Note: Konami didn't remaster ALL the FMV's, there are still some lower-resolution ones.)";
    constexpr const char* GameMovie_PW_Option_Original = "Original";
    constexpr const char* GameMovie_PW_Option_HighRes = "High-Resolution";


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


    constexpr const char* VerboseLogging_Section = "Debugging";
    constexpr const char* VerboseLogging_Setting = "Debug Logging";
    constexpr const char* VerboseLogging_Help = "";
    constexpr const char* VerboseLogging_Tooltip = "Enables verbose logging for debugging purposes.";

    constexpr const char* ControllerType_Auto = "AUTO";
    constexpr const char* ControllerType_Keyboard = "Keyboard";
    constexpr const char* ControllerType_XboxOne = "Xbox";
    constexpr const char* ControllerType_PS4 = "PlayStation 4";
    constexpr const char* ControllerType_PS5 = "PlayStation 5";
    constexpr const char* ControllerType_NintendoSwitch = "Nintendo Switch";

}



inline const std::initializer_list<std::string> kMGS4LauncherConfigCtrlTypes = {
    ConfigKeys::ControllerType_Auto,
    ConfigKeys::ControllerType_XboxOne,
    ConfigKeys::ControllerType_PS4,
    ConfigKeys::ControllerType_PS5,
    ConfigKeys::ControllerType_NintendoSwitch,
};

inline const std::initializer_list<std::string> kMGS4LauncherConfigCtrlTypesInternal = {
    "AUTO",
    "XBOX",
    "PS4",
    "PS5",
    "NX"
};

inline const std::initializer_list<std::string> kMGSPWLauncherConfigCtrlTypes = {
    ConfigKeys::ControllerType_Keyboard,
    ConfigKeys::ControllerType_XboxOne,
    ConfigKeys::ControllerType_PS4,
    ConfigKeys::ControllerType_PS5,
    ConfigKeys::ControllerType_NintendoSwitch,
};

inline const std::initializer_list<std::string> kMGSPWLauncherConfigCtrlTypesInternal = {
    "KBD",
    "XBOX",
    "PS4",
    "PS5",
    "NX"
};

inline const std::initializer_list<std::string> kMGSPWGameResolutionOptions = {
    ConfigKeys::GameResolution_PW_Option_Original,
    ConfigKeys::GameResolution_PW_Option_FullHD,
};

inline const std::initializer_list<std::string> kMGSPWGameUpscaleOptions = {
    ConfigKeys::GameUpscale_PW_Option_Original,
    ConfigKeys::GameUpscale_PW_Option_FullHD,
    ConfigKeys::GameUpscale_PW_Option_WQHD,
    ConfigKeys::GameUpscale_PW_Option_4K,
};

inline const std::initializer_list<std::string> kMGSPWGameMovieOptions = {
    ConfigKeys::GameMovie_PW_Option_Original,
    ConfigKeys::GameMovie_PW_Option_HighRes,
};


struct Game_Language_Pair_View
{
    std::string_view Region_Name;
    std::string_view Language_Name;
    std::string_view Game_Region;
    std::string_view Game_Language;
};

//Config Tool -> iTargetGame = TARGET_GAME_MGSPW;
inline constexpr std::array<Game_Language_Pair_View, 7> MGSPW_LanguagePairs =
{ {
    { "US / EU", "English",  "eu", "en" },
    { "US / EU", "French",  "eu", "fr" },
    { "US / EU", "Italian",  "eu", "it" },
    { "US / EU", "German",  "eu", "gr" },
    { "US / EU", "Spanish",  "eu", "sp" },
    { "US / EU", "Portuguese",  "eu", "pt" },
    { "Japan",   "Japanese", "jp", "jp" }
} };

//Config Tool -> iTargetGame = TARGET_GAME_MGS4
inline constexpr std::array<Game_Language_Pair_View, 7> MGS4_LanguagePairs =
{ {
    { "US / EU", "English",  "eu", "en" },
    { "US / EU", "French",  "eu", "fr" },
    { "US / EU", "Italian",  "eu", "it" },
    { "US / EU", "German",  "eu", "gr" },
    { "US / EU", "Spanish",  "eu", "sp" },
    { "US / EU", "Portuguese",  "eu", "pt" },
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

