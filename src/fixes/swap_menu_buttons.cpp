// ReSharper disable CppClangTidyClangDiagnosticLanguageExtensionToken
#include "stdafx.h"

#include "swap_menu_buttons.hpp"

#include "common.hpp"
#include "logging.hpp"

namespace
{
    constexpr int16_t kMGS4SouthButton = 0x4000;
    constexpr int16_t kMGS4EastButton = 0x2000;
    
    int16_t g_MGS4OKButton = kMGS4SouthButton;
    SafetyHookInline MGS4_SetMenuButtonAssignments_hook{};

    __int64 __fastcall MGS4_SetMenuButtonAssignments_hook_dest(int16_t)
    {
        return MGS4_SetMenuButtonAssignments_hook.call<__int64>(g_MGS4OKButton);
    }
}

void SwapMenuButtons::Apply()
{
    if (!(eGameType & MGS4))
        return;

    if (force_menu_buttons == ConfigKeys::MenuButton_Option_Default)
    {
        spdlog::info("MGS 4: Swap Menu Buttons: Using default menu button inputs, skipping.");
        return;
    }

    if (force_menu_buttons != ConfigKeys::MenuButton_Option_EastForOK && force_menu_buttons != ConfigKeys::MenuButton_Option_SouthForOK)
    {
        spdlog::error("MGS 4: Swap Menu Buttons: Invalid option '{}', skipping.", force_menu_buttons);
        return;
    }

    const bool eastForOK = force_menu_buttons == ConfigKeys::MenuButton_Option_EastForOK;


	g_MGS4OKButton = eastForOK ? kMGS4EastButton : kMGS4SouthButton;

	if (uint8_t* SetMenuButtonAssignments_Scan = Memory::PatternScanUnique(baseModule, "BA ?? ?? ?? ?? 66 89 0D", "MGS 4: Swap Menu Buttons | SetMenuButtonAssignments()"))
	{
		MGS4_SetMenuButtonAssignments_hook = safetyhook::create_inline(SetMenuButtonAssignments_Scan, MGS4_SetMenuButtonAssignments_hook_dest);
		LOG_HOOK(MGS4_SetMenuButtonAssignments_hook, "MGS 4: Swap Menu Buttons | SetMenuButtonAssignments()")
		if (MGS4_SetMenuButtonAssignments_hook)
		{
			MGS4_SetMenuButtonAssignments_hook.call<__int64>(g_MGS4OKButton);
		}
	}

}
