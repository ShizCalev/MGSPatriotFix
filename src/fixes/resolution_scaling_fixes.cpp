#include "stdafx.h"

#include"resolution_scaling_fixes.hpp"

#include "common.hpp"
//#include "custom_resolution_and_borderless.hpp"
#include "logging.hpp"



namespace
{
    double scaleX_fromPs2 = 1.0;
    double scaleX_fromPs2_4by3 = 1.0;
    double scaleY_fromPs2 = 1.0;





    void MGS4Fixes()
    {

        using namespace ResolutionScalingFixes;
        {


        }
    }


    void FixFullscreenMaxResolution()
    {
		if(!ResolutionScalingFixes::bFixPillarboxing)
		{
			return;
		}
        MAKE_HOOK_MID(baseModule, "44 8B 4D ?? 33 D2 44 8B 45 ?? 48 8B 0D ?? ?? ?? ?? 44 89 64 24 ?? 44 89 74 24 ?? 44 89 7C 24", "MGSPW - Resolution Scaling Fixes: Fix pillarboxing on 16:10 monitors",
            {
                auto desktopDimensions = Util::GetPhysicalDesktopDimensions();
                if (desktopDimensions.first > 0 && desktopDimensions.second > 0)
                {
                    ctx.r15 = static_cast<uint64_t>(static_cast<uint32_t>(desktopDimensions.first));
                    ctx.r14 = static_cast<uint64_t>(static_cast<uint32_t>(desktopDimensions.second));
                }
            });
    }

    void MGSPWFixes()
    {
        using namespace ResolutionScalingFixes;

        FixFullscreenMaxResolution();
    }


}


void MGS2Fixes();

void ResolutionScalingFixes::ApplyFixes()
{
    //scaleX_fromPs2 = CustomResolutionAndBorderless::iInternalResX / 512.0;
    //scaleX_fromPs2_4by3 = ((double)CustomResolutionAndBorderless::iInternalResX) / (double)(CustomResolutionAndBorderless::iInternalResY) / (4.0 / 3.0);
    //scaleY_fromPs2 = CustomResolutionAndBorderless::iInternalResY / 448.0;
    //
    //SPDLOG_INFO("Resolution Scaling Fixes: Internal Width = {}, Internal Height = {}", CustomResolutionAndBorderless::iInternalResX, CustomResolutionAndBorderless::iInternalResY);
    //SPDLOG_INFO("Resolution Scaling Fixes: PS2 Height Delta = {}, PS2 Width Delta = {}, PS2 Width 4:3 Delta = {}", scaleY_fromPs2, scaleX_fromPs2, scaleX_fromPs2_4by3);

#ifdef BEFORE_COMPARISON_PICS
    spdlog::info("BEFORE_COMPARISON_PICS TRUE. SKIPPING RESOLUTION SCALING FIXES");
    return;
#endif



    if (eGameType & MGS4)
    {


        MGS4Fixes();




    }
    else if (eGameType & MGSPW)
    {

        MGSPWFixes();


    }
}

