#pragma once


// Shaking the pad resets the OctoCamo. The game gets pad motion from Steam, which has none for a
// DualShock 3, so we answer that call with the pad's accelerometer instead.
namespace PadMotion
{
    inline bool bEnabled = true;

    void Initialize();
}
