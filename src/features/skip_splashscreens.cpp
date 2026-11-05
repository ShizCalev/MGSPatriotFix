#include "stdafx.h"

#include "common.hpp"
#include "game_defines.hpp"
#include "game_funcs.hpp"
#include "skip_splashscreens.hpp"

#include "logging.hpp"

void SkipSplashscreens::Apply()
{
    if (!bSkipSplashscreens)
    {
        return;
    }

    if (eGameType & MGS4)
    {
        auto* startupStateFunction = MGS4_Gamefuncs::FindCharaFunction(MGS4_Defines::GM_GetStartupState);
        if (!startupStateFunction)
        {
            spdlog::error("MGS4: Skip Splashscreens: Failed to find startup chara... Fuck.");
            return;
        }

        static SafetyHookInline hook {};
        hook = safetyhook::create_inline(startupStateFunction, +[]() -> uint32_t { return 2; });
        LOG_HOOK(hook, "MGS4: Skip Splashscreens")
    }
}
