#include "stdafx.h"

#include "common.hpp"
#include "d3d11_api.hpp"

#include "gpu_check.hpp"
#include "logging.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "color_correction.hpp"
#include "input_handler.hpp"

#include "d3d11_text_overlay.hpp"

void afterPresent();

namespace
{
    bool g_preMenuFired = false;

    // Hooks
    SafetyHookInline CreateDXGIFactory_hook {};
    SafetyHookInline CreateSwapChain_hook {};
    SafetyHookInline ResizeBuffersHook {};

    using ResizeBuffersFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    ResizeBuffersFn oResizeBuffers = nullptr;




    void RefreshDeviceAndContext(IDXGISwapChain* swap)
    {
        ComPtr<ID3D11Device> device;
        if (SUCCEEDED(swap->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(device.GetAddressOf()))) && device)
        {
            g_D3D11Hooks.d3dDevice = device;

            ComPtr<ID3D11DeviceContext> context;
            device->GetImmediateContext(context.GetAddressOf());
            if (context)
            {
                g_D3D11Hooks.d3dDeviceContext = context;
                spdlog::info("D3D11 Device and Context refreshed successfully.");

                //HookDevice(device.Get());
            }
            else
            {
                spdlog::error("Failed to get ID3D11DeviceContext from ID3D11Device.");
            }
        }
        else
        {
            spdlog::error("Failed to get ID3D11Device from IDXGISwapChain.");
        }
    }


    //Don't hook Present directly, as streaming software like OBS might hook before us, resulting in our Present() effects not showing up on streams / in recordings.
    void BeforePresent(safetyhook::Context& ctx)
    {
        IDXGISwapChain* pSwapChain = reinterpret_cast<IDXGISwapChain*>(ctx.rcx);
        static bool firstInit = false;

        if (!firstInit)
        {
            firstInit = true;

            g_D3D11Hooks.swapChain = pSwapChain;
            RefreshDeviceAndContext(pSwapChain);

            // ==== GPU logging + driver version check ====
            IDXGIDevice* dxgiDevice = nullptr;
            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))) && dxgiDevice)
            {
                IDXGIAdapter* adapter = nullptr;
                if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)) && adapter)
                {
                    g_D3D11Hooks.dxgiAdapter = adapter;

                    DXGI_ADAPTER_DESC desc;
                    if (SUCCEEDED(adapter->GetDesc(&desc)))
                    {
                        std::string gpuName = Util::WideToUTF8(desc.Description);

                        LARGE_INTEGER driverVersion = {};
                        if (SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &driverVersion)))
                        {
                            UINT product = HIWORD(driverVersion.HighPart);
                            UINT version = LOWORD(driverVersion.HighPart);
                            UINT subVersion = HIWORD(driverVersion.LowPart);
                            UINT build = LOWORD(driverVersion.LowPart);

                            if (!Util::IsSteamOS())
                            {
                                CheckMinimumGPU(gpuName, true, product, version, subVersion, build);
                            }
                            else
                            {
                                spdlog::info("Running on SteamOS with GPU: {}. Driver version: {}.{}.{}.{}", gpuName, product, version, subVersion, build);
                            }
                        }
                        else
                        {
                            spdlog::warn("Could not query GPU driver version.");
                            spdlog::info("Running on GPU: {}", gpuName);
                        }
                    }
                }
                dxgiDevice->Release();
            }

            afterPresent();
        }

        {
            ComPtr<ID3D11Device> deviceFromSwap;
            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(deviceFromSwap.GetAddressOf()))))
            {
                static ComPtr<ID3D11Device> lastDevice;
                if (deviceFromSwap.Get() != lastDevice.Get())
                {
                    lastDevice = deviceFromSwap;
                    RefreshDeviceAndContext(pSwapChain);
                }
            }

        }

        g_InputHandler.Update();

        ColorCorrection::Draw(pSwapChain);


        D3D11TextOverlay::Tick(); //keep last.
        g_preMenuFired = false;
        g_D3D11Hooks.FrameCount++;
    }


    HRESULT __stdcall HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
    {
        HRESULT result = ResizeBuffersHook.call<HRESULT>(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

        if (SUCCEEDED(result))
        {
            RefreshDeviceAndContext(pSwapChain);
        }

        return result;
    }

    void HookSwapChainPresent(IDXGISwapChain* swapChain)
    {
        if (!swapChain || oResizeBuffers)
            return;

        void** vtable = *reinterpret_cast<void***>(swapChain);
        oResizeBuffers = reinterpret_cast<ResizeBuffersFn>(vtable[13]);
        ResizeBuffersHook = safetyhook::create_inline(vtable[13], reinterpret_cast<void*>(HookedResizeBuffers));
        LOG_HOOK(ResizeBuffersHook, "ResizeBuffersHook");
    }

    HRESULT __stdcall HookedCreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
    {

        HRESULT result = CreateSwapChain_hook.stdcall<HRESULT>(pFactory, pDevice, pDesc, ppSwapChain);
        if (SUCCEEDED(result) && ppSwapChain && *ppSwapChain)
        {
            g_D3D11Hooks.swapChain = *ppSwapChain;
            RefreshDeviceAndContext(*ppSwapChain);
            HookSwapChainPresent(*ppSwapChain);
        }
        else
        {
            spdlog::error("IDXGIFactory::CreateSwapChain failed. HRESULT: 0x{:08X}", result);
        }

        return result;
    }

    HRESULT WINAPI CreateDXGIFactory_hooked(REFIID riid, _COM_Outptr_ void** ppFactory)
    {
        HRESULT result = CreateDXGIFactory_hook.stdcall<HRESULT>(riid, ppFactory);

        if (SUCCEEDED(result))
        {
            g_D3D11Hooks.dxgiFactory = static_cast<IDXGIFactory*>(*ppFactory);
            void** vtable = *reinterpret_cast<void***>(g_D3D11Hooks.dxgiFactory.Get());
            CreateSwapChain_hook = safetyhook::create_inline(vtable[10], reinterpret_cast<void*>(HookedCreateSwapChain));
            LOG_HOOK(CreateSwapChain_hook, "CreateSwapChain.");
        }
        else
        {
            spdlog::error("CreateDXGIFactory failed. HRESULT: 0x{:08X}", result);
        }

        return result;
    }

}

