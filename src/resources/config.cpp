#include "stdafx.h"

#include "common.hpp"
#include "config.hpp"
#include "pressure_inputs.hpp"

#include "inipp/inipp.h"

#include "input_handler.hpp"
#include "logging.hpp"
#include "background_shuffle_warning.hpp"
#include "check_gamesave_folder.hpp"
#include "version_checking.hpp"
#include "config_keys.hpp"
#include "d3d11_text_overlay.hpp"
#include "game_funcs.hpp"
#include "graphics_tuning.hpp"
#include "launcher_skips_and_starts.hpp"
#include "skip_splashscreens.hpp"

// -----------------------------------------------------------------------------
// ConfigHelper: A type-safe, case-insensitive, error-checked INI config reader.
// Automatically logs missing/invalid values and exits the thread immediately.
// By Afevis/ShizCalev, 2025.
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Config Logger: caches config values for sorted flush at the end
// -----------------------------------------------------------------------------
namespace ConfigLogger
{
    inline std::map<std::string, std::map<std::string, std::string>> cache;

    template <typename T>
    void Cache(const char* section, const char* setting, const T& value)
    {
        cache[section][setting] = fmt::format("{}", value);
    }

    void Flush()
    {
        spdlog::info("---------- Config Parse Results ----------");
        for (auto& sec : cache)
        {
            spdlog::info("[{}]", sec.first);
            for (auto& kv : sec.second)
            {
                spdlog::info("    {} = {}", kv.first, kv.second);
            }
        }
        spdlog::info("---------- End Config Parse ----------");
    }
}

#define LOG_CONFIG(section, setting, value) \
    ConfigLogger::Cache(section, setting, value)

namespace ConfigHelper
{
    inline void FatalConfigError(const std::string& section, const std::string& key, const std::string& reason)
    {
        std::string message = "[" + sFixName + " Config Helper] Failed to read config key '" + key +
            "' in section '" + section + "': " + reason;

        spdlog::error(message);
        spdlog::error("Please run the {} to update your settings file.", sFixName + " Config Tool");
        Logging::ShowConsole();
        std::cout << message << std::endl;
        std::cout << "Please run the " << sFixName + " Config Tool" << " to update your settings file." << std::endl;

        FreeLibraryAndExitThread(baseModule, 1);
    }

    /// Internal parsing helper
    template <typename T>
    bool TryParse(const std::string& str, T& out)
    {
        std::istringstream iss(str);
        return (iss >> std::boolalpha >> out) ? true : false;
    }

    /// Parses bool values with case-insensitivity and common boolean strings
    template <>
    inline bool TryParse<bool>(const std::string& str, bool& out)
    {
        std::string val = str;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        if (val == "1" || val == "true" || val == "yes" || val == "on")
        {
            out = true;
            return true;
        }
        if (val == "0" || val == "false" || val == "no" || val == "off")
        {
            out = false;
            return true;
        }
        return false;
    }

    /// Generic value loader from INI with hard error on failure
    template <typename T>
    void getValue(const inipp::Ini<char>& ini, const std::string& section, const std::string& key, T& out)
    {
        auto secIt = ini.sections.find(section);
        if (secIt == ini.sections.end())
            FatalConfigError(section, key, "Section not found");

        const auto& keyvals = secIt->second;
        auto keyIt = keyvals.find(key);
        if (keyIt == keyvals.end())
            FatalConfigError(section, key, "Key not found");

        if (!TryParse<T>(keyIt->second, out))
            FatalConfigError(section, key, "Failed to parse value '" + keyIt->second + "'");
    }

    /// Specialization for std::string values (handles quotes)
    template <>
    inline void getValue<std::string>(const inipp::Ini<char>& ini, const std::string& section, const std::string& key, std::string& out)
    {
        auto secIt = ini.sections.find(section);
        if (secIt == ini.sections.end())
            FatalConfigError(section, key, "Section not found");

        const auto& keyvals = secIt->second;
        auto keyIt = keyvals.find(key);
        if (keyIt == keyvals.end())
            FatalConfigError(section, key, "Key not found");

        out = Util::StripQuotes(keyIt->second);
    }
}


