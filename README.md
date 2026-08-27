# Metal Gear Solid Patriot Fix
[![Releases](https://img.shields.io/github/v/release/ShizCalev/MGSPatriotFix)](https://github.com/ShizCalev/MGSPatriotFix/releases) [![Downloads](https://img.shields.io/github/downloads/ShizCalev/MGSPatriotFix/total)](https://github.com/ShizCalev/MGSPatriotFix/releases) ![Commits](https://img.shields.io/github/commit-activity/t/ShizCalev/MGSPatriotFix) ![License](https://img.shields.io/github/license/ShizCalev/MGSPatriotFix)

[![Discord](https://img.shields.io/discord/1116856754920226876?logo=discord&logoColor=white&label=Metal%20Gear%20Network%20-%20%23HDFix&color=7289DA)](https://discord.gg/bFv9bZmWDV)


This is bugfix mod for the Metal Gear Solid Master Collection: Volume 2 versions of MGS4 & MGS Peace Walker.<br />

## Games Supported
- Metal Gear Solid 4
- Metal Gear Solid: Peace Walker

## Other Metal Gear Fix Projects
- MGS Master Collection - Metal Gear / Metal Gear 2: Solid Snake / Metal Gear Solid 2 / Metal Gear Solid 3 | MGSHDFix - [Repo](https://gitlab.com/ShizCalev/MGSHDFix)
- MGS Master Collection - Metal Gear Solid 1 and Bonus Content (MG1/2 NES) | MGSM2Fix - [Repo](https://github.com/nuggslet/MGSM2Fix) / [Nexus Page](https://www.nexusmods.com/metalgearsolidmc/mods/5)
- Metal Gear Solid V: The Phantom Pain | MGSVFix - [Repo](https://codeberg.org/Lyall/MGSVFix)
- Metal Gear Solid Delta: Snake Eater | MGSDeltaFix - [Repo](https://codeberg.org/Lyall/MGSDeltaFix) / [Nexus Page](https://www.nexusmods.com/metalgearsoliddeltasnakeeater/mods/27)

## Features
> [!NOTE]
(More features and fixes are added frequently and may be missing from this list.)

## Bug Fixes
#### Shared Bugs:
- Fixes the collection's games sometimes defaulting to integrated graphics processors on systems with multiple GPUs (due to Nvidia/AMD driver misconfiguration.)
- Fixes window size on displays with High DPI scaling enabled.
- Fixes the monitor going to sleep during long cutscenes (for Windows only, Linux needs to be [fixed by Valve](https://github.com/ValveSoftware/Proton/issues/8881).)



## Logging / Warnings for Common Configuration Issues
- Added a warning if Windows Multi-Plane Overlay is disabled, which can cause DirectX games to freeze/crash when alt-tabbing.


## Installation

### Steps:
1. Grab the latest release of MGSPatriotFix from [here.](https://github.com/ShizCalev/MGSPatriotFix/releases)
2. Extract the contents of the release zip into your game folder.
   - (e.g., `steamapps\common\METAL GEAR SOLID 4` or `steamapps\common\MGS_PW` for Steam.)
3. Set both "Internal Resolution" & "Internal Upscaling" to Default / Original in the game's launcher. (Resolution is entirely handled by MGSPatriotFix.)
4. Launch the MGSPatriotFix Config Tool (in the game's /plugins folder) to generate a settings file if you're installing the mod for the first time.

### Steam Deck/Linux Additional Instructions

> [!NOTE]
**🚩 These steps are only needed if you’re on Steam Deck/Linux. Skip if you’re using Windows.**

- Open up the game properties of either MGS4/MGSPW in Steam and add the following line to the launch options:

      WINEDLLOVERRIDES="wininet,winhttp=n,b" %command%

- MGSPatriotFix's Config Tool requires **ProtonTricks** to be installed via Linux's **Discover** software store.
- When opening the MGSPatriotFix Config Tool on Steam Deck/Linux, a Proton Tricks Wine Prefix window will pop up. Select any game and hit "OK" to open the MGSPatriotFix Config Tool.
   - If you do not have any games in the list, or the MGSPatriotFix Config Tool fails to launch, add it as a non-steam game and launch it once through Steam to generate a new Proton Tricks Wine Prefix entry.
   - You can remove the Config Tool from your Steam game list and launch it directly after generating this prefix.
   

### Configuration

- See **MGSPatriotFix Config Tool.exe** in the `/plugins` folder to adjust settings for the fix.


## Support
Please report any issues you notice on our Github [here](https://github.com/ShizCalev/MGSPatriotFix/issues/new/choose).

For more immediate problems, you can contact us in the [#Vol2Fix](https://discord.gg/bFv9bZmWDV) channel of the Metal Gear Network Discord.

## Known Issues
This list will contain bugs which may or may not be fixed.


### MGS Master Collection - Community Bug Tracker
- A detailed tracker which catalogs all of the known Master Collection bugs (including issues fixed by MGSPatriotFix) can be located [here](https://docs.google.com/spreadsheets/d/1WhQSRpkC_A9wBDV0o-Pohh1dMhL1H6nbVzvdluIVWrw/edit?gid=0#gid=0).
- To submit new entries to the tracker, either report a new issue on the MGSPatriotFix [Github](https://github.com/ShizCalev/MGSPatriotFix/issues/new/choose), or use [this form](https://docs.google.com/forms/d/e/1FAIpQLSef8Vx38tHpBsR-dXnawF6X0iad3XU7vmDX29pcmjbaZhQiew/viewform).


## Building
```bash
git clone https://github.com/ShizCalev/MGSPatriotFix.git
cd MGSPatriotFix
git submodule update --init --recursive
git config submodule.recurse true
```

wxWidgets has nested Git submodules; `git config submodule.recurse true` ensures they are automatically updated to the correct commits when pulling.

wxWidgets, SDL3, and Zydis are built automatically as part of the Visual Studio build process. They can also be manually rebuilt from a Visual Studio Developer Command Prompt using `build_wx.cmd`, `build_sdl3.cmd`, or `build_zydis.cmd` respectively.

### Build Requirements

- Visual Studio 2026
- MSVC v145
- Desktop development with C++
- Game development with C++
- CMake with Visual Studio 2026 support
 
### Windows
Open MGSPatriotFix.sln in Visual Studio (2026) and build.

## Credits
[@ShizCalev/Afevis](https://github.com/shizcalev)<br />
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.  <br />
[stb](https://github.com/nothings/stb) for png decoding. <br />
Universal Config Tool (made by ShizCalev/Afevis. Powered by SDL3.)
