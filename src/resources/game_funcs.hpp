#pragma once

namespace Shared_Gamefuncs
{
    void HookFuncs();
}

namespace MGS4_Gamefuncs
{
    struct CharaEntry
    {
        uint32_t class_id;
        void* new_func;
    };

    static_assert(sizeof(CharaEntry) == 16);

    void HookGameFuncs();
    [[nodiscard]] void* FindCharaFunction(uint32_t classId);
}

namespace MGSPW_Gamefuncs
{
    void HookGameFuncs();
    [[nodiscard]] void* FindCharaFunction(uint32_t classId);
}
