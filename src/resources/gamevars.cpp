#include "stdafx.h"
#include "common.hpp"
#include "gamevars.hpp"

//#include "game_funcs.hpp"
#include "logging.hpp"



void GameVars::Initialize()
{
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
