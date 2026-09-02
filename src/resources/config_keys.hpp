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

    constexpr const char* EnablePauseOnFocusLoss_Section = "Enhancements && Tweaks";
    constexpr const char* EnablePauseOnFocusLoss_Setting = "Pause On Focus Loss";
    constexpr const char* EnablePauseOnFocusLoss_Help = "";
    constexpr const char* EnablePauseOnFocusLoss_Tooltip = "Pauses the game when the window loses focus (alt-tabbed)";
    
   // constexpr const char* MSAASamples_Section = "Enhancements && Tweaks";
   // constexpr const char* MSAASamples_Setting = "Multisample Anti-Aliasing (MSAA)";
   // constexpr const char* MSAASamples_Help = "";
   // constexpr const char* MSAASamples_Tooltip = "Enables MSAA.\n"
   //                                             "\n"
   //                                             "0 = off / the game's default.";

    constexpr const char* LogCreateResults_Section = "Debugging";
    constexpr const char* LogCreateResults_Setting = "Log Create Results";
    constexpr const char* LogCreateResults_Help = "";
    constexpr const char* LogCreateResults_Tooltip = "Reports why the renderer rejects a framebuffer or texture.";

    constexpr const char* LogFrameBuffers_Section = "Debugging";
    constexpr const char* LogFrameBuffers_Setting = "Log Framebuffers";
    constexpr const char* LogFrameBuffers_Help = "";
    constexpr const char* LogFrameBuffers_Tooltip = "Logs which render targets are attached to each framebuffer.";

    constexpr const char* MSAALogTargets_Section = "Debugging";
    constexpr const char* MSAALogTargets_Setting = "MSAA Log Targets";
    constexpr const char* MSAALogTargets_Help = "";
    constexpr const char* MSAALogTargets_Tooltip = "Logs every render target as it is created, along with its final flags.";

    // on automatically w/ msaa.
   // constexpr const char* ShaderPerSampleCutouts_Section = "Enhancements && Tweaks";
   // constexpr const char* ShaderPerSampleCutouts_Setting = "Smooth Cutout Edges";
   // constexpr const char* ShaderPerSampleCutouts_Help = "";
   // constexpr const char* ShaderPerSampleCutouts_Tooltip = "Smooths the edges of foliage, fences and railings. MSAA alone cannot do this.";
//
//    constexpr const char* ShaderDumpShaders_Section = "Development Tools";
//    constexpr const char* ShaderDumpShaders_Setting = "Dump Shaders";
//    constexpr const char* ShaderDumpShaders_Help = "";
//    constexpr const char* ShaderDumpShaders_Tooltip = "Writes every shader the engine creates into a shader_dump folder in the game's root folder.";
//
//    constexpr const char* ShaderReplaceShaders_Section = "Development Tools";
//    constexpr const char* ShaderReplaceShaders_Setting = "Replace Shaders";
//    constexpr const char* ShaderReplaceShaders_Help = "";
//    constexpr const char* ShaderReplaceShaders_Tooltip = "Replaces shaders with matching files from a shader_replace folder in the game's root folder. Uses the same filenames as Dump Shaders.";
//
    constexpr const char* ShadowBufferSize_Section = "Enhancements && Tweaks";
    constexpr const char* ShadowBufferSize_Setting = "Custom Shadow Resolution";
    constexpr const char* ShadowBufferSize_Help = "(0 = Use Vanilla Game Setting)";
    constexpr const char* ShadowBufferSize_Tooltip = "By default, the game uses a mixture of shadows with resolutions of 512, 1024, and 2048 pixels.\n"
                                                     "\n"
                                                     "This will override all shadows to be the same consistent resolution.";

    // needs a bit more tweaking before release, there's some serious performance overhead & i didn't notice much in terms of fidelity improvement.
   // constexpr const char* ShadowSampleCount_Section = "Enhancements && Tweaks";
   // constexpr const char* ShadowSampleCount_Setting = "Shadow Sample Count";
   // constexpr const char* ShadowSampleCount_Help = "(0 = Use Vanilla Game Setting)";
   // constexpr const char* ShadowSampleCount_Tooltip = "Overrides how many samples are taken when filtering a shadow map.";

    constexpr const char* Ds3Support_Section = "Controller Settings";
    constexpr const char* Ds3Support_Setting = "Enable DualShock 3 Support";
    constexpr const char* Ds3Support_Help = "(Pressure Sensitive Buttons)";
    constexpr const char* Ds3Support_Tooltip = "Restores pressure controls, rumble and shake for Dualshock 3 controllers.\n"
                                               "\n"
                                               "Any pressure supporting button bound will use analog values.";

    constexpr const char* MouseRawInput_Section = "Mouse Settings";
    constexpr const char* MouseRawInput_Setting = "Use Raw Mouse Input";
    constexpr const char* MouseRawInput_Help = "(Disables Mouse Acceleration)";
    constexpr const char* MouseRawInput_Tooltip = "Reads mouse movement directly from the device.\n"
                                                  "\n"
                                                  "Bypasses Windows mouse acceleration and pointer speed settings.";

    constexpr const char* MouseSensitivityX_Section = "Mouse Settings";
    constexpr const char* MouseSensitivityX_Setting = "Mouse Horizontal Sensitivity";
    constexpr const char* MouseSensitivityX_Help = "";
    constexpr const char* MouseSensitivityX_Tooltip = "Horizontal camera speed. 1.0 is the game's default speed.";

    constexpr const char* MouseSensitivityY_Section = "Mouse Settings";
    constexpr const char* MouseSensitivityY_Setting = "Mouse Vertical Sensitivity";
    constexpr const char* MouseSensitivityY_Help = "";
    constexpr const char* MouseSensitivityY_Tooltip = "Vertical camera speed. 1.0 is the game's default speed.";

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

