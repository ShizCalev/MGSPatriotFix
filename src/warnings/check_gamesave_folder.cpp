#include "stdafx.h"
#include "check_gamesave_folder.hpp"

#include "common.hpp"
#include "logging.hpp"


void CheckGamesaveFolderWritable::CheckStatus()
{
    if (bVerifySameDirectoryWriteable)
    {
        spdlog::info("Verifying gamesave directory is writeable: {}", sGameSavePath.string());


        bool success = true;

        try
        {
            if (!std::filesystem::exists(sGameSavePath))
            {
                spdlog::info("Directory '{}' does not exist. Creating...", sGameSavePath.string());
                std::filesystem::create_directories(sGameSavePath);
            }
        }
        catch (const std::exception& ex)
        {
            spdlog::error("Failed to create folder '{}': {}", sGameSavePath.string(), ex.what());
            Logging::ShowConsole();
            std::cerr << "Error creating folder '" << sGameSavePath << "': " << ex.what() << std::endl;
            success = false;
        }


        if (success)
        {

            try
            {
                for (const auto& entry : std::filesystem::directory_iterator(sGameSavePath))
                {
                    if (!entry.is_directory())
                    {
                        continue;
                    }

                    const auto& dir = entry.path();
                    const auto testFile = dir / "write_test.tmp";

                    std::error_code ec;

                    std::filesystem::remove(testFile, ec);
                    if (ec)
                    {
                        spdlog::error("Failed to remove leftover test file in '{}': {}", dir.string(), ec.message());
                        Logging::ShowConsole();
                        std::cerr << "ERROR: Failed to remove leftover save test file from a previous launch in " << dir << ": " << ec.message() << std::endl;
                        std::cerr << "ERROR: This typically indicates that the folder is set to read-only, which breaks game saving." << std::endl;
                        success = false;
                        continue;
                    }

                    std::ofstream ofs(testFile, std::ios::out | std::ios::trunc);
                    if (!ofs.is_open())
                    {
                        spdlog::error("Cannot write to directory '{}'. Please check folder permissions.", dir.string());
                        Logging::ShowConsole();
                        std::cerr << "ERROR: Cannot write to " << dir << std::endl;
                        std::cerr << "ERROR: This typically indicates that the folder is set to read-only, which breaks game saving." << std::endl;
                        success = false;
                        continue;
                    }

                    ofs << "test";
                    ofs.close();

                    std::filesystem::remove(testFile, ec);
                    if (ec)
                    {
                        spdlog::error("Failed to remove temporary save test file in '{}': {}", dir.string(), ec.message());
                        success = false;
                        continue;
                    }
                }
            }
            catch (const std::exception& ex)
            {
                spdlog::error("Exception while checking save folder writability: {}", ex.what());
                Logging::ShowConsole();
                std::cerr << "Error while checking save folder writability: " << ex.what() << std::endl;
                success = false;
            }
        }
        if (success)
        {
            spdlog::info("All gamesave folders are writable.");
        }
    }

    if (bCheckSaveFilesReadOnly)
    {
        spdlog::info("Checking individual save files for read-only attribute.");

        size_t readOnlyCount = 0;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(sGameSavePath))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto& filePath = entry.path();

            if (Util::IsFileReadOnly(filePath))
            {
                spdlog::warn("Detected read-only save file: {}", filePath.string());
                Logging::ShowConsole();

                std::wcout << L"[WARNING: READ-ONLY FILE] " << filePath << std::endl;
                ++readOnlyCount;
            }
        }

        if (readOnlyCount > 0)
        {
            std::cout << "\nWARNING: " << readOnlyCount << " file" << (readOnlyCount > 1 ? "s are" : " is") << " set to read-only in the Save folder." << std::endl;
            std::cout << "Game saving may fail until these files are made writable.\n" << std::endl;

            spdlog::warn("{} save file{} detected as read-only.", readOnlyCount, (readOnlyCount > 1 ? "s" : ""));
            spdlog::warn("Game saving may fail until these files are made writable.");
        }
        else
        {
            spdlog::info("All gamesave folders and files verified as writable.");
        }
    }

    
}
