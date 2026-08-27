#pragma once



struct Stage
{
    const char* sStageId;
    const char* sGameMode;
    const char* sRichPresenceName;
};
//
//#define MGS4_STAGE_LIST \
//    /* Menus                 */ \
//    X(INIT,      "init",      "Menu", "Game Initialization") \
//    X(W00B, "w00b", "Tanker", "Deck A - Starboard (Olga)") 
//
//namespace MGS4Stages
//{
//#define X(name, id, mode, disp) inline constexpr const char* name = id;
//    MGS4_STAGE_LIST
//#undef X
//}
//
//
//#define MGSPW_STAGE_LIST \
//    X(TITLE,   "title",   "Menu", "Title Screen / Main Menu") \
//    X(ENDING,  "ending",  "Menu", "Credits")
//
//
//namespace MGSPWStages
//{
//#define X(name, id, mode, disp) constexpr const char* name = id;
//    MGSPW_STAGE_LIST
//#undef X
//}
//