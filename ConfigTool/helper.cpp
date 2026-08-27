#include "pch.h"
#include "helper.hpp"
#include <wx/app.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/thread.h>
#include "version.h"

namespace
{
    bool HasAnyGameExe(const std::filesystem::path& dir)
    {
        if (const auto mgs4Dir = Helper::FindSubfolderCaseInsensitive(dir, "MGS4"); !mgs4Dir.empty())
        {
            if (std::filesystem::exists(mgs4Dir / "mgs4.exe"))
            {
                return true;
            }
        }

        if (const auto mgspwDir = Helper::FindSubfolderCaseInsensitive(dir, "mgspw"); !mgspwDir.empty())
        {
            if (std::filesystem::exists(mgspwDir / "METAL GEAR SOLID PEACE WALKER.exe"))
            {
                return true;
            }
        }

        return false;
    }

    std::filesystem::path TryFindDetectedGameRootByWalkingUp(const std::filesystem::path& start)
    {
        if (start.empty())
        {
            return {};
        }

        std::filesystem::path cur;
        try
        {
            cur = std::filesystem::weakly_canonical(start);
        }
        catch (...)
        {
            cur = start;
        }

        for (;;)
        {
            if (HasAnyGameExe(cur))
            {
                return cur;
            }

            const std::filesystem::path parent = cur.parent_path();
            if (parent.empty() || parent == cur)
            {
                break;
            }

            cur = parent;
        }

        return {};
    }

    bool IsWineHomePath(const std::filesystem::path& p)
    {
        const std::wstring ws = p.wstring();
        if (ws.size() < 7)
        {
            return false;
        }

        auto iequals_prefix = [](const std::wstring& s, const std::wstring& prefix)
            {
                if (s.size() < prefix.size())
                {
                    return false;
                }

                for (size_t i = 0; i < prefix.size(); ++i)
                {
                    if (towlower(s[i]) != towlower(prefix[i]))
                    {
                        return false;
                    }
                }
                return true;
            };

        return iequals_prefix(ws, L"Z:\\home");
    }

    std::filesystem::path GetExeDirPath()
    {
        wxStandardPaths& sp = wxStandardPaths::Get();
        return std::filesystem::path(sp.GetExecutablePath().ToStdWstring()).parent_path();
    }


    std::vector<int> parseVersionString(const std::string& versionStr)
    {
        std::vector<int> parts;
        std::istringstream ss(versionStr);
        std::string token;

        while (std::getline(ss, token, '.'))
        {
            if (token.empty())
            {
                parts.push_back(0);
                {
                    continue;
                }
            }

            size_t i = 0;
            while (i < token.size() && std::isdigit(static_cast<unsigned char>(token[i])))
            {
                ++i;
            }

            int value = (i > 0) ? std::stoi(token.substr(0, i)) : 0;
            parts.push_back(value);

            if (i < token.size())
            {
                // take first suffix letter -> 'a' = 1, 'b' = 2, etc.
                char c = static_cast<char>(std::tolower(token[i]));
                if (c >= 'a' && c <= 'z')
                {
                    parts.push_back((c - 'a') + 1);
                }
                else
                {
                    parts.push_back(1); // fallback for weird suffix
                }
            }
        }

        return parts;
    }
}

namespace Helper
{

    void RunOnMainThread(std::function<void()> fn)
    {
        if (wxIsMainThread())
        {
            fn();
        }
        else
        {
            wxTheApp->CallAfter(std::move(fn));
        }
    }

    std::filesystem::path FindSubfolderCaseInsensitive(const std::filesystem::path& root, const std::string& name)
    {
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(root, ec))
        {
            if (ec)
            {
                break;
            }

            if (entry.is_directory(ec) && _stricmp(entry.path().filename().string().c_str(), name.c_str()) == 0)
            {
                return entry.path();
            }
        }

