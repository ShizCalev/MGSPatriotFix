#include "stdafx.h"

#include "common.hpp"
#include "graphics_tuning.hpp"

#include "logging.hpp"


namespace
{
    int64_t __fastcall DisableMotionBlurHook()
    {
        return 0;
    }

    struct ShadowEntry
    {
        int32_t field;
        int32_t index;
        int32_t unknown[2];
        int32_t value;
        int32_t padding;
    };
    static_assert(sizeof(ShadowEntry) == 24);

    constexpr int32_t kShadowFieldSampleCount = 4;
    constexpr int32_t kShadowFieldBufferSize = 5;

    struct ShadowEntryVector
    {
        ShadowEntry* first;
        ShadowEntry* last;
        ShadowEntry* capacity;
    };

    SafetyHookInline ParseShadowSettings_hook{};

    void* ParseShadowSettings_dest(void* source, ShadowEntryVector* entries)
    {
        void* result = ParseShadowSettings_hook.call<void*>(source, entries);

        for (ShadowEntry* entry = entries->first; entry != entries->last; entry++)
        {
            if (entry->field == kShadowFieldBufferSize)
            {
                //spdlog::info("GraphicsTuning - Shadow Settings: default buffer size = {}", entry->value);
                entry->value = GraphicsTuning::iShadowBufferSize;
            }
						//samplecount needs a bit more cooking - it has a pretty hefty performance hit for minimal (if any) fidelity increase from testing.
           // else if (entry->field == kShadowFieldSampleCount)
           // {
           //     spdlog::info("GraphicsTuning - Shadow Settings: default sample count = {}", entry->value);
           //     if (GraphicsTuning::iShadowSampleCount > 0)
           //     {
           //         entry->value = GraphicsTuning::iShadowSampleCount;
           //     }
           // }
        }

        return result;
    }
}


void GraphicsTuning::Apply()
{
    if (eGameType & MGS4)
    {

        MAKE_HOOK_MID(baseModule, "FF 90 ?? ?? ?? ?? 4C 8B 4E", "GraphicsTuning - D3D11: Anisotropic level",
                      {
                          constexpr size_t offset = offsetof(D3D11_SAMPLER_DESC, MaxAnisotropy);
                          if (*reinterpret_cast<uint32_t*>(ctx.rdx + offset) == 8)
                          {
                              //SPDLOG_INFO("GraphicsTuning - D3D11: Anisotropic level: rdx = {:#x}, MaxAnisotropy = {}", ctx.rdx, *reinterpret_cast<uint32_t*>(ctx.rdx + offset));
                              *reinterpret_cast<uint32_t*>(ctx.rdx + offset) = iAnisotropicFiltering;
                          }
                      });

        MAKE_HOOK_MID(baseModule, "41 FF 91 ?? ?? ?? ?? 44 8B 5D", "GraphicsTuning - D3D12: Anisotropic level",
                      {
                          constexpr size_t offset = offsetof(D3D12_SAMPLER_DESC, MaxAnisotropy);
                          if (*reinterpret_cast<uint32_t*>(ctx.rdx + offset) == 8)
                          {
                              //SPDLOG_INFO("GraphicsTuning - D3D12: Anisotropic level: rdx = {:#x}, MaxAnisotropy = {}", ctx.rdx, *reinterpret_cast<uint32_t*>(ctx.rdx + offset));
                              *reinterpret_cast<uint32_t*>(ctx.rdx + offset) = iAnisotropicFiltering;
                          }
                      });

        if (bDisableDynamicResolution)
        {
            MAKE_HOOK_MID(baseModule, "E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 84 ?? ?? ?? ?? 41 B8 ?? ?? ?? ?? 8B 15", "GraphicsTuning - Force Dynamic Resolution Off",
                          {
                              ctx.rcx = 0;
                          });
        }

        if (bDisableMotionBlur)
        {
            constexpr char name[] = "GraphicsTuning - Disable Motion Blur";

            if (uint8_t* address = Memory::PatternScan(baseModule, "40 53 48 83 EC ?? 0F 29 74 24 ?? BA ?? ?? ?? ?? B9", name))
            {
                static SafetyHookInline hook {};
                hook = safetyhook::create_inline(address, reinterpret_cast<void*>(DisableMotionBlurHook));
                LOG_HOOK(hook, name)
            }
        }

        if (iShadowBufferSize > 0) //|| iShadowSampleCount > 0)
        {
            if (uint8_t* ShadowSettings_Scan = Memory::PatternScanUnique(baseModule, "48 89 5C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 55 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC ?? ?? ?? ?? 4C 8B EA", "GraphicsTuning - Shadow Settings"))
            {
                ParseShadowSettings_hook = safetyhook::create_inline(ShadowSettings_Scan, reinterpret_cast<void*>(ParseShadowSettings_dest));
                LOG_HOOK(ParseShadowSettings_hook, "GraphicsTuning - Shadow Settings")
            }
        }
    }


}