void D3D11Hooks::Initialize()
{
    SPDLOG_ERROR("NULLSTUB - D3D11Hooks::Initialize() called. This should not happen.");
    return;
    if (!(eGameType & (MGS4|MGSPW)))
    {
        return;
    }

    spdlog::info("D3D11Hooks: Initializing D3D11 hooks.");
 //   //uint8_t* Present_scan = Memory::PatternScan(baseModule, eGameType & MGS2 ? "FF 50 ?? 8B F0" : "FF 50 ?? 48 8D 4C 24 ?? 8B F0", "D3D11Hooks: Before present hook");
 //   if (!Present_scan)
 //   {
 //       return;
 //   }
 //   
 //   PresentHook = safetyhook::create_mid(Present_scan, BeforePresent);
 //   spdlog::info("D3D11Hooks: BeforePresent hook installed successfully.");


    if (const HMODULE d3dcompiler = LoadLibraryA("d3dcompiler_43.dll"))
    {
        g_D3D11Hooks.D3DCompileFunc = reinterpret_cast<pD3DCompile>(GetProcAddress(d3dcompiler, "D3DCompile"));
        spdlog::info("D3D11Hooks: d3dcompiler_43.dll loaded successfully.");
    }
    else
    {
        spdlog::error("D3D11Hooks: failed to load d3dcompiler_43.dll");
    }

    CreateDXGIFactory_hook = safetyhook::create_inline(CreateDXGIFactory, reinterpret_cast<void*>(CreateDXGIFactory_hooked));
    LOG_HOOK(CreateDXGIFactory_hook, "CreateDXGIFactory");

}
