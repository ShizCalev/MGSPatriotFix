#pragma once
#include "helper.hpp"
#include "logging.hpp"


inline std::string sExeName;
inline std::filesystem::path sExePath;
inline std::filesystem::path sGameRootPath;
inline std::filesystem::path sGameSavePath;
inline bool bIsLauncher = false;

inline HMODULE baseModule = GetModuleHandle(NULL);
inline HMODULE engineModule;
inline HMODULE unityPlayer;


struct GameInfo
{
    std::string GameTitle;
    std::string ExeName;
    std::string GameSubfolder;
    int SteamAppId;
};
inline const GameInfo* game = nullptr;

enum MgsGame : std::uint8_t
{
    NONE     = 0,
    MGS4     = 1 << 0,
    MGSPW     = 1 << 1,
    LAUNCHER = 1 << 3,
    UNKNOWN  = 1 << 4
};
inline MgsGame eGameType = UNKNOWN;



inline const std::map<MgsGame, GameInfo> kGames = {
    {MGS4, {"Metal Gear Solid 4", "mgs4.exe", "MGS4", 4184400}},
    {MGSPW, {"Metal Gear Solid Peace Walker", "METAL GEAR SOLID PEACE WALKER.exe", "mgspw", 4184360}},
};

inline bool usDatExists;
inline bool jpDatExists;

//constexpr float f_PS2_Width = 512.0f;
//constexpr float f_PS2_Height = 448.0f;

 
//#define BEFORE_COMPARISON_PICS
#if defined(BEFORE_COMPARISON_PICS)
//#define BEFORE_COMPARISON_PICS_NO_GAMMA_CORRECTION
#define BEFORE_COMPARISON_PICTURES_NO_TYPO_FIXES
#define BEFORE_COMPARISON_PICS_NO_SMAA
#endif


