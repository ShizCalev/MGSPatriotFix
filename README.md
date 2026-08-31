# Metal Gear Solid Patriot Fix
[![Releases](https://img.shields.io/github/v/release/ShizCalev/MGSPatriotFix)](https://github.com/ShizCalev/MGSPatriotFix/releases) [![Downloads](https://img.shields.io/github/downloads/ShizCalev/MGSPatriotFix/total)](https://github.com/ShizCalev/MGSPatriotFix/releases) ![Commits](https://img.shields.io/github/commit-activity/t/ShizCalev/MGSPatriotFix) ![License](https://img.shields.io/github/license/ShizCalev/MGSPatriotFix)

[![Discord](https://img.shields.io/discord/1116856754920226876?logo=discord&logoColor=white&label=Metal%20Gear%20Network%20-%20%23HDFix&color=7289DA)](https://discord.gg/bFv9bZmWDV)


This is bugfix mod for the Metal Gear Solid Master Collection: Volume 2 versions of MGS4 & MGS Peace Walker.<br />

## Games Supported
- Metal Gear Solid 4
- Metal Gear Solid: Peace Walker

## Other Metal Gear Fix Projects
- MGS Master Collection - Metal Gear / Metal Gear 2: Solid Snake / Metal Gear Solid 2 / Metal Gear Solid 3 | MGSHDFix - [Repo](https://github.com/ShizCalev/MGSHDFix)
- MGS Master Collection - Metal Gear Solid 1 and Bonus Content (MG1/2 NES) | MGSM2Fix - [Repo](https://github.com/nuggslet/MGSM2Fix) / [Nexus Page](https://www.nexusmods.com/metalgearsolidmc/mods/5)
- Metal Gear Solid V: The Phantom Pain | MGSVFix - [Repo](https://codeberg.org/Lyall/MGSVFix)
- Metal Gear Solid Delta: Snake Eater | MGSDeltaFix - [Repo](https://codeberg.org/Lyall/MGSDeltaFix) / [Nexus Page](https://www.nexusmods.com/metalgearsoliddeltasnakeeater/mods/27)

<br />

## Features
> [!NOTE]
(More features and fixes are added frequently and may be missing from this list.)

#### Shared Features
- Option to skip the launchers.
- Option to skip the the launcher's splashscreens / jump to the game-start menu.


#### MGS4 Specific Features:
- DualShock 3 support (pressure-sensitive face buttons/triggers, rumble, and motion for shake actions such as resetting the OctoCamo.) 
  - See [DualShock 3 Setup](#dualshock-3-setup).
- Option to skip in-game splashscreens.
- Option to disable motion blur.
- Option to disable dynamic resolution.


<br />

## Bug Fixes
#### Shared Bugfixes:
- Fixes the monitor going to sleep during long cutscenes (for Windows only, Linux needs to be [fixed by Valve](https://github.com/ValveSoftware/Proton/issues/8881).)

#### MGS4 Specific Bugfixes:
- Fixes the MGS1 flashback sometimes defaulting to integrated graphics processors on systems with multiple GPUs.
- Fixes performance issues with the MGS1 flashback on some hardware configurations. (Windows fullscreen optimization issue.)

#### Peace Walker Specific Bugfixes:
- Fixes Peace Walker sometimes defaulting to integrated graphics processors on systems with multiple GPUs.


<br />

## Installation

### Steps:
1. Grab the latest release of MGSPatriotFix from [here.](https://github.com/ShizCalev/MGSPatriotFix/releases)
   - There are two separate zips per release, one per game:
     - `MGS4_MGSPatriotFix_<version>.zip` for Metal Gear Solid 4
     - `PW_MGSPatriotFix_<version>.zip` for Metal Gear Solid: Peace Walker
2. Extract the contents of the zip into the matching game's root folder.
   - (e.g., extract the MGS4 zip's contents into `steamapps\common\METAL GEAR SOLID 4`, the PW zip's contents into `steamapps\common\MGS_PW`.)
4. Launch the MGSPatriotFix Config Tool (in the game's root folder) to generate a settings file.

### Steam Deck/Linux Additional Instructions

> [!NOTE]
**🚩 These steps are only needed if you’re on Steam Deck/Linux. Skip if you’re using Windows.**

- Open up the game properties of either MGS4/MGSPW in Steam and add the following line to the launch options:

      WINEDLLOVERRIDES="winmm=n,b" %command%

- MGSPatriotFix's Config Tool requires **ProtonTricks** to be installed via Linux's **Discover** software store.
- When opening the MGSPatriotFix Config Tool on Steam Deck/Linux, a Proton Tricks Wine Prefix window will pop up. Select any game and hit "OK" to open the MGSPatriotFix Config Tool.
   - If you do not have any games in the list, or the MGSPatriotFix Config Tool fails to launch, add it as a non-steam game and launch it once through Steam to generate a new Proton Tricks Wine Prefix entry.
   - You can remove the Config Tool from your Steam game list and launch it directly after generating this prefix.


<br />

### Configuration

- See **MGSPatriotFix Config Tool.exe** in the game's root folder to adjust settings for the fix.


<br />

## DualShock 3 Setup
### Windows:

 - Download and install [DsHidMini](https://docs.nefarius.at/projects/DsHidMini/)
 - Set DsHidMini to SXS mode.

  (This matches the configuration required for PCSX2. If your DS3 controller is already set up properly with PCSX2, then you're already good to go!)

### Steam Deck / Linux:
> [!NOTE]
**🚩 These steps are only needed if you’re on Steam Deck/Linux. Skip if you’re using Windows.**

 The pad works out of the box, but Steam needs to be told to prefer it.

 Step 1)
  - Open up the game properties of MGS4 in Steam and set the following line to the launch options:

        SDL_GAMECONTROLLER_IGNORE_DEVICES= PROTON_ENABLE_HIDRAW=0x054C/0x0268 WINEDLLOVERRIDES="winmm=n,b" %command%

 Step 2)
  - Open the Steam overlay while the game is running.
  - Open controller settings
  - Click reorder controllers
  
  <img width="600" height="400" alt="mgs2 - d00t - rain 1" src="https://raw.githubusercontent.com/dotlessone/MGSHDFix-Media/refs/heads/main/MGSHDFix%20Features/Dualshock%202-3/controller%20order%201.png" />

  Step 3)
  - Set the PS3 controller as your primary device.
  
  <img width="400" height="250" alt="mgs2 - d00t - rain 1" src="https://raw.githubusercontent.com/dotlessone/MGSHDFix-Media/refs/heads/main/MGSHDFix%20Features/Dualshock%202-3/controller%20order%202.png" />


<br />

## Support
Please report any issues you notice on our Github [here](https://github.com/ShizCalev/MGSPatriotFix/issues/new/choose).

For more immediate problems, you can contact us in the [#PatriotFix](https://discord.gg/bFv9bZmWDV) channel of the Metal Gear Network Discord.

<br />

### MGS Master Collection - Community Bug Tracker
- A detailed tracker which catalogs all of the known Master Collection bugs (including issues fixed by MGSPatriotFix) can be located [here](https://docs.google.com/spreadsheets/d/1WhQSRpkC_A9wBDV0o-Pohh1dMhL1H6nbVzvdluIVWrw/edit?gid=0#gid=0).
- To submit new entries to the tracker, either report a new issue on the MGSPatriotFix [Github](https://github.com/ShizCalev/MGSPatriotFix/issues/new/choose), or use [this form](https://docs.google.com/forms/d/e/1FAIpQLSef8Vx38tHpBsR-dXnawF6X0iad3XU7vmDX29pcmjbaZhQiew/viewform).

<br />

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

<br />

## Credits

Maintained by [@ShizCalev/Afevis](https://github.com/shizcalev)<br />
[@cipherxof](https://github.com/cipherxof), and [@gibletto](https://github.com/gibletto) for contributing more amazing fixes/features. <br />
 <br />
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.  <br />
Universal Config Tool (made by ShizCalev/Afevis. Powered by SDL3.)
