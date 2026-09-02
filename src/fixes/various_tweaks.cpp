#include "stdafx.h"

#include "common.hpp"
#include "various_tweaks.hpp"

#include "logging.hpp"

void VariousTweaks::Apply()
{
    if (eGameType & MGS4)
    {
        if (!bPauseOnFocusLoss)
        {
            MAKE_HOOK_MID(baseModule, "48 85 C0 74 ?? 8B D3 48 8B CD", "MGS4 - Tweaks | Disable _glfwInputWindowFocus()",
                          {
                              if (static_cast<uint32_t>(ctx.rbx) == 0)
                              {
                                  ctx.rax = 0;
                              }
                          });
        }
    }
}
