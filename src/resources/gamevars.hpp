// ReSharper disable CppClangTidyClangDiagnosticUniqueObjectDuplication
#pragma once

#include "game_stages.hpp"


class GameVars final
{
public:
    void Initialize();

    ///Note: only accepts ASCII. If you need to use EUC-JP (any japanese characters), ask Afevis for their python hash generator.
    [[nodiscard]] static constexpr uint32_t GV_StrCode(const char* inputString)
    {
        constexpr uint32_t kBitLength = 24;
        constexpr uint32_t kBitMask = (1u << kBitLength) - 1;

        uint32_t hashValue = 0;

        for (const char* currentChar = inputString; *currentChar != '\0'; ++currentChar)
        {
            const auto characterValue = static_cast<unsigned char>(*currentChar);

            hashValue = ((hashValue << 5) | (hashValue >> (kBitLength - 5)));
            hashValue += characterValue;
            hashValue &= kBitMask;
        }

        return (hashValue == 0) ? 1 : hashValue;
    }


private:
    static void OnLevelTransition();

};

inline GameVars g_GameVars;

