#include "stdafx.h"
#include "common.hpp"
#include "asi_loader_checks.hpp"


#include "logging.hpp"
#include "version.h"
#include "version_checking.hpp"
#include "config_keys.hpp"

namespace
{
    std::time_t ToTimeTWithOffset(std::filesystem::file_time_type ftime, int offsetHours)
    {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + system_clock::now()
        );
        std::time_t t = system_clock::to_time_t(sctp);
        return t + (offsetHours * 60 * 60);
    }

    std::vector<std::filesystem::path> g_InstalledMods;

    const std::unordered_set<std::string> g_BaseGameFiles =
    {
        //pw launcher
        "UnityPlayer.dll",
        "GameAssembly.dll",
        "baselib.dll",
        "d3d11.dll",

        //pw
        "sdkencryptedappticket64.dll",
        "steam_api64.dll",
        "winmm.dll",

        //mgs4
        "bink2w64.dll",
        "WinPixEventRuntime.dll",
        "MGSPatriotFix.asi",
    };

    bool IsBlacklisted(const std::filesystem::path& file)
    {
        const std::string filename = file.filename().string();
        return std::ranges::any_of(g_BaseGameFiles, [&](const std::string& banned) {
                               return _stricmp(filename.c_str(), banned.c_str()) == 0;
                           });
    }

    void GenerateInstalledModList(const std::filesystem::path& pathToCheck)
    {
        if (!std::filesystem::is_directory(pathToCheck))
        {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(pathToCheck, std::filesystem::directory_options::skip_permission_denied))
        {
            const auto path = entry.path();
            const auto ext = path.extension().string();

            if (_stricmp(ext.c_str(), ".asi") != 0 && _stricmp(ext.c_str(), ".dll") != 0)
            {
                continue;
            }

            if (IsBlacklisted(path))
            {
                continue;
            }

            auto rel = path.lexically_relative(sExePath);
            g_InstalledMods.push_back(rel.empty() ? path.filename() : rel);
        }
    }

    void CheckInstalledMods()
    {
        g_InstalledMods.clear();

        GenerateInstalledModList(sExePath);
        GenerateInstalledModList(sExePath / "plugins");
        GenerateInstalledModList(sExePath / "scripts");
        GenerateInstalledModList(sExePath / "update");

        /*
        const std::filesystem::path MGS2_w01a_stpt02_ctxr = (sExePath / "textures/flatlist/ovr_stm/_win/w01a_stpt02.bmp.ctxr");
        const std::filesystem::path MGS3_00d81b4d_ctxr = (sExePath / "textures/flatlist/ovr_stm/_win/00d81b4d.ctxr");

        const bool texturePackDetected = (eGameType & MGS2) ? std::filesystem::exists(MGS2_w01a_stpt02_ctxr) : (eGameType & MGS3) ? std::filesystem::exists(MGS3_00d81b4d_ctxr) : false;

        if (texturePackDetected)
        {
            spdlog::info("---------- Installed Mods ----------");
            spdlog::info("== textures ==");
            spdlog::info("    Upscaled texture pack installed.");
        }

        if (g_InstalledMods.empty())
        {
            return;
        }
        if (!texturePackDetected)
        {
        */
            spdlog::info("---------- Installed Mods ----------");
        //}

        spdlog::info("== root folder ==");

        std::map<std::string, std::vector<std::filesystem::path>> grouped;
        for (const auto& mod : g_InstalledMods)
        {
            std::string group = mod.has_parent_path() ? (*mod.begin()).string() : ".";
            grouped[group].push_back(mod);
        }

        for (auto& [group, files] : grouped)
        {
            std::sort(files.begin(), files.end());
        }

        auto logGroup = [](const std::string& group, const std::vector<std::filesystem::path>& files)
            {
                if (group != ".")
                {
                    spdlog::info("=== {} ===", group);
                }

                for (const auto& mod : files)
                {
                    auto absPath = sExePath / mod;
                    std::string productName = Util::GetFileProductName(absPath);

                    auto ftime = std::filesystem::last_write_time(absPath);
                    std::time_t mstTime = ToTimeTWithOffset(ftime, -7);

                    std::tm tm {};
                    localtime_s(&tm, &mstTime);

                    char timeBuf[64];
                    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S (MST / GMT -7)", &tm);

                    if (!productName.empty())
                    {
                        spdlog::info("    {}   |   {}   |   modified {}", mod.string(), productName, timeBuf);
                    }
                    else
                    {
                        spdlog::info("    {}   |   modified {}", mod.string(), timeBuf);
                    }
                }
            };

        if (grouped.contains("."))
        {
            logGroup(".", grouped["."]);
        }

        for (auto& [group, files] : grouped)
        {
            if (group == ".")
            {
                continue;
            }
            logGroup(group, files);
        }
    }

    struct AsiLoaderDllCandidate
    {
        const char* Name; // "winhttp.dll"
        std::filesystem::path Path;
    };

    void LogOutdatedAsiLoader(const char* dllName, const std::string& detectedVersion)
    {
        spdlog::error("MGSPatriotFix Warning: An outdated version of ASI Loader ({}) has been installed after MGSPatriotFix.", dllName);
        spdlog::error("MGSPatriotFix Warning: Detected ASI Loader version {}. MGSPatriotFix v{} was packaged with ASI Loader version {}.",
                      detectedVersion, sFixVersion, ASI_LOADER_VERSION_STRING);
        spdlog::error("MGSPatriotFix Warning: This can result in bugs & crashes.");
        spdlog::error("MGSPatriotFix Warning: Please reinstall MGSPatriotFix to update to the latest version of ASI Loader.");

        Logging::ShowConsole();

        std::cout << "MGSPatriotFix Warning: An outdated version of ASI Loader (" << dllName << ") has been installed after MGSPatriotFix.\n";
        std::cout << "MGSPatriotFix Warning: Detected ASI Loader version " << detectedVersion
            << ". MGSPatriotFix v" << sFixVersion
            << " was packaged with ASI Loader version " << ASI_LOADER_VERSION_STRING << ".\n";
        std::cout << "MGSPatriotFix Warning: This can result in bugs & crashes.\n";
        std::cout << "MGSPatriotFix Warning: Please reinstall MGSPatriotFix to update to the latest version of ASI Loader.\n";
    }

    void CheckSingleAsiLoaderDll(const AsiLoaderDllCandidate& dll)
    {
        if (!std::filesystem::exists(dll.Path))
        {
            spdlog::info("ASI Loader Compatibility Check: {} not present.", dll.Name);
            return;
        }

        const std::string description = Util::GetFileDescription(dll.Path.string());
        const bool isUltimateAsi = (description == kAsiLoaderDescription);

        spdlog::info(
            "ASI Loader Compatibility Check: {} | FileDescription: \"{}\" | IsUltimateASI: {}",
            dll.Name,
            description,
            isUltimateAsi ? "true" : "false"
        );

        if (!isUltimateAsi)
        {
            return;
        }

        const std::string version =
            VersionCheck::GetFileVersion(dll.Path, VersionCheck::VersionType::Product);

        switch (VersionCheck::CompareSemanticVersion(version, ASI_LOADER_VERSION_STRING))
        {
        case VersionCheck::CompareResult::Older:
            LogOutdatedAsiLoader(dll.Name, version);
            return;

        case VersionCheck::CompareResult::Equal:
            spdlog::info("ASI Loader Compatibility Check: {} version {} is same as time of MGSPatriotFix compilation.", dll.Name, version);
            return;

        case VersionCheck::CompareResult::Newer:
            spdlog::info("ASI Loader Compatibility Check: {} is newer version than at time of MGSPatriotFix compilation.", dll.Name);
            spdlog::info("ASI Loader Compatibility Check: Detected ASI Loader version {}. MGSPatriotFix Compilation Version: {}",
                         version, ASI_LOADER_VERSION_STRING);
            return;
        }
    }

    void CheckBothAsiLoaderDlls()
    {
        static const AsiLoaderDllCandidate kCandidates[] =
        {
            { "winhttp.dll", sExePath / "winhttp.dll" },
            { "wininet.dll", sExePath / "wininet.dll" }
        };

        for (const auto& dll : kCandidates)
        {
            CheckSingleAsiLoaderDll(dll);
        }
    }
}

void ASILoaderCompatibility::Check()
{
    //CheckBothAsiLoaderDlls();

    CheckInstalledMods();

}
