#pragma once

class FixFullscreenOptimization final
{
public:
    static void Fix();
    bool enabled = false;
};

inline FixFullscreenOptimization g_FixFullscreenOptimization;
