// ReSharper disable CppClangTidyClangDiagnosticUniqueObjectDuplication
#pragma once

#include "game_stages.hpp"


class GameVars final
{
public:
    void Initialize();


private:
    static void OnLevelTransition();

};

inline GameVars g_GameVars;

