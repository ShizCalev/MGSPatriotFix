#include "stdafx.h"

#include "custom_resolution_and_borderless.hpp"

#include "logging.hpp"

#include "common.hpp"

namespace
{
    float fAspectRatio;

    constexpr float fNativeAspect = 16.0f / 9.0f;
    float fAspectMultiplier;
    float fHUDWidth;
    float fHUDHeight;
    constexpr float fDefaultHUDWidth = 1280.0f;
    constexpr float fDefaultHUDHeight = 720.0f;
    float fHUDWidthOffset;
    float fHUDHeightOffset;


}

namespace CustomResolutionAndBorderless
{


    void Init_FixDPIScaling()
    {
        if (eGameType & (MGS4 | MGSPW))
        {
            SetProcessDPIAware();
            spdlog::info("MGS4 | MGSPW: High-DPI scaling fixed.");
        }
    }


    void Init_CalculateScreenSize()
    {
        SPDLOG_ERROR("CustomResolutionAndBorderless::Init_CalculateScreenSize() NULLSTUB");
        return;
    //    // Calculate aspect ratio
    //    fHeightDeltaFrom720p = static_cast<float>(iInternalResY) / 720.0f;
    //    fAspectRatio = (float)iInternalResX / (float)iInternalResY;
    //    fAspectMultiplier = fAspectRatio / fNativeAspect;
    //
    //    // HUD variables
    //    fHUDWidth = iInternalResY * fNativeAspect;
    //    fHUDHeight = (float)iInternalResY;
    //    fHUDWidthOffset = (float)(iInternalResX - fHUDWidth) / 2;
    //    fHUDHeightOffset = 0;
    //    if (fAspectRatio < fNativeAspect)
    //    {
    //        fHUDWidth = (float)iInternalResX;
    //        fHUDHeight = (float)iInternalResX / fNativeAspect;
    //        fHUDWidthOffset = 0;
    //        fHUDHeightOffset = (float)(iInternalResY - fHUDHeight) / 2;
    //    }
    //
    //
    //    // Log details about current resolution
    //    spdlog::info("Current Resolution: Aspect Ratio: {}", fAspectRatio);
    //    spdlog::info("Current Resolution: Aspect Ratio Multiplier: {}", fAspectMultiplier);
    //    spdlog::info("Current Resolution: Corrected HUD Width: {}", fHUDWidth);
    //    spdlog::info("Current Resolution: Correct HUD Height: {}", fHUDHeight);
    //    spdlog::info("Current Resolution: HUD Width Offset: {}", fHUDWidthOffset);
    //    spdlog::info("Current Resolution: HUD Height Offset: {}", fHUDHeightOffset);
    }


}
