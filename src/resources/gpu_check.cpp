#include "stdafx.h"

#include "gpu_check.hpp"

#include "version_checking.hpp"
#include "spdlog/spdlog.h"

#ifndef MINIMUM_GPU_NAME
#define MINIMUM_GPU_NAME "NVIDIA GeForce GTX 980"
#endif

constexpr auto LATEST_NVIDIA_DRIVER_VERSION = "32.0.15.8129";
constexpr auto LATEST_AMD_DRIVER_VERSION = "32.0.21025.10016";

namespace
{
    struct GpuEntry
    {
        std::string model;
        int tier;
    };

    // Known GPU models. The table is sorted by model-name length at startup so
    // specific variants (e.g. "RTX 4070 TI SUPER") win over base models.
    std::vector<GpuEntry> gpuTable = {
        // NVIDIA GPUs
        {"GTX 650", 30}, {"GTX 650 TI", 35}, {"GTX 660", 50}, {"GTX 660 TI", 55},
        {"GTX 670", 70}, {"GTX 680", 80}, {"GTX 690", 90},
        {"GTX 750", 35}, {"GTX 750 TI", 40},
        {"GTX 950", 55}, {"GTX 960", 65}, {"GTX 970", 100}, {"GTX 980", 120}, {"GTX 980 TI", 140},
        {"GTX TITAN", 120}, {"GTX TITAN BLACK", 130}, {"GTX TITAN X", 150},
        {"GTX 1050", 60}, {"GTX 1050 TI", 70},
        {"GTX 1060 3GB", 110}, {"GTX 1060 6GB", 120}, {"GTX 1070", 150}, {"GTX 1070 TI", 160},
        {"GTX 1080", 170}, {"GTX 1080 TI", 200},
        {"GTX 1650", 85}, {"GTX 1650 SUPER", 95},
        {"GTX 1660", 125}, {"GTX 1660 SUPER", 135}, {"GTX 1660 TI", 135},

        {"RTX 2050", 100},
        {"RTX 2060", 180}, {"RTX 2060 SUPER", 190}, {"RTX 2070", 200}, {"RTX 2070 SUPER", 210},
        {"RTX 2080", 220}, {"RTX 2080 SUPER", 230}, {"RTX 2080 TI", 250},
        {"RTX 3050", 130}, {"RTX 3050 TI", 145},
        {"RTX 3060", 200}, {"RTX 3060 TI", 230}, {"RTX 3070", 260}, {"RTX 3070 TI", 280},
        {"RTX 3080", 300}, {"RTX 3080 TI", 320}, {"RTX 3090", 350}, {"RTX 3090 TI", 370},
        {"RTX 4050 LAPTOP GPU", 180},
        {"RTX 4060", 220}, {"RTX 4060 TI", 240},
        {"RTX 4070", 280}, {"RTX 4070 SUPER", 300}, {"RTX 4070 TI", 310}, {"RTX 4070 TI SUPER", 330},
        {"RTX 4080", 350}, {"RTX 4080 SUPER", 360}, {"RTX 4090", 400},
        {"RTX 5050", 190},
        {"RTX 5060", 270}, {"RTX 5060 TI", 300},
        {"RTX 5070", 340}, {"RTX 5070 TI", 370},
        {"RTX 5080", 430}, {"RTX 5090", 460},

        // AMD discrete GPUs
        {"R7 250", 30}, {"R7 250X", 35}, {"R7 260", 45}, {"R7 260X", 50},
        {"R9 270", 50}, {"R9 270X", 55}, {"R9 280", 70}, {"R9 280X", 80},
        {"R9 285", 85}, {"R9 290", 100}, {"R9 290X", 110}, {"R9 295X2", 130},
        {"RX 460", 50}, {"RX 470", 95}, {"RX 480", 100}, {"RX 550", 40}, {"RX 560", 55},
        {"RX 570", 100}, {"RX 580", 110}, {"RX 590", 115},
        {"RX VEGA 56", 130}, {"RX VEGA 64", 150},
        {"RX 5500 XT", 120}, {"RX 5600 XT", 140}, {"RX 5700", 160}, {"RX 5700 XT", 170},
        {"RX 6400", 75}, {"RX 6500 XT", 100},
        {"RX 6600", 170}, {"RX 6600 XT", 180}, {"RX 6650 XT", 190},
        {"RX 6700", 200}, {"RX 6700 XT", 220}, {"RX 6750 XT", 235},
        {"RX 6800", 270}, {"RX 6800 XT", 290},
        {"RX 6900 XT", 310}, {"RX 6950 XT", 320},
        {"RX 7600", 190}, {"RX 7600 XT", 200},
        {"RX 7700", 220}, {"RX 7700 XT", 230}, {"RX 7800 XT", 270},
        {"RX 7900 GRE", 300}, {"RX 7900 XT", 340}, {"RX 7900 XTX", 360},
        {"RX 9050", 150},
        {"RX 9060", 270}, {"RX 9060 XT", 290},
        {"RX 9070 GRE", 300}, {"RX 9070", 340}, {"RX 9070 XT", 370},

        // AMD integrated Radeon graphics
        {"RADEON 610M", 25},
        {"RADEON 660M", 55}, {"RADEON 680M", 75},
        {"RADEON 740M", 45}, {"RADEON 760M", 65}, {"RADEON 780M", 80},
        {"RADEON 820M", 30}, {"RADEON 840M", 55}, {"RADEON 860M", 70},
        {"RADEON 880M", 85}, {"RADEON 890M", 90},
        {"RADEON 8040S", 90}, {"RADEON 8050S", 190},
        {"RADEON 8060S", 220}, {"RADEON 8065S", 225},

        // Intel HD Graphics
        {"HD GRAPHICS", 3},
        {"HD GRAPHICS 2000", 3},
        {"HD GRAPHICS 2500", 4},
        {"HD GRAPHICS 3000", 5},
        {"HD GRAPHICS 4000", 10},
        {"HD GRAPHICS 4200", 12},
        {"HD GRAPHICS 4400", 13},
        {"HD GRAPHICS 4600", 15},
        {"HD GRAPHICS 5000", 18},
        {"HD GRAPHICS 505", 19},
        {"HD GRAPHICS 510", 20},
        {"HD GRAPHICS 515", 20},
        {"HD GRAPHICS 520", 22},
        {"HD GRAPHICS 530", 24},
        {"HD GRAPHICS 6000", 26},

        // Intel UHD Graphics
        {"UHD GRAPHICS 600", 25},
        {"UHD GRAPHICS 610", 28},
        {"UHD GRAPHICS 615", 30},
        {"UHD GRAPHICS 620", 32},
        {"UHD GRAPHICS 630", 35},
        {"UHD GRAPHICS 710", 32},
        {"UHD GRAPHICS 730", 38},
        {"UHD GRAPHICS 750", 40},
        {"UHD GRAPHICS 770", 45},

        // Intel Iris / Iris Pro / Iris Plus
        {"IRIS", 30},
        {"IRIS PRO 5200", 32},
        {"IRIS PRO 580", 40},
        {"IRIS PLUS 540", 35},
        {"IRIS PLUS 550", 38},
        {"IRIS PLUS 655", 40},
        {"IRIS XE", 60},

        // Intel Arc discrete GPUs
        {"ARC A310", 90},
        {"ARC A380", 120},
        {"ARC A580", 170},
        {"ARC A750", 210},
        {"ARC A770", 230},
        {"ARC B570", 200},
        {"ARC B580", 225},

        // Intel Arc integrated GPUs
        {"ARC GRAPHICS 130V", 80}, {"ARC 130V", 80},
        {"ARC GRAPHICS 140V", 95}, {"ARC 140V", 95},
        {"ARC GRAPHICS 130T", 80}, {"ARC 130T", 80},
        {"ARC GRAPHICS 140T", 90}, {"ARC 140T", 90},
        {"ARC B370", 105}, {"ARC B390", 115},
        {"ARC GRAPHICS", 75},
    };

