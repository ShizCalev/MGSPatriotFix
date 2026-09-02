// ============================================================================
// Project:   Universal Config Tool
// File:      tab_data.cpp
//
// Copyright (c) 2025 Afevis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// ============================================================================
// ReSharper disable CppClangTidyClangDiagnosticMissingFieldInitializers
#include "pch.h"
#include "tab_data.hpp"

#include "config_keys.hpp"

const std::vector<std::pair<wxString, std::vector<Field>>> kTabs = {
    { wxString("General"), {

        { (MGS4), ConfigKeys::ForceDynamicResolutionOff_Section, ConfigKeys::ForceDynamicResolutionOff_Setting, ConfigKeys::ForceDynamicResolutionOff_Help, ConfigKeys::ForceDynamicResolutionOff_Tooltip,
          std::nullopt, false, Field::Bool, false },

        { (MGS4), ConfigKeys::DisableMotionBlur_Section, ConfigKeys::DisableMotionBlur_Setting, ConfigKeys::DisableMotionBlur_Help, ConfigKeys::DisableMotionBlur_Tooltip,
          std::nullopt, false, Field::Bool, false },

        { (MGS4), ConfigKeys::EnablePauseOnFocusLoss_Section, ConfigKeys::EnablePauseOnFocusLoss_Setting, ConfigKeys::EnablePauseOnFocusLoss_Help, ConfigKeys::EnablePauseOnFocusLoss_Tooltip,
          std::nullopt, false, Field::Bool, false },

        { (MGS4), ConfigKeys::AnisotropicFiltering_Section, ConfigKeys::AnisotropicFiltering_Setting, ConfigKeys::AnisotropicFiltering_Help, ConfigKeys::AnisotropicFiltering_Tooltip,
          std::nullopt, false, Field::Int, 16, 1, 16 },

        { (MGS4|MGSPW), ConfigKeys::DisableFullscreenOptimization_Section, ConfigKeys::DisableFullscreenOptimization_Setting, ConfigKeys::DisableFullscreenOptimization_Help, ConfigKeys::DisableFullscreenOptimization_Tooltip,
          std::nullopt, false, Field::Bool, false },

        { (MGS4 | MGSPW), ConfigKeys::SkipLauncher_Section, ConfigKeys::SkipLauncher_Setting, ConfigKeys::SkipLauncher_Help, ConfigKeys::SkipLauncher_Tooltip,
          std::nullopt, false, Field::Bool, false },


        { (MGS4), ConfigKeys::LauncherSkip_Section, ConfigKeys::LauncherSkip_Setting, ConfigKeys::LauncherSkip_Help, ConfigKeys::LauncherSkip_Tooltip,
          std::make_pair(ConfigKeys::SkipLauncher_Section, ConfigKeys::SkipLauncher_Setting), true, Field::Choice, 0, 0, 0, ConfigKeys::LauncherSkip_Option_Disabled,
          {ConfigKeys::LauncherSkip_Option_Disabled, ConfigKeys::LauncherSkip_Option_GameStart, ConfigKeys::LauncherSkip_Option_DatabaseStart} },

        { (MGSPW), ConfigKeys::LauncherSkip_Section, ConfigKeys::LauncherSkip_Setting_PW, ConfigKeys::LauncherSkip_Help, ConfigKeys::LauncherSkip_Tooltip_PW,
          std::make_pair(ConfigKeys::SkipLauncher_Section, ConfigKeys::SkipLauncher_Setting), true, Field::Choice, 0, 0, 0, ConfigKeys::LauncherSkip_Option_Disabled,
          {ConfigKeys::LauncherSkip_Option_Disabled, ConfigKeys::LauncherSkip_Option_GameStart} },


        { (MGS4), ConfigKeys::SkipSplashscreens_Section, ConfigKeys::SkipSplashscreens_Setting, ConfigKeys::SkipSplashscreens_Help, ConfigKeys::SkipSplashscreens_Tooltip,
          std::nullopt, false, Field::Bool, false },


        { (MGSPW), ConfigKeys::LauncherSkip_Section, ConfigKeys::GameResolution_PW_Setting, ConfigKeys::GameResolution_PW_Help, ConfigKeys::GameResolution_PW_Tooltip,
          std::make_pair(ConfigKeys::SkipLauncher_Section, ConfigKeys::SkipLauncher_Setting), false, Field::Choice, 0, 0, 0, ConfigKeys::GameResolution_PW_Option_Original,
          { std::begin(kMGSPWGameResolutionOptions), std::end(kMGSPWGameResolutionOptions) } },

        { (MGSPW), ConfigKeys::LauncherSkip_Section, ConfigKeys::GameUpscale_PW_Setting, ConfigKeys::GameUpscale_PW_Help, ConfigKeys::GameUpscale_PW_Tooltip,
          std::make_pair(ConfigKeys::SkipLauncher_Section, ConfigKeys::SkipLauncher_Setting), false, Field::Choice, 0, 0, 0, ConfigKeys::GameUpscale_PW_Option_Original,
          { std::begin(kMGSPWGameUpscaleOptions), std::end(kMGSPWGameUpscaleOptions) } },

        { (MGSPW), ConfigKeys::LauncherSkip_Section, ConfigKeys::GameMovie_PW_Setting, ConfigKeys::GameMovie_PW_Help, ConfigKeys::GameMovie_PW_Tooltip,
          std::make_pair(ConfigKeys::SkipLauncher_Section, ConfigKeys::SkipLauncher_Setting), false, Field::Choice, 0, 0, 0, ConfigKeys::GameMovie_PW_Option_Original,
          { std::begin(kMGSPWGameMovieOptions), std::end(kMGSPWGameMovieOptions) } },



        { (MGS4|MGSPW), ConfigKeys::Region_Section, ConfigKeys::Region_Setting, ConfigKeys::Region_Help, ConfigKeys::Region_Tooltip,
          std::nullopt, false, Field::Choice, 0, 0, 0, "", {} },

        { (MGS4|MGSPW), ConfigKeys::Language_Section, ConfigKeys::Language_Setting, ConfigKeys::Language_Help, ConfigKeys::Language_Tooltip,
          std::nullopt, false, Field::Choice, 0, 0, 0, "", {} },

        { (MGS4), ConfigKeys::CtrlType_Section, ConfigKeys::CtrlType_Setting, ConfigKeys::CtrlType_Help, ConfigKeys::CtrlType_Tooltip,
          std::nullopt, false, Field::Choice, 0, 0, 0, *kMGS4LauncherConfigCtrlTypes.begin(),
          { std::begin(kMGS4LauncherConfigCtrlTypes), std::end(kMGS4LauncherConfigCtrlTypes) } },

        { (MGSPW), ConfigKeys::CtrlType_Section, ConfigKeys::CtrlType_Setting_PW, ConfigKeys::CtrlType_Help, ConfigKeys::CtrlType_Tooltip,
          std::nullopt, false, Field::Choice, 0, 0, 0, *kMGSPWLauncherConfigCtrlTypes.begin(),
          { std::begin(kMGSPWLauncherConfigCtrlTypes), std::end(kMGSPWLauncherConfigCtrlTypes) } },



      { (MGS4), ConfigKeys::Ds3Support_Section, ConfigKeys::Ds3Support_Setting, ConfigKeys::Ds3Support_Help, ConfigKeys::Ds3Support_Tooltip,
          std::nullopt, false, Field::Bool, false },

    }},
    { wxString("MGSPatriotFix / Internal"), {
        { (MGS4|MGSPW), ConfigKeys::CheckForUpdates_Section, ConfigKeys::CheckForUpdates_Setting, ConfigKeys::CheckForUpdates_Help, ConfigKeys::CheckForUpdates_Tooltip,
          std::nullopt, false, Field::Bool, true },

        { (MGS4|MGSPW), ConfigKeys::UpdateConsoleNotifications_Section, ConfigKeys::UpdateConsoleNotifications_Setting, ConfigKeys::UpdateConsoleNotifications_Help, ConfigKeys::UpdateConsoleNotifications_Tooltip,
          std::make_pair(ConfigKeys::CheckForUpdates_Section, ConfigKeys::CheckForUpdates_Setting), false, Field::Bool, true},


        { (MGS4|MGSPW), ConfigKeys::VerboseLogging_Section, ConfigKeys::VerboseLogging_Setting, ConfigKeys::VerboseLogging_Help, ConfigKeys::VerboseLogging_Tooltip,
          std::nullopt, false, Field::Bool, false },


        {(MGS4|MGSPW), "About", "", "", "", std::nullopt, false, Field::Spacer},

    }}
};
