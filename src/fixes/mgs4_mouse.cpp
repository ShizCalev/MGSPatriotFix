#include "stdafx.h"

#include "mgs4_mouse.hpp"

#include "common.hpp"
#include "helper.hpp"
#include "logging.hpp"

namespace
{
    using GlfwSetInputMode_fn = void(__fastcall*)(uint32_t mode, uint32_t value);

    // GLFW_RAW_MOUSE_MOTION.
    constexpr uint32_t kGlfwRawMouseMotion = 0x00033005;

    GlfwSetInputMode_fn g_glfwSetInputMode = nullptr;

    // Roughly matches stock mouse sensitivity.
    constexpr float kStockMouseScale = 2.0f / 3.0f;

    // Leftover fraction from last frame's truncation, one pair per camera object.
    uintptr_t g_residualOwner = 0;
    float g_residualX = 0.0f;
    float g_residualY = 0.0f;

    void CameraScale_dest(SafetyHookContext& ctx)
    {
        const int32_t rawX = *reinterpret_cast<int32_t*>(ctx.rbp + 0xB0);
        const int32_t rawY = *reinterpret_cast<int32_t*>(ctx.rbp + 0xB8);

        if (ctx.rbx != g_residualOwner)
        {
            g_residualOwner = ctx.rbx;
            g_residualX = 0.0f;
            g_residualY = 0.0f;
        }

        const float scaledX = float(rawX) * kStockMouseScale * MGS4_RawMouseInput::fSensitivityX + g_residualX;
        const float scaledY = float(rawY) * kStockMouseScale * MGS4_RawMouseInput::fSensitivityY + g_residualY;

        const float stepX = std::floor(scaledX);
        const float stepY = std::floor(scaledY);

        g_residualX = scaledX - stepX;
        g_residualY = scaledY - stepY;

        ctx.rax = uint32_t(int32_t(stepX));
        ctx.rcx = uint32_t(int32_t(stepY));
    }
}

void MGS4_RawMouseInput::Apply()
{
    if (!(eGameType & MGS4))
        return;

    if (bRawInput)
    {
        if (uint8_t* GlfwSetInputMode_Scan = Memory::PatternScanUnique(baseModule, "48 89 5C 24 ?? 57 48 83 EC ?? 8B F9 8B DA 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C8", "MGS4_RawMouseInput: glfwSetInputMode"))
        {
            g_glfwSetInputMode = reinterpret_cast<GlfwSetInputMode_fn>(GlfwSetInputMode_Scan);

            // glfwSetInputMode(GLFWwindow*, int, int), window already bound. Runs right after the game disables the cursor, since GLFW only takes raw motion once that's done.
            MAKE_HOOK_MID(baseModule, "0F 57 C0 48 8D 54 24 ?? 48 8D 4C 24", "MGS4_RawMouseInput: enable raw input",
                          {
                              g_glfwSetInputMode(kGlfwRawMouseMotion, true);
                          });
        }
    }

    if (fSensitivityX == 1.0f && fSensitivityY == 1.0f)
        return;

    const std::vector<uint8_t*> CameraScale_Scans = Memory::FindMultiplePatternMatches(baseModule, "89 85 B0 00 00 00 89 8D B8 00 00 00");
    if (CameraScale_Scans.size() != 2)
    {
        spdlog::error("MGS4_RawMouseInput: camera scale sites: expected 2, found {}; leaving sensitivity and residual smoothing off.", CameraScale_Scans.size());
        return;
    }

    static SafetyHookMid CameraScale_hooks[2]{};
    for (size_t i = 0; i < CameraScale_Scans.size(); i++)
    {
        CameraScale_hooks[i] = safetyhook::create_mid(CameraScale_Scans[i], CameraScale_dest);
        LOG_HOOK(CameraScale_hooks[i], "MGS4_RawMouseInput: camera scale")
    }
}
