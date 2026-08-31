// ReSharper disable CppClangTidyReadabilityUseConcisePreprocessorDirectives
#include "stdafx.h"

#include "common.hpp"
#include "logging.hpp"

#include "game_funcs.hpp"

#include "mgspw_linkvarbuf.hpp"
#include "mgs4_linkvarbuf.hpp"
/*
#if defined(RELEASE_BUILD)
#define RELEASE_CLEARED
#undef RELEASE_BUILD
#endif
*/
namespace
{
    int* GM_LoadRequest = nullptr;

#if !defined(RELEASE_BUILD)
    void* null_fn(int, int) { return nullptr; }

    void HookReturn(uint8_t* addr)
    {
        static SafetyHookInline hook;
        hook = safetyhook::create_inline(addr, null_fn);
    }

    using NewCharaCallback = std::function<void* (int, int)>;

    NewCharaCallback* pending = nullptr;

    std::unordered_map<std::string, std::unordered_map<uint32_t, NewCharaCallback>> return_hooks;

    struct ObserveEntry {
        std::function<void()> callback;
        SafetyHookMid hook;
    };

    std::unordered_map<std::string, std::unordered_map<uint32_t, ObserveEntry>> observe_hooks;

    SafetyHookInline GM_GetCharaID_hook;

    void* override_stub(int name, int map)
    {
        NewCharaCallback* cb = pending;
        pending = nullptr;
        if (cb && *cb) return (*cb)(name, map);
        return nullptr;
    }

    void* hooked_GM_GetCharaID(int nID)
    {
        if (const char* stage = Shared_Gamefuncs::GM_GetArea())
        {
            auto osit = observe_hooks.find(stage);
            if (osit != observe_hooks.end())
            {
                auto oiit = osit->second.find(static_cast<uint32_t>(nID));
                if (oiit != osit->second.end() && !oiit->second.hook)
                {
                    void* fn = GM_GetCharaID_hook.call<void*>(nID);
                    auto cb = oiit->second.callback;
                    static std::function<void()>* s_cb = nullptr;
                    s_cb = &oiit->second.callback;
                    oiit->second.hook = safetyhook::create_mid(fn, [](SafetyHookContext&) { if (s_cb) (*s_cb)(); });
                    return fn;
                }
            }

            auto sit = return_hooks.find(stage);
            if (sit != return_hooks.end())
            {
                auto iit = sit->second.find(static_cast<uint32_t>(nID));
                if (iit != sit->second.end())
                {
                    pending = &iit->second;
                    return reinterpret_cast<void*>(override_stub);
                }
            }
        }
        return GM_GetCharaID_hook.call<void*>(nID);
    }
#endif


}

namespace MGS4_GameFuncs
{


}

void MGS4_GameFuncs::HookGameFuncs()
{
    using namespace Shared_Gamefuncs;
    using namespace MGS4_GameFuncs;
    using namespace MGS4_LinkVarBuf;
    //using namespace MGS4Stages;


}




void MGSPW_Gamefuncs::HookGameFuncs()
{
    using namespace Shared_Gamefuncs;
    using namespace MGSPW_Gamefuncs;
    using namespace MGSPW_LinkVarBuf;
    //using namespace MGSPWStages;




}



void Shared_Gamefuncs::HookFuncs()
{
 
    /*
#if !defined(RELEASE_BUILD)

    spdlog::info("Shared_GameFuncs: Hooking CHARA stage function table.");

    uint8_t* GM_GetCharaID_scan = Memory::PatternScan(baseModule, eGameType & MGS2 ? "E8 ?? ?? ?? ?? 48 8B F0 48 85 C0 75 ?? 8D 46 ?? 48 83 C4" : "E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 75 ?? 48 8D 43 ?? 48 83 C4 ?? 5B", "GM_GetCharaID call site");
    GM_GetCharaID_hook = safetyhook::create_inline(Memory::ResolveCall(GM_GetCharaID_scan), hooked_GM_GetCharaID);
    spdlog::info("Shared_GameFuncs: GM_GetCharaID address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)GM_GetCharaID_hook.target() - (uintptr_t)baseModule);

#endif
*/



    switch (eGameType)
    {
    case MGS4:
        MGS4_GameFuncs::HookGameFuncs();
        break;
    case MGSPW:
        MGSPW_Gamefuncs::HookGameFuncs();
        break;
    default:
        return;

    }
}

#if defined (RELEASE_CLEARED)
#define RELEASE_BUILD
#endif
