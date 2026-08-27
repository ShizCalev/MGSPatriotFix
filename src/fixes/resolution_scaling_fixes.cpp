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


    void MGSPWFixes()
    {
        using namespace ResolutionScalingFixes;


    }


}


void MGS2Fixes();

void ResolutionScalingFixes::ApplyFixes()
{
    SPDLOG_ERROR("ResolutionScalingFixes::ApplyFixes() NULLSTUB");
    return;
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

