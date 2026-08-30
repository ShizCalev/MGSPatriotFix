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

        { (MGS4), ConfigKeys::AnisotropicFiltering_Section, ConfigKeys::AnisotropicFiltering_Setting, ConfigKeys::AnisotropicFiltering_Help, ConfigKeys::AnisotropicFiltering_Tooltip,
          std::nullopt, false, Field::Int, 16, 1, 16 },

        { (MGS4|MGSPW), ConfigKeys::DisableFullscreenOptimization_Section, ConfigKeys::DisableFullscreenOptimization_Setting, ConfigKeys::DisableFullscreenOptimization_Help, ConfigKeys::DisableFullscreenOptimization_Tooltip,
          std::nullopt, false, Field::Bool, false },


    }},
    { wxString("MGSPatriotFix / Internal"), {
        { (MGS4|MGSPW), ConfigKeys::CheckForUpdates_Section, ConfigKeys::CheckForUpdates_Setting, ConfigKeys::CheckForUpdates_Help, ConfigKeys::CheckForUpdates_Tooltip,
          std::nullopt, false, Field::Bool, true },

        { (MGS4|MGSPW), ConfigKeys::UpdateConsoleNotifications_Section, ConfigKeys::UpdateConsoleNotifications_Setting, ConfigKeys::UpdateConsoleNotifications_Help, ConfigKeys::UpdateConsoleNotifications_Tooltip,
          std::make_pair(ConfigKeys::CheckForUpdates_Section, ConfigKeys::CheckForUpdates_Setting), false, Field::Bool, true},

        { (MGS4|MGSPW), ConfigKeys::WindowsSlideshowWarning_Section, ConfigKeys::WindowsSlideshowWarning_Setting, ConfigKeys::WindowsSlideshowWarning_Help, ConfigKeys::WindowsSlideshowWarning_Tooltip,
          std::nullopt, false, Field::Bool, true },

        { (MGS4|MGSPW), ConfigKeys::SaveFileReadOnlyWarning_Section, ConfigKeys::SaveFileReadOnlyWarning_Setting, ConfigKeys::SaveFileReadOnlyWarning_Help, ConfigKeys::SaveFileReadOnlyWarning_Tooltip,
          std::nullopt, false, Field::Bool, true },

        { (MGS4|MGSPW), ConfigKeys::VerboseLogging_Section, ConfigKeys::VerboseLogging_Setting, ConfigKeys::VerboseLogging_Help, ConfigKeys::VerboseLogging_Tooltip,
          std::nullopt, false, Field::Bool, false },


        {(MGS4|MGSPW), "About", "", "", "", std::nullopt, false, Field::Spacer},

    }}
};
