#include "stdafx.h"
#include "common.hpp"
#include "game_funcs.hpp"

namespace MGS4_Gamefuncs
{
    namespace
    {
        const CharaEntry* s_CharaTable = nullptr;
        size_t s_CharaCount = 0;
    }
}

namespace MGSPW_Gamefuncs
{
    namespace
    {
        using GetCharaID_t = void* (__fastcall*)(uint32_t);
        GetCharaID_t s_PWGetCharaID = nullptr;
    }
}

void Shared_Gamefuncs::HookFuncs()
{
    switch (eGameType)
    {
    case MGS4:
        MGS4_Gamefuncs::HookGameFuncs();
        break;
    case MGSPW:
        MGSPW_Gamefuncs::HookGameFuncs();
        break;
    default:
        break;
    }
}

void MGSPW_Gamefuncs::HookGameFuncs()
{
    if (s_PWGetCharaID)
    {
        return;
    }

    s_PWGetCharaID = reinterpret_cast<GetCharaID_t>(Memory::PatternScanUnique(baseModule, "48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 8B F1 48 8D 1D", "MGSPW GameFuncs: GM_GetCharaID"));
}

void* MGSPW_Gamefuncs::FindCharaFunction(uint32_t classId)
{
    return s_PWGetCharaID ? s_PWGetCharaID(classId) : nullptr;
}

void MGS4_Gamefuncs::HookGameFuncs()
{
    if (s_CharaTable)
    {
        return;
    }

    auto* initChara = Memory::PatternScanUnique(baseModule, "48 8D 05 ?? ?? ?? ?? C7 05 ?? ?? ?? ?? 00 00 00 00 48 89 05 ?? ?? ?? ?? C3", "MGS4 GameFuncs: GM_InitChara");
    if (!initChara)
    {
        return;
    }

    auto* table = reinterpret_cast<const CharaEntry*>(Memory::GetRipRelativeAddress(initChara, 3, 7));
    size_t count = 0;
    for (;; ++count)
    {
        const auto* entry = table + count;
        if (!Memory::IsReadable(entry, sizeof(*entry)))
        {
            spdlog::error("MGS4 GameFuncs: Failed to read chara table.");
            return;
        }
        if (!entry->new_func)
        {
            break;
        }
        if ((count && entry->class_id <= table[count - 1].class_id) || !Memory::IsExecutable(entry->new_func))
        {
            spdlog::error("MGS4 GameFuncs: Invalid chara table entry.");
            return;
        }
    }

    s_CharaTable = table;
    s_CharaCount = count;
    spdlog::info("MGS4 GameFuncs: Cached {} chara entries.", count);
}

void* MGS4_Gamefuncs::FindCharaFunction(uint32_t classId)
{
    if (!s_CharaTable)
    {
        return nullptr;
    }

    const auto* end = s_CharaTable + s_CharaCount;
    const auto* entry = std::lower_bound(s_CharaTable, end, classId, [](const CharaEntry& candidate, uint32_t id) { return candidate.class_id < id; });
    return entry != end && entry->class_id == classId ? entry->new_func : nullptr;
}
