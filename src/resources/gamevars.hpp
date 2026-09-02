// ReSharper disable CppClangTidyClangDiagnosticUniqueObjectDuplication
#pragma once

#include "game_stages.hpp"


class GameVars final
{
public:
    void Initialize();

    [[nodiscard]] uintptr_t DescTable() const { return p_DescTable; }
    [[nodiscard]] int32_t RenderWidth() const { return p_RenderWidth ? *p_RenderWidth : 0; }
    [[nodiscard]] int32_t RenderHeight() const { return p_RenderHeight ? *p_RenderHeight : 0; }

private:
    static void OnLevelTransition();

    uintptr_t p_DescTable = 0;
    int32_t* p_RenderWidth = nullptr;
    int32_t* p_RenderHeight = nullptr;
};

inline GameVars g_GameVars;