namespace
{

}

void Config::Read()
{
    std::filesystem::path sConfigFile = sFixName + ".settings";

    std::ifstream iniFile((sGameRootPath / sConfigFile).string());
    if (!iniFile)
    {
        spdlog::error("CONFIG ERROR: File not found: {}", (sGameRootPath / sConfigFile).string());
        spdlog::error("Make sure that you've run the {} (in your game's root folder) to generate your settings file.", sFixName + " Config Tool");
        spdlog::error("and that {} is located in {}", sConfigFile.string(), sGameRootPath.string());
        Logging::ShowConsole();
        std::cout << "" << sFixName << " v" << sFixVersion << " loaded." << std::endl;
        std::cout << "ERROR: File not found: " << (sGameRootPath / sConfigFile).string() << std::endl;
        std::cout << "ERROR: Make sure that you've run the " << sFixName + " Config Tool" << " (in your game's root folder) to generate your settings file." << std::endl;
        std::cout << "ERROR: And that " << sConfigFile << " is located in " << sGameRootPath << "\n" << std::endl;
        if (Util::IsSteamOS())
        {
            std::cout << "ERROR: When launching the MGSPatriotFix Config Tool.exe on SteamOS, a protontricks window will open.\n"
                "ERROR: Simply select ANY game that's in the list and hit OK.\n"
                "ERROR: The Config Tool will then open normally.\n"
                "\n"
                "ERROR: If you still experience difficulty launching the config tool on SteamOS, add it as a non-steam game and launch it once.\n"
                "ERROR: That will generate a new Wine prefix just for the config tool, allowing you to open it directly via protontricks in the future."
                "\n"
                "If you require further assistance, you can find our support channel at the Metal Gear Network Discord - #HDFix: " << DISCORD_URL << std::endl;
            spdlog::error("When launching the MGSPatriotFix Config Tool.exe on SteamOS, a protontricks window will open.");
            spdlog::error("Simply select ANY game that's in the list and hit OK.");
            spdlog::error("The Config Tool will then open normally.");
            spdlog::error("If you still experience difficulty launching the config tool on SteamOS, add it as a non-steam game and launch it once.");
            spdlog::error("That will generate a new Wine prefix just for the config tool, allowing you to open it directly via protontricks in the future.");
            spdlog::error("If you require further assistance, you can find our support channel at the Metal Gear Network Discord - #HDFix: {}", DISCORD_URL);
        }
        return FreeLibraryAndExitThread(baseModule, 1);
    }

    spdlog::info("Config file: {}", (sGameRootPath / sConfigFile).string());

    inipp::Ini<char> ini;
    ini.parse(iniFile);
    if (!ini.errors.empty())
    {
        spdlog::error("Error parsing ini file, encountered {} errors:", ini.errors.size());
        Logging::ShowConsole();
        for (auto err : ini.errors)
        {
            spdlog::error(err);
            std::cout << err << std::endl;
        }
    }

    // Grab desktop resolution


    ConfigHelper::getValue(ini, ConfigKeys::VerboseLogging_Section, ConfigKeys::VerboseLogging_Setting, g_Logging.bVerboseLogging);
    LOG_CONFIG(ConfigKeys::VerboseLogging_Section, ConfigKeys::VerboseLogging_Setting, g_Logging.bVerboseLogging);


    ConfigHelper::getValue(ini, ConfigKeys::WindowsSlideshowWarning_Section, ConfigKeys::WindowsSlideshowWarning_Setting, BackgroundShuffleWarning::bEnabled);
    LOG_CONFIG(ConfigKeys::WindowsSlideshowWarning_Section, ConfigKeys::WindowsSlideshowWarning_Setting, BackgroundShuffleWarning::bEnabled);

    ConfigHelper::getValue(ini, ConfigKeys::CheckForUpdates_Section, ConfigKeys::CheckForUpdates_Setting, bShouldCheckForUpdates);
    ConfigHelper::getValue(ini, ConfigKeys::UpdateConsoleNotifications_Section, ConfigKeys::UpdateConsoleNotifications_Setting, bConsoleUpdateNotifications);
    LOG_CONFIG(ConfigKeys::CheckForUpdates_Section, ConfigKeys::CheckForUpdates_Setting, bShouldCheckForUpdates);
    LOG_CONFIG(ConfigKeys::UpdateConsoleNotifications_Section, ConfigKeys::UpdateConsoleNotifications_Setting, bConsoleUpdateNotifications);


    //if (eGameType & (MGS4 | MGSPW))
    //{
    //    std::string sSpeedrunnerOverlay;
    //    ConfigHelper::getValue(ini, ConfigKeys::ShowSpeedrunnerOverlay_Section, ConfigKeys::ShowSpeedrunnerOverlay_Setting, sSpeedrunnerOverlay);
    //    if (sSpeedrunnerOverlay != ConfigKeys::ShowSpeedrunnerOverlay_Option_Disabled &&
    //        sSpeedrunnerOverlay != ConfigKeys::ShowSpeedrunnerOverlay_Option_TopLeft &&
    //        sSpeedrunnerOverlay != ConfigKeys::ShowSpeedrunnerOverlay_Option_TopRight &&
    //        sSpeedrunnerOverlay != ConfigKeys::ShowSpeedrunnerOverlay_Option_BottomLeft &&
    //        sSpeedrunnerOverlay != ConfigKeys::ShowSpeedrunnerOverlay_Option_BottomRight)
    //    {
    //        spdlog::error("Invalid config value for Gameplay Stats Overlay: {}", sSpeedrunnerOverlay);
    //        Logging::ShowConsole();
    //        std::cout << "Invalid config value for Gameplay Stats Overlay: " << sSpeedrunnerOverlay << std::endl;
    //        return FreeLibraryAndExitThread(baseModule, 1);
    //    }
    //    if (sSpeedrunnerOverlay != ConfigKeys::ShowSpeedrunnerOverlay_Option_Disabled)
    //    {
    //        D3D11TextOverlay::bShowSpeedrunnerStats = true;
    //        if (sSpeedrunnerOverlay == ConfigKeys::ShowSpeedrunnerOverlay_Option_TopLeft)
    //        {
    //            D3D11TextOverlay::iStatsPosition = D3D11TextOverlay::StatsPosition::TopLeft;
    //        }
    //        else if (sSpeedrunnerOverlay == ConfigKeys::ShowSpeedrunnerOverlay_Option_TopRight)
    //        {
    //            D3D11TextOverlay::iStatsPosition = D3D11TextOverlay::StatsPosition::TopRight;
    //        }
    //        else if (sSpeedrunnerOverlay == ConfigKeys::ShowSpeedrunnerOverlay_Option_BottomLeft)
    //        {
    //            D3D11TextOverlay::iStatsPosition = D3D11TextOverlay::StatsPosition::BottomLeft;
    //        }
    //        else if (sSpeedrunnerOverlay == ConfigKeys::ShowSpeedrunnerOverlay_Option_BottomRight)
    //        {
    //            D3D11TextOverlay::iStatsPosition = D3D11TextOverlay::StatsPosition::BottomRight;
    //        }
    //    }
    //    LOG_CONFIG(ConfigKeys::ShowSpeedrunnerOverlay_Section, ConfigKeys::ShowSpeedrunnerOverlay_Setting, sSpeedrunnerOverlay);
    //
    //}

    ConfigHelper::getValue(ini, ConfigKeys::Ds3Support_Section, ConfigKeys::Ds3Support_Setting, PressureInputs::bEnabled);
    LOG_CONFIG(ConfigKeys::Ds3Support_Section, ConfigKeys::Ds3Support_Setting, PressureInputs::bEnabled);

    ConfigHelper::getValue(ini, ConfigKeys::SaveFileReadOnlyWarning_Section, ConfigKeys::SaveFileReadOnlyWarning_Setting, CheckGamesaveFolderWritable::bCheckSaveFilesReadOnly);
    LOG_CONFIG(ConfigKeys::SaveFileReadOnlyWarning_Section, ConfigKeys::SaveFileReadOnlyWarning_Setting, CheckGamesaveFolderWritable::bCheckSaveFilesReadOnly);

    /*

    ConfigHelper::getValue(ini, ConfigKeys::ColorCorrection_Enabled_Section, ConfigKeys::ColorCorrection_Enabled_Setting, ColorCorrection::bEnabled);
#if defined(BEFORE_COMPARISON_PICS_NO_GAMMA_CORRECTION)
    ColorCorrection::bEnabled = false;
    spdlog::info("BEFORE_COMPARISON_PICS_NO_GAMMA_CORRECTION ENABLED. DISABLED COLOR CORRECTION");
#endif
    LOG_CONFIG(ConfigKeys::ColorCorrection_Enabled_Section, ConfigKeys::ColorCorrection_Enabled_Setting, ColorCorrection::bEnabled);
    */


    ConfigHelper::getValue(ini, ConfigKeys::AnisotropicFiltering_Section, ConfigKeys::AnisotropicFiltering_Setting, GraphicsTuning::iAnisotropicFiltering);
    if (GraphicsTuning::iAnisotropicFiltering < 1 || GraphicsTuning::iAnisotropicFiltering > 16)
    {
        GraphicsTuning::iAnisotropicFiltering = std::clamp(GraphicsTuning::iAnisotropicFiltering, 1, 16);
        spdlog::warn("Config Parse: Anisotropic Filtering value invalid, clamped to {}", GraphicsTuning::iAnisotropicFiltering);
    }
    LOG_CONFIG(ConfigKeys::AnisotropicFiltering_Section, ConfigKeys::AnisotropicFiltering_Setting, GraphicsTuning::iAnisotropicFiltering);

    ConfigHelper::getValue(ini, ConfigKeys::ForceDynamicResolutionOff_Section, ConfigKeys::ForceDynamicResolutionOff_Setting, GraphicsTuning::bDisableDynamicResolution);
    LOG_CONFIG(ConfigKeys::ForceDynamicResolutionOff_Section, ConfigKeys::ForceDynamicResolutionOff_Setting, GraphicsTuning::bDisableDynamicResolution);

    ConfigHelper::getValue(ini, ConfigKeys::SkipSplashscreens_Section, ConfigKeys::SkipSplashscreens_Setting, SkipSplashscreens::bSkipSplashscreens);
    LOG_CONFIG(ConfigKeys::SkipSplashscreens_Section, ConfigKeys::SkipSplashscreens_Setting, SkipSplashscreens::bSkipSplashscreens);


    {
        std::string sLauncherSkip;
        ConfigHelper::getValue(ini, ConfigKeys::LauncherSkip_Section, ConfigKeys::LauncherSkip_Setting, sLauncherSkip);

        if (sLauncherSkip == ConfigKeys::LauncherSkip_Option_Disabled)
        {
            LauncherSkipsAndStarts::eJumpMode = LauncherSkipsAndStarts::JumpMode::Disabled;
        }
        else if (sLauncherSkip == ConfigKeys::LauncherSkip_Option_GameStart)
        {
            LauncherSkipsAndStarts::eJumpMode = LauncherSkipsAndStarts::JumpMode::GameStart;
        }
        else if (sLauncherSkip == ConfigKeys::LauncherSkip_Option_DatabaseStart)
        {
            if (eGameType & MGS4)
            {
                LauncherSkipsAndStarts::eJumpMode = LauncherSkipsAndStarts::JumpMode::DatabaseStart;
            }
            else
            {
                spdlog::warn("Config Parse: Skip Launcher Splashscreens set to Database Start, but that's only available for Metal Gear Solid 4. Falling back to Game Start.");
                LauncherSkipsAndStarts::eJumpMode = LauncherSkipsAndStarts::JumpMode::GameStart;
            }
        }
        else
        {
            spdlog::error("Config Parse: Invalid value for Skip Launcher Splashscreens: {}", sLauncherSkip);
            Logging::ShowConsole();
            std::cout << "Invalid config value for Skip Launcher Splashscreens: " << sLauncherSkip << std::endl;
            return FreeLibraryAndExitThread(baseModule, 1);
        }

        LOG_CONFIG(ConfigKeys::LauncherSkip_Section, ConfigKeys::LauncherSkip_Setting, sLauncherSkip);
    }


    ConfigLogger::Flush();
}