    // Sort GPU table descending by model length for potential substring matching (not strictly necessary here)
    struct TableSorter
    {
        TableSorter()
        {
            std::sort(gpuTable.begin(), gpuTable.end(),
                [](const GpuEntry& a, const GpuEntry& b)
                {
                    return a.model.size() > b.model.size();
                });
        }
    } tableSorter;

    std::string SanitizeGPUName(const std::string& name)
    {
        std::string sanitized = name;
        // Use regex to remove (c), (r), (tm) case insensitive, with optional spaces inside parentheses
        std::regex trademark_re(R"(\(\s*(c|r|tm)\s*\))", std::regex_constants::icase);
        sanitized = std::regex_replace(sanitized, trademark_re, "");
        // Also trim extra spaces left after removal
        // Simple trim leading and trailing spaces:
        sanitized.erase(sanitized.begin(), std::find_if(sanitized.begin(), sanitized.end(), [](unsigned char ch)
            {
                return !std::isspace(ch);
            }));
        sanitized.erase(std::find_if(sanitized.rbegin(), sanitized.rend(), [](unsigned char ch)
            {
                return !std::isspace(ch);
            }).base(), sanitized.end());

        return sanitized;
    }

    // Convert string to uppercase for case-insensitive matching
    std::string ToUpper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c)
            {
                return std::toupper(c);
            });
        return str;
    }

    // Extract vendor from GPU name
    std::string GetVendor(const std::string& name)
    {
        if (name.find("NVIDIA") != std::string::npos) return "NVIDIA";
        if (name.find("AMD") != std::string::npos || name.find("RADEON") != std::string::npos) return "AMD";
        if (name.find("INTEL") != std::string::npos) return "INTEL";
        return "UNKNOWN";
    }

    // Estimate NVIDIA GPU tier from the numeric model when there is no known
    // table match. Keep generations separated so e.g. RTX 4050 cannot fall
    // through into the RTX 30-series comparisons.
    int EstimateNVidia(int model)
    {
        // RTX 50 series
        if (model >= 5000 && model < 5100)
        {
            if (model >= 5090) return 460;
            if (model >= 5080) return 430;
            if (model >= 5070) return 340;
            if (model >= 5060) return 270;
            if (model >= 5050) return 190;
            return 180;
        }

        // RTX 40 series
        if (model >= 4000 && model < 5000)
        {
            if (model >= 4090) return 400;
            if (model >= 4080) return 350;
            if (model >= 4070) return 280;
            if (model >= 4060) return 220;
            if (model >= 4050) return 180;
            return 160;
        }

        // RTX 30 series
        if (model >= 3000 && model < 4000)
        {
            if (model >= 3090) return 350;
            if (model >= 3080) return 300;
            if (model >= 3070) return 260;
            if (model >= 3060) return 200;
            if (model >= 3050) return 130;
            return 120;
        }

        // RTX 20 series
        if (model >= 2000 && model < 3000)
        {
            if (model >= 2080) return 220;
            if (model >= 2070) return 200;
            if (model >= 2060) return 180;
            if (model >= 2050) return 100;
            return 90;
        }

        // GTX 16 series
        if (model >= 1600 && model < 1700)
        {
            if (model >= 1660) return 125;
            if (model >= 1650) return 85;
            return 75;
        }

        // GTX 10 series
        if (model >= 1000 && model < 1100)
        {
            if (model >= 1080) return 170;
            if (model >= 1070) return 150;
            if (model >= 1060) return 120;
            if (model >= 1050) return 60;
            return 50;
        }

        // GTX 900 series
        if (model >= 900 && model < 1000)
        {
            if (model >= 980) return 120;
            if (model >= 970) return 100;
            if (model >= 960) return 65;
            if (model >= 950) return 55;
            return 45;
        }

        // GTX 700 series
        if (model >= 700 && model < 800)
        {
            if (model >= 780) return 100;
            if (model >= 770) return 80;
            if (model >= 760) return 55;
            if (model >= 750) return 35;
            return 30;
        }

        // GTX 600 series
        if (model >= 600 && model < 700)
        {
            if (model >= 690) return 90;
            if (model >= 680) return 80;
            if (model >= 670) return 70;
            if (model >= 660) return 50;
            if (model >= 650) return 30;
            return 25;
        }

        return 20;
    }

    // Estimate AMD RX GPU tier from the numeric model when there is no known
    // table match. As above, keep each generation in its own range.
    int EstimateAMD(int model)
    {
        // RX 9000 series
        if (model >= 9000 && model < 10000)
        {
            if (model >= 9070) return 340;
            if (model >= 9060) return 270;
            if (model >= 9050) return 150;
            return 140;
        }

        // RX 7000 series
        if (model >= 7000 && model < 8000)
        {
            if (model >= 7900) return 340;
            if (model >= 7800) return 270;
            if (model >= 7700) return 220;
            if (model >= 7600) return 190;
            return 170;
        }

        // RX 6000 series
        if (model >= 6000 && model < 7000)
        {
            if (model >= 6900) return 310;
            if (model >= 6800) return 270;
            if (model >= 6700) return 200;
            if (model >= 6600) return 170;
            if (model >= 6500) return 100;
            if (model >= 6400) return 75;
            return 65;
        }

        // RX 5000 series
        if (model >= 5000 && model < 6000)
        {
            if (model >= 5700) return 160;
            if (model >= 5600) return 140;
            if (model >= 5500) return 120;
            return 100;
        }

        // RX 400/500 series
        if (model >= 500 && model < 600)
        {
            if (model >= 590) return 115;
            if (model >= 580) return 110;
            if (model >= 570) return 100;
            if (model >= 560) return 55;
            if (model >= 550) return 40;
            return 35;
        }

        if (model >= 400 && model < 500)
        {
            if (model >= 480) return 100;
            if (model >= 470) return 95;
            if (model >= 460) return 50;
            return 35;
        }

        return 20;
    }

    int EstimateIntelArc(const std::string& series, int model, const std::string& variant)
    {
        if (series == "A")
        {
            if (model >= 770) return 230;
            if (model >= 750) return 210;
            if (model >= 580) return 170;
            if (model >= 380) return 120;
            if (model >= 310) return 90;
            return 75;
        }

        if (series == "B")
        {
            // B5xx = discrete Battlemage, B3xx = Panther Lake integrated.
            if (model >= 580) return 225;
            if (model >= 570) return 200;
            if (model >= 390) return 115;
            if (model >= 370) return 105;
            return 90;
        }

        if (variant == "V")
        {
            if (model >= 140) return 95;
            if (model >= 130) return 80;
        }
        else if (variant == "T")
        {
            if (model >= 140) return 90;
            if (model >= 130) return 80;
        }

        return 75;
    }

    // Parse an otherwise-unknown GPU string and produce a conservative estimate.
    int ParseAndEstimate(const std::string& name)
    {
        // Intel Arc uses several naming schemes:
        // ARC A770 / B580, ARC B390, ARC GRAPHICS 140V, etc.
        {
            std::regex arcRe(R"(ARC(?:\s+GRAPHICS)?\s*([AB]?)([0-9]{3})([VT])?)");
            std::smatch arcMatch;
            if (std::regex_search(name, arcMatch, arcRe))
            {
                const std::string series = arcMatch[1].str();
                const int modelNum = std::stoi(arcMatch[2].str());
                const std::string variant = arcMatch[3].matched ? arcMatch[3].str() : "";
                return EstimateIntelArc(series, modelNum, variant);
            }
        }

        // Intel HD/UHD adapter names normally include the word "GRAPHICS"
        // between the family and model number.
        {
            std::regex intelGraphicsRe(R"((UHD|HD)\s+GRAPHICS\s*([0-9]{3,4}))");
            std::smatch intelGraphicsMatch;
            if (std::regex_search(name, intelGraphicsMatch, intelGraphicsRe))
            {
                const std::string prefix = intelGraphicsMatch[1].str();
                const int modelNum = std::stoi(intelGraphicsMatch[2].str());

                if (prefix == "UHD")
                {
                    return (modelNum >= 700) ? 35 : 25;
                }

                return 10;
            }
        }

        // Other model names: prefix, numeric model, optional performance suffix.
        std::regex re(R"((GTX|RTX|RX|HD|IRIS|UHD|XE)\s*([0-9]{3,4})(?:\s*(TI|SUPER|ULTRA|XTX|XT|GRE))?)");
        std::smatch match;
        if (std::regex_search(name, match, re))
        {
            const std::string prefix = match[1].str();
            const int modelNum = std::stoi(match[2].str());
            const std::string suffix = (match.size() >= 4 && match[3].matched) ? match[3].str() : "";

            int baseTier = 0;
            if (prefix == "GTX" || prefix == "RTX")
            {
                baseTier = EstimateNVidia(modelNum);
            }
            else if (prefix == "RX")
            {
                baseTier = EstimateAMD(modelNum);
            }
            else if (prefix == "IRIS")
            {
                baseTier = 35;
            }
            else if (prefix == "HD")
            {
                baseTier = 10;
            }
            else if (prefix == "UHD")
            {
                baseTier = (modelNum >= 700) ? 35 : 25;
            }
            else if (prefix == "XE")
            {
                baseTier = 60;
            }

            if (baseTier <= 0)
            {
                return 0;
            }

            // These are only fallback estimates. Known variants should have
            // already matched the explicit table before reaching this point.
            if (suffix == "TI")
            {
                baseTier = static_cast<int>(baseTier * 1.10);
            }
            else if (suffix == "SUPER")
            {
                baseTier = static_cast<int>(baseTier * 1.15);
            }
            else if (suffix == "ULTRA")
            {
                baseTier = static_cast<int>(baseTier * 1.20);
            }
            else if (suffix == "XT")
            {
                baseTier = static_cast<int>(baseTier * 1.10);
            }
            else if (suffix == "XTX")
            {
                baseTier = static_cast<int>(baseTier * 1.15);
            }
            else if (suffix == "GRE")
            {
                baseTier = static_cast<int>(baseTier * 0.90);
            }

            return baseTier;
        }

        return 0;
    }

    bool IsGenericTableEntry(const std::string& model)
    {
        return model == "HD GRAPHICS"
            || model == "IRIS"
            || model == "ARC GRAPHICS";
    }

    // Get the tier for the GPU name:
    // 1) Try exact match.
    // 2) Try specific known models as substrings (longest names first).
    // 3) Fall back to parsing/estimating an unknown model.
    // 4) Finally try generic catch-all table entries.
    int GetTier(const std::string& upperName)
    {
        for (const auto& entry : gpuTable)
        {
            if (upperName == entry.model)
            {
                return entry.tier;
            }
        }

        for (const auto& entry : gpuTable)
        {
            if (!IsGenericTableEntry(entry.model) && upperName.find(entry.model) != std::string::npos)
            {
                return entry.tier;
            }
        }

        const int estimatedTier = ParseAndEstimate(upperName);
        if (estimatedTier > 0)
        {
            return estimatedTier;
        }

        for (const auto& entry : gpuTable)
        {
            if (IsGenericTableEntry(entry.model) && upperName.find(entry.model) != std::string::npos)
            {
                return entry.tier;
            }
        }

        return 0;
    }

    // Compute minimum tier once, at startup, from MINIMUM_GPU_NAME
    const int kMinimumTier = []
        {
            std::string minUpper = ToUpper(MINIMUM_GPU_NAME);
            int tier = GetTier(minUpper);
            if (tier == 0) tier = ParseAndEstimate(minUpper);
            if (tier == 0)
            {
                spdlog::warn("Minimum GPU \"{}\" could not be recognized - defaulting to tier 100.", MINIMUM_GPU_NAME);
                return 100;
            }
            return tier;
        }();


} // namespace

