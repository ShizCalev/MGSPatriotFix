#pragma once

namespace GraphicsTuning
{
    inline int iAnisotropicFiltering;
    inline bool bDisableDynamicResolution = false;
    inline bool bDisableMotionBlur = false;
    inline int iShadowBufferSize = 0;
    inline int iShadowSampleCount = 0;

    void Apply();
}
