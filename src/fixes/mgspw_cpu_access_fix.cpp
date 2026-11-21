#include "stdafx.h"

#include "common.hpp"
#include "mgspw_cpu_access_fix.hpp"

#include "logging.hpp"

#include <dxgi1_3.h>

#pragma comment(lib, "dxgi.lib")

namespace
{

    SafetyHookInline CreateDXGIFactory_hook {};
    SafetyHookInline CreateDXGIFactory1_hook {};
    SafetyHookInline CreateDXGIFactory2_hook {};
    SafetyHookInline CreateSwapChain_hook {};
    SafetyHookInline CreateTexture2D_hook {};

    HRESULT STDMETHODCALLTYPE HookedCreateTexture2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D)
    {
        D3D11_TEXTURE2D_DESC desc;
        if (pDesc && pDesc->Usage == D3D11_USAGE_DEFAULT && pDesc->CPUAccessFlags != 0)
        {
            desc = *pDesc;
            desc.CPUAccessFlags = 0;
            pDesc = &desc;
        }
        return CreateTexture2D_hook.stdcall<HRESULT>(device, pDesc, pInitialData, ppTexture2D);
    }

    void HookDevice(ID3D11Device* device)
    {
        if (!device || CreateTexture2D_hook)
        {
            return;
        }
        void** vtable = *reinterpret_cast<void***>(device);
        constexpr size_t kCreateTexture2D = 5;
        CreateTexture2D_hook = safetyhook::create_inline(vtable[kCreateTexture2D], reinterpret_cast<void*>(HookedCreateTexture2D));
        LOG_HOOK(CreateTexture2D_hook, "MGSPW - CPU Access Fix: ID3D11Device::CreateTexture2D")
    }

    HRESULT WINAPI HookedCreateSwapChain(IDXGIFactory* factory, IUnknown* device, DXGI_SWAP_CHAIN_DESC* desc, IDXGISwapChain** swapChain)
    {
        const HRESULT result = CreateSwapChain_hook.stdcall<HRESULT>(factory, device, desc, swapChain);
        if (SUCCEEDED(result) && device)
        {
            ID3D11Device* d3dDevice = nullptr;
            if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&d3dDevice))))
            {
                HookDevice(d3dDevice);
                d3dDevice->Release();
            }
        }
        return result;
    }

    void HookFactory(void* factory)
    {
        if (!factory || CreateSwapChain_hook)
        {
            return;
        }
        void** vtable = *reinterpret_cast<void***>(factory);
        constexpr size_t kCreateSwapChain = 10;
        CreateSwapChain_hook = safetyhook::create_inline(vtable[kCreateSwapChain], reinterpret_cast<void*>(HookedCreateSwapChain));
        LOG_HOOK(CreateSwapChain_hook, "MGSPW - CPU Access Fix: IDXGIFactory::CreateSwapChain")
    }

    HRESULT WINAPI HookedCreateDXGIFactory(REFIID riid, void** ppFactory)
    {
        const HRESULT result = CreateDXGIFactory_hook.stdcall<HRESULT>(riid, ppFactory);
        if (SUCCEEDED(result) && ppFactory)
        {
            HookFactory(*ppFactory);
        }
        return result;
    }

    HRESULT WINAPI HookedCreateDXGIFactory1(REFIID riid, void** ppFactory)
    {
        const HRESULT result = CreateDXGIFactory1_hook.stdcall<HRESULT>(riid, ppFactory);
        if (SUCCEEDED(result) && ppFactory)
        {
            HookFactory(*ppFactory);
        }
        return result;
    }

    HRESULT WINAPI HookedCreateDXGIFactory2(UINT flags, REFIID riid, void** ppFactory)
    {
        const HRESULT result = CreateDXGIFactory2_hook.stdcall<HRESULT>(flags, riid, ppFactory);
        if (SUCCEEDED(result) && ppFactory)
        {
            HookFactory(*ppFactory);
        }
        return result;
    }
}

void MGSPW_CpuAccessFix::Apply()
{
    if (!(eGameType & MGSPW))
    {
        return;
    }

    if (!bEnabled)
    {
        spdlog::info("MGSPW - CPU Access Fix: Disabled via config, skipping.");
        return;
    }

    CreateDXGIFactory_hook = safetyhook::create_inline(CreateDXGIFactory, reinterpret_cast<void*>(HookedCreateDXGIFactory));
    LOG_HOOK(CreateDXGIFactory_hook, "MGSPW - CPU Access Fix: CreateDXGIFactory")

    CreateDXGIFactory1_hook = safetyhook::create_inline(CreateDXGIFactory1, reinterpret_cast<void*>(HookedCreateDXGIFactory1));
    LOG_HOOK(CreateDXGIFactory1_hook, "MGSPW - CPU Access Fix: CreateDXGIFactory1")

    CreateDXGIFactory2_hook = safetyhook::create_inline(CreateDXGIFactory2, reinterpret_cast<void*>(HookedCreateDXGIFactory2));
    LOG_HOOK(CreateDXGIFactory2_hook, "MGSPW - CPU Access Fix: CreateDXGIFactory2")
}