void CheckMinimumGPU(const std::string& gpuName, bool logDriver, UINT product, UINT version, UINT subVersion, UINT build)
{
    static bool alreadyWarned = false;
    if (alreadyWarned)
    {
        return;
    }
    std::string sanitizedName = SanitizeGPUName(gpuName);
    if (std::string_view(ToUpper(sanitizedName)).starts_with("LLVMPIPE"))
    {
        spdlog::info("Game is running on Proton Compatibility Layer: {}", sanitizedName);
        alreadyWarned = true;
        return;
    }

    std::string upper = ToUpper(sanitizedName);
    std::string vendor = GetVendor(upper);

    int tier = GetTier(upper);

    if (tier == 0 && vendor == "INTEL")
    {
        spdlog::warn("GPU WARNING: {} detected as Intel integrated graphics.", sanitizedName);
        tier = 20; // fallback for unrecognized Intel
    }

    if (tier == 0)
    {
        spdlog::warn("");
        spdlog::warn("=====================================    GPU WARNING     =====================================");
        spdlog::warn("GPU WARNING: {} ({}) was not recognized.", sanitizedName, vendor);
        spdlog::warn("GPU WARNING: The game requires a minimum of a {} or equivalent.", MINIMUM_GPU_NAME);
        spdlog::warn("GPU WARNING: Degraded performance (ie \"Snake moving in slow motion\") and crashing likely to occur.");
        spdlog::warn("=====================================    GPU WARNING     =====================================");
        spdlog::warn("");
        alreadyWarned = true;
        return;
    }


    if (tier < kMinimumTier)
    {
        spdlog::warn("");
        spdlog::warn("=====================================    GPU WARNING     =====================================");
        spdlog::warn("GPU WARNING: This GPU is below the minimum system requirements of a {} or equivalent.", MINIMUM_GPU_NAME);
        int percent = tier * 100 / kMinimumTier;
        spdlog::warn("GPU WARNING: Estimated performance compared to a {}: {}%", MINIMUM_GPU_NAME, percent);
        spdlog::warn("GPU WARNING: Degraded performance (ie \"Snake moving in slow motion\") and crashing likely to occur.");
        spdlog::warn("=====================================    GPU WARNING     =====================================");
        spdlog::warn("");
        alreadyWarned = true;
        return;
    }

    if (!logDriver)
    {
        return;
    }

    std::string driverVersion = fmt::format("{}.{}.{}.{}", product, version, subVersion, build);

    spdlog::info("Game is running on GPU: {} (Driver Version: {})", sanitizedName, driverVersion);

    if ((vendor == "NVIDIA" || vendor == "AMD") && (VersionCheck::CompareSemanticVersion(driverVersion, vendor == "NVIDIA" ? LATEST_NVIDIA_DRIVER_VERSION : LATEST_AMD_DRIVER_VERSION) == VersionCheck::CompareResult::Older))
    {
        spdlog::warn("");
        spdlog::warn("=====================================    GPU WARNING     =====================================");
        spdlog::warn("GPU WARNING: Your {} graphics drivers are out of date.", vendor);
        spdlog::warn("GPU WARNING: Outdated drivers can cause performance and stability issues.");
        spdlog::warn("GPU WARNING: Please update to the latest driver version from the vendor's website.");
        spdlog::warn("=====================================    GPU WARNING     =====================================");
        spdlog::warn("");
        alreadyWarned = true;
    }


}
