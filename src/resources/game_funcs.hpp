#pragma once

struct CHARA
{
    unsigned int class_id;
    void* (__fastcall* new_)(int, int);
};
static_assert(sizeof(CHARA) == 0x10);

namespace Shared_Gamefuncs
{
    void HookFuncs();


};



namespace MGS4_GameFuncs
{
    void HookGameFuncs();


}


namespace MGSPW_Gamefuncs
{

    void HookGameFuncs();
}


