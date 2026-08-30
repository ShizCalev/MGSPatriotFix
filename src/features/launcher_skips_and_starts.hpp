#pragma once

namespace LauncherSkipsAndStarts
{
    enum class JumpMode
    {
        Disabled,
        GameStart,
        DatabaseStart, // MGS4 only, mutually exclusive with GameStart
    };

    inline JumpMode eJumpMode = JumpMode::Disabled;

    inline bool bSkipLauncher = false;
    inline int iCtrlType = 0;

    inline std::string sRegion = "eu";
    inline std::string sLanguage = "en";

    void Apply();
}
