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

    void Apply();
}
