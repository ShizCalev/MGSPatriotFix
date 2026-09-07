// ReSharper disable CppClangTidyClangDiagnosticLanguageExtensionToken
#include "stdafx.h"

#include "swap_menu_buttons.hpp"

#include "common.hpp"
#include "gamevars.hpp"
#include "logging.hpp"

namespace
{
    constexpr int16_t kMGS4SouthButton = 0x4000;
    constexpr int16_t kMGS4EastButton = 0x2000;
    
    int16_t g_MGS4OKButton = kMGS4SouthButton;
    int16_t g_MGS4VanillaOKButton = kMGS4SouthButton;
    SafetyHookInline MGS4_SetMenuButtonAssignments_hook{};
    SafetyHookInline MGS4_UpdateInputDevice_hook{};

    __int64 __fastcall MGS4_SetMenuButtonAssignments_hook_dest(int16_t okButton)
    {
        g_MGS4VanillaOKButton = okButton;
        return MGS4_SetMenuButtonAssignments_hook.call<__int64>(g_GameVars.IsControllerActive() ? g_MGS4OKButton : g_MGS4VanillaOKButton);
    }

    __int64 __fastcall MGS4_UpdateInputDevice_hook_dest()
    {
        const bool wasControllerActive = g_GameVars.IsControllerActive();
        const __int64 result = MGS4_UpdateInputDevice_hook.call<__int64>();
        if (wasControllerActive != g_GameVars.IsControllerActive() && MGS4_SetMenuButtonAssignments_hook)
        {
            MGS4_SetMenuButtonAssignments_hook.call<__int64>(g_GameVars.IsControllerActive() ? g_MGS4OKButton : g_MGS4VanillaOKButton);
        }
        return result;
    }
}

void SwapMenuButtons::Apply()
{
    if (!(eGameType & MGS4))
    {
        return;
    }

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

    uint8_t* SetMenuButtonAssignments_Scan = Memory::PatternScanUnique(baseModule, "BA ?? ?? ?? ?? 66 89 0D", "MGS 4: Swap Menu Buttons | SetMenuButtonAssignments()");
    uint8_t* UpdateInputDevice_Scan = Memory::PatternScanUnique(baseModule, "48 83 EC ?? 8B 05 ?? ?? ?? ?? 85 C0 75 ?? E8 ?? ?? ?? ?? 8B 05", "MGS 4: Swap Menu Buttons | UpdateInputDevice()");
    if (!SetMenuButtonAssignments_Scan || !UpdateInputDevice_Scan)
    {
        return;
    }

    if (!g_GameVars.ActiveInputDevice())
    {
        spdlog::error("MGS 4: Swap Menu Buttons: GameVars fialed to resolve ActiveInputDevice(), skipping.");
        return;
    }

    MGS4_SetMenuButtonAssignments_hook = safetyhook::create_inline(SetMenuButtonAssignments_Scan, MGS4_SetMenuButtonAssignments_hook_dest);
    LOG_HOOK(MGS4_SetMenuButtonAssignments_hook, "MGS 4: Swap Menu Buttons | SetMenuButtonAssignments()")
    MGS4_UpdateInputDevice_hook = safetyhook::create_inline(UpdateInputDevice_Scan, MGS4_UpdateInputDevice_hook_dest);
    LOG_HOOK(MGS4_UpdateInputDevice_hook, "MGS 4: Swap Menu Buttons | UpdateInputDevice()")
    if (MGS4_SetMenuButtonAssignments_hook)
    {
        MGS4_SetMenuButtonAssignments_hook.call<__int64>(g_GameVars.IsControllerActive() ? g_MGS4OKButton : g_MGS4VanillaOKButton);
    }
}
