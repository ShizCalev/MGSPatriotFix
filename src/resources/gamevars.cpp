#include "stdafx.h"
#include "common.hpp"
#include "gamevars.hpp"

//#include "game_funcs.hpp"
#include "helper.hpp"
#include "logging.hpp"



void GameVars::Initialize()
{
    if (eGameType & MGS4)
    {
        if (uint8_t* DescTable_Scan = Memory::PatternScan(baseModule, "48 8D 05 ?? ?? ?? ?? 8B D9", "GameVars: MGS4 render target descriptor table"))
        {
            p_DescTable = Memory::GetRipRelativeAddress(DescTable_Scan, 3, 7);
        }

        if (uint8_t* RenderWidth_Scan = Memory::PatternScan(baseModule, "8B 05 ?? ?? ?? ?? 45 33 FF 89 44 24", "GameVars: MGS4 render width"))
        {
            p_RenderWidth = reinterpret_cast<int32_t*>(Memory::GetRipRelativeAddress(RenderWidth_Scan, 2, 6));
        }

        if (uint8_t* RenderHeight_Scan = Memory::PatternScan(baseModule, "44 8B 3D ?? ?? ?? ?? 89 44 24", "GameVars: MGS4 render height"))
        {
            p_RenderHeight = reinterpret_cast<int32_t*>(Memory::GetRipRelativeAddress(RenderHeight_Scan, 3, 7));
        }

        if (!Memory::IsReadable(p_RenderWidth, sizeof(int32_t)) || !Memory::IsReadable(p_RenderHeight, sizeof(int32_t)))
        {
            p_RenderWidth = nullptr;
            p_RenderHeight = nullptr;
        }

        spdlog::info("GameVars: MGS4 DescTable address is {:s}+{:X}", sExeName.c_str(), p_DescTable - (uintptr_t)baseModule);
        spdlog::info("GameVars: MGS4 RenderWidth address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)p_RenderWidth - (uintptr_t)baseModule);
        spdlog::info("GameVars: MGS4 RenderHeight address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)p_RenderHeight - (uintptr_t)baseModule);
    }

   // if (eGameType & MGS4)
   // {
   //
   //     //if (uint8_t* LevelTransitionResult = Memory::PatternScan(baseModule, "89 73 ?? 81 25", "GameVars: Level Transition"))
   //     //{
   //     //    static SafetyHookMid levelTransitionMidHook {};
   //     //    levelTransitionMidHook = safetyhook::create_mid(LevelTransitionResult,
   //     //        [](SafetyHookContext& ctx)
   //     //        {
   //     //            OnLevelTransition();
   //     //        });
   //     //    LOG_HOOK(levelTransitionMidHook, "GameVars: Level Transition")
   //     //}
   // }
   // else if (eGameType & MGSPW)
   // {
   //
   //
   //     //if (uint8_t* LevelTransitionResult = Memory::PatternScan(baseModule, "89 5F ?? E9 ?? ?? ?? ?? 39 1D", "GameVars: Level Transition"))
   //     //{
   //     //    static SafetyHookMid levelTransitionMidHook {};
   //     //    levelTransitionMidHook = safetyhook::create_mid(LevelTransitionResult,
   //     //        [](SafetyHookContext& ctx)
   //     //        {
   //     //            OnLevelTransition();
   //     //        });
   //     //    LOG_HOOK(levelTransitionMidHook, "GameVars: Level Transition")
   //     //}
   // }

   // if (eGameType & (MGS4|MGSPW))
   // {
   //     //Shared_Gamefuncs::HookFuncs();
   // }
}
