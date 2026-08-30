#include "stdafx.h"

#include "common.hpp"
#include "graphics_tuning.hpp"

#include "logging.hpp"


void GraphicsTuning::ApplyHooks()
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
    }


}