        return {};
    }

    std::filesystem::path FindGameRoot()
    {
        static std::filesystem::path s_cachedRoot;

        // Cache hit
        if (!s_cachedRoot.empty() && HasAnyGameExe(s_cachedRoot))
        {
            return s_cachedRoot;
        }

        const std::filesystem::path cwdBase = std::filesystem::path(wxGetCwd().ToStdWstring());
        const std::filesystem::path exeDir = GetExeDirPath();

        // 1) Prefer CWD (symlink friendly)
        if (!IsWineHomePath(cwdBase) && HasAnyGameExe(cwdBase))
        {
            s_cachedRoot = cwdBase;
            return s_cachedRoot;
        }

        // 2) Fallback to executable directory (Proton/Vortex friendly)
        if (HasAnyGameExe(exeDir))
        {
            s_cachedRoot = exeDir;
            return s_cachedRoot;
        }

        // ------------------------------------------------------------
        // Not found OR found only in an invalid location: show install guidance
        // ------------------------------------------------------------
        const std::filesystem::path currentLocation = IsWineHomePath(cwdBase) ? exeDir : cwdBase;

        std::filesystem::path detectedGameRoot = TryFindDetectedGameRootByWalkingUp(currentLocation);
        if (detectedGameRoot.empty())
        {
            const std::filesystem::path alt = (currentLocation == cwdBase) ? exeDir : cwdBase;
            detectedGameRoot = TryFindDetectedGameRootByWalkingUp(alt);
        }

        std::string message;

        if (!detectedGameRoot.empty())
        {
            message =
                INTERNAL_NAME_CONFIG " has been extracted to the wrong folder!\n"
                "Please move it from:\n\n" + currentLocation.string() +
                "\n\nto the detected game folder:\n\n" + detectedGameRoot.string();
        }
        else
        {
            message =
                INTERNAL_NAME_CONFIG " could not determine a valid game installation folder.\n"
                "\n"
                INTERNAL_NAME_CONFIG " must be placed directly inside the game's main folder (the one containing the \"Launcher\" and \"MGS4\"/\"mgspw\" subfolders), e.g.:\n"
                "\n"
                "  steamapps\\common\\METAL GEAR SOLID 4\\" INTERNAL_NAME_CONFIG "\n"
                "  steamapps\\common\\MGS_PW\\" INTERNAL_NAME_CONFIG "\n"
                "\n"
                "Current Location:\n\n" + currentLocation.string();
        }

        wxLogError(message);
        ExitProcess(1);
        return {};
    }

    void WarnIfAsiMissing(const std::filesystem::path& launcherDir, const std::filesystem::path& gameDir, const std::string& fileName)
    {
        static const std::array<std::filesystem::path, 3> subdirs = {
            std::filesystem::path("plugins"),
            std::filesystem::path("scripts"),
            std::filesystem::path("update")
        };

        auto hasAsi = [&](const std::filesystem::path& folder) -> bool
            {
                if (folder.empty())
                {
                    return false;
                }

                if (std::filesystem::exists(folder / (fileName + ".asi")))
                {
                    return true;
                }

                for (const auto& sub : subdirs)
                {
                    if (std::filesystem::exists(folder / sub / (fileName + ".asi")))
                    {
                        return true;
                    }
                }
                return false;
            };

        const bool launcherHasAsi = hasAsi(launcherDir);
        const bool gameHasAsi = hasAsi(gameDir);

        if (launcherHasAsi && gameHasAsi)
        {
            return;
        }

        auto describe = [](const std::filesystem::path& dir) -> std::string
            {
                return dir.empty() ? "(launcher/game folder not found)" : dir.string();
            };

        std::vector<std::string> missingDirs;
        if (!launcherHasAsi)
        {
            missingDirs.push_back(describe(launcherDir));
        }
        if (!gameHasAsi)
        {
            missingDirs.push_back(describe(gameDir));
        }

        std::string missingList;
        for (const auto& dir : missingDirs)
        {
            missingList += "  " + dir + "\n";
        }

        const bool bothMissing = missingDirs.size() == 2;
        const std::string explanation = bothMissing
            ? ("These folders each need their own copy of " + fileName + " for the fix to apply correctly. "
                "Please rextract the " + fileName + " zip exactly as it was packaged into the game folder.")
            : ("This folder needs its own copy of " + fileName + " for the fix to apply correctly. "
                "Please rextract the " + fileName + " zip exactly as it was packaged into the game folder.");

        wxLogError((fileName + ".asi is missing from:\n\n" + missingList + "\n" + explanation).c_str());
    }

    VersionCompareResult CompareSemanticVersion(const std::string& currentVersion,
        const std::string& targetVersion)
    {
        std::vector<int> currentParts = parseVersionString(currentVersion);
        std::vector<int> targetParts = parseVersionString(targetVersion);

        size_t n = std::max(currentParts.size(), targetParts.size());
        currentParts.resize(n, 0);
        targetParts.resize(n, 0);

        for (size_t i = 0; i < n; ++i)
        {
            if (currentParts[i] < targetParts[i])
            {
                return VersionCompareResult::Older;
            }
            if (currentParts[i] > targetParts[i])
            {
                return VersionCompareResult::Newer;
            }
        }
        return VersionCompareResult::Equal;
    }

    std::string GetFileDescription(const std::string& filePath)
    {
        DWORD handle = 0;
        DWORD size = GetFileVersionInfoSizeA(filePath.c_str(), &handle);
        if (size > 0)
        {
            std::vector<BYTE> versionInfo(size);
            if (GetFileVersionInfoA(filePath.c_str(), handle, size, versionInfo.data()))
            {
                void* buffer = nullptr;
                UINT sizeBuffer = 0;
                if (VerQueryValueA(versionInfo.data(), R"(\VarFileInfo\Translation)", &buffer, &sizeBuffer))
                {
                    auto translations = static_cast<WORD*>(buffer);
                    size_t translationCount = sizeBuffer / sizeof(WORD) / 2; // Each translation is two WORDs (language and code page)
                    for (size_t i = 0; i < translationCount; ++i)
                    {
                        WORD language = translations[i * 2];
                        WORD codePage = translations[i * 2 + 1];
                        // Construct the query string for the file description
                        std::ostringstream subBlock;
                        subBlock << R"(\StringFileInfo\)" << std::hex << std::setw(4) << std::setfill('0') << language
                            << std::setw(4) << std::setfill('0') << codePage << R"(\ProductName)";
                        if (VerQueryValueA(versionInfo.data(), subBlock.str().c_str(), &buffer, &sizeBuffer))
                        {
                            return std::string(static_cast<char*>(buffer), sizeBuffer - 1);
                        }
                    }
                }
            }
        }

        return "File description not found.";
    }

    bool IsSteamOS()
    {
        static bool bCheckedSteamDeck = false;
        static bool bIsSteamDeck = false;

        if (bCheckedSteamDeck)
        {
            return bIsSteamDeck;
        }

        bCheckedSteamDeck = true;
        // Check for Proton/Steam Deck environment variables
        if (std::getenv("STEAM_COMPAT_CLIENT_INSTALL_PATH") || std::getenv("STEAM_COMPAT_DATA_PATH") || std::getenv("XDG_SESSION_TYPE"))
        {
            bIsSteamDeck = true;
        }

        return bIsSteamDeck;
    }
}
