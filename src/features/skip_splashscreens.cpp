#include "stdafx.h"

#include "common.hpp"
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
        MAKE_HOOK_MID(baseModule, "84 C0 0F 45 CF 88 0D", "MGS4: Skip Splashscreens", {
                reghelpers::set_al(ctx, true);
                      });
    }

}
