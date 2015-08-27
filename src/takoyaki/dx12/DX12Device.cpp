// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "pch.h"
#include "DX12Device.h"

#include <thread>

#include "DX12DeviceContext.h"
#include "DX12DescriptorHeap.h"
#include "DX12Texture.h"
#include "DXUtility.h"
#include "../utility/log.h"
#include "../public/framework.h"

namespace Takoyaki
{
    DX12Device::DX12Device()
        : window_(nullptr)
        , bufferCount_(0)
        , currentFrame_(0)
    {
    }

    void DX12Device::create(const FrameworkDesc& desc)
    {
        bufferCount_ = desc.bufferCount;
        window_ = reinterpret_cast<IUnknown*>(desc.windowHandle);
        currentOrientation_ = desc.currentOrientation;
        nativeOrientation_ = desc.nativeOrientation;
        dpi_ = desc.windowDpi;
        windowSize_ = desc.windowSize;

        createDevice(bufferCount_);
        createSwapChain();
    }

    void DX12Device::createDevice(uint_fast32_t bufferCount)
    {
        LOGC << "Creating D3D12 Device...";

#if defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
        }
#endif

        DXCheckThrow(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory_)));

        // Create the Direct3D 12 API device object
        HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice_));

        if (FAILED(hr)) {
            LOGW << "ID3D12Device initialization fails, falling back to the WARP device.";

            Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;

            DXGIFactory_->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
            DXCheckThrow(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice_)));
        }

        // Create the command queue.
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        DXCheckThrow(D3DDevice_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_)));

        commandAllocators_.resize(bufferCount);

        for (size_t i = 0; i < bufferCount; ++i) {
            DXCheckThrow(D3DDevice_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators_[i])));
        }

        // Create synchronization objects.
        fenceValues_.resize(bufferCount);
        std::fill(fenceValues_.begin(), fenceValues_.end(), 0);
        DXCheckThrow(D3DDevice_->CreateFence(fenceValues_[currentFrame_], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
        fenceValues_[currentFrame_]++;
        fenceEvent_ = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        
        // Create a context for the main thread        
        contexts_.insert(std::make_pair(std::this_thread::get_id(), std::make_shared<DX12DeviceContext>(shared_from_this())));
    }

    void DX12Device::createSwapChain()
    {
        LOGC << "Creating swap chain...";

        waitForGpu();

        // Calculate the necessary render target size in pixels.
        glm::vec2 outSize;

        outSize.x = ConvertDipsToPixels(windowSize_.x, dpi_);
        outSize.y = ConvertDipsToPixels(windowSize_.y, dpi_);

        // The width and height of the swap chain must be based on the window's
        // natively-oriented width and height. If the window is not in the native
        // orientation, the dimensions must be reversed.
        DXGI_MODE_ROTATION displayRotation = getDXGIOrientation();

        if (displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270)
            std::swap(outSize.x, outSize.y);

        viewport_ = { 0.0f, 0.0f, outSize.x, outSize.y, 0.0f, 1.0f };

        switch (displayRotation) {
            case DXGI_MODE_ROTATION_IDENTITY:
                matDeviceRotation_ = glm::mat4(1.0f);
                break;

            case DXGI_MODE_ROTATION_ROTATE90:
                matDeviceRotation_ = glm::rotate(glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
                break;

            case DXGI_MODE_ROTATION_ROTATE180:
                matDeviceRotation_ = glm::rotate(glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f));
                break;

            case DXGI_MODE_ROTATION_ROTATE270:
                matDeviceRotation_ = glm::rotate(glm::radians(270.f), glm::vec3(0.f, 0.f, 1.f));
                break;

            default:
                throw std::runtime_error("DX12Device::createSwapChain, displayRotation");
        }

        if (swapChain_ != nullptr) {
            // TODO: Do proper handling or DXGI_ERROR_DEVICE_REMOVED and DXGI_ERROR_DEVICE_RESET
            DXCheckThrow(swapChain_->ResizeBuffers(bufferCount_, lround(outSize.x), lround(outSize.y), DXGI_FORMAT_B8G8R8A8_UNORM, 0));
        } else {
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

            swapChainDesc.Width = lround(outSize.x);
            swapChainDesc.Height = lround(outSize.y);
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = bufferCount_;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.Flags = 0;
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

            Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

            DXCheckThrow(DXGIFactory_->CreateSwapChainForCoreWindow(commandQueue_.Get(), window_, &swapChainDesc, nullptr, &swapChain));
            DXCheckThrow(swapChain.As(&swapChain_));
        }

        DXCheckThrow(swapChain_->SetRotation(displayRotation));


        // All pending GPU work was already finished. Update the tracked fence values
        // to the last value signaled.
        for (uint_fast32_t i = 0; i < bufferCount_; i++) {
            fenceValues_[i] = fenceValues_[currentFrame_];
        }

        currentFrame_ = 0;

        auto context = contexts_[std::this_thread::get_id()].get();

        for (uint_fast32_t i = 0; i < bufferCount_; ++i) {
            auto tex = context->CreateTexture();
            auto& res = tex->getResource();

            DXCheckThrow(swapChain_->GetBuffer(i, IID_PPV_ARGS(&res)));            
            D3DDevice_->CreateRenderTargetView(res.Get(), nullptr, tex->getRenderTargetView());

            auto fmt = boost::wformat(L"Swap chain Render Target %1%") % i;

            res->SetName(boost::str(fmt).c_str());
        }
    }

    DXGI_MODE_ROTATION DX12Device::getDXGIOrientation() const
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        // Note: NativeOrientation can only be Landscape or Portrait even though
        // the DisplayOrientations enum has other values.
        switch (nativeOrientation_) {
            case EDisplayOrientation::LANDSCAPE:
                switch (currentOrientation_) {
                    case EDisplayOrientation::LANDSCAPE:
                        rotation = DXGI_MODE_ROTATION_IDENTITY;
                        break;

                    case EDisplayOrientation::PORTRAIT:
                        rotation = DXGI_MODE_ROTATION_ROTATE270;
                        break;

                    case EDisplayOrientation::LANDSCAPE_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE180;
                        break;

                    case EDisplayOrientation::PORTRAIT_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE90;
                        break;

                    default:
                        throw std::runtime_error("DX12Device::GetDXGIOrientation, nativeOrientation_");
                }
                break;

            case EDisplayOrientation::PORTRAIT:
                switch (currentOrientation_) {
                    case EDisplayOrientation::LANDSCAPE:
                        rotation = DXGI_MODE_ROTATION_ROTATE90;
                        break;

                    case EDisplayOrientation::PORTRAIT:
                        rotation = DXGI_MODE_ROTATION_IDENTITY;
                        break;

                    case EDisplayOrientation::LANDSCAPE_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE270;
                        break;

                    case EDisplayOrientation::PORTRAIT_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE180;
                        break;

                    default:
                        throw std::runtime_error("DX12Device::GetDXGIOrientation, currentOrientation_");
                }
                break;
        }
        return rotation;
    }

    void DX12Device::present()
    {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        auto res = swapChain_->Present(1, 0);

        // If the device was removed either by a disconnection or a driver upgrade, we 
        // must recreate all device resources.
        if (res == DXGI_ERROR_DEVICE_REMOVED || res == DXGI_ERROR_DEVICE_RESET) {
            // TODO: Apparently this can only happen for on mobile devices
            // when the application is killed by the OS. Add proper support
            throw new std::runtime_error("Device removal not implemented");
            D3DDevice_.Reset();
        } else {
            DXCheckThrow(res);

            // Schedule a Signal command in the queue.
            auto current = fenceValues_[currentFrame_];
            DXCheckThrow(commandQueue_->Signal(fence_.Get(), current));

            // Advance the frame index.
            currentFrame_ = (currentFrame_ + 1) % bufferCount_;

            // Check to see if the next frame is ready to start.
            if (fence_->GetCompletedValue() < fenceValues_[currentFrame_]) {
                DXCheckThrow(fence_->SetEventOnCompletion(fenceValues_[currentFrame_], fenceEvent_));
                WaitForSingleObjectEx(fenceEvent_, INFINITE, FALSE);
            }

            // Set the fence value for the next frame.
            fenceValues_[currentFrame_]++;
        }
    }

    void DX12Device::setProperty(EPropertyID id, const boost::any& value)
    {
        switch (id) {
            case Takoyaki::EPropertyID::WINDOW_SIZE:
                windowSize_ = boost::any_cast<glm::vec2>(value);
                break;

            case Takoyaki::EPropertyID::WINDOW_ORIENTATION:
                currentOrientation_ = boost::any_cast<EDisplayOrientation>(value);
                break;

            case Takoyaki::EPropertyID::WINDOW_DPI:
                dpi_ = boost::any_cast<float>(value);
                break;

            default:
                throw new std::runtime_error("DX12Device::setProperty, id");
                break;
        }

        createSwapChain();
    }

    void DX12Device::validate()
    {
        // The D3D Device is no longer valid if the default adapter changed since the device
        // was created or if the device has been removed.

        // First, get the LUID for the adapter from when the device was created.

        LUID previousAdapterLuid = D3DDevice_->GetAdapterLuid();

        // Next, get the information for the current default adapter.

        Microsoft::WRL::ComPtr<IDXGIFactory2> currentFactory;

        DXCheckThrow(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

        Microsoft::WRL::ComPtr<IDXGIAdapter1> currentDefaultAdapter;

        DXCheckThrow(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

        DXGI_ADAPTER_DESC currentDesc;

        DXCheckThrow(currentDefaultAdapter->GetDesc(&currentDesc));

        // If the adapter LUIDs don't match, or if the device reports that it has been removed,
        // a new D3D device must be created.

        if ((previousAdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart) ||
            (previousAdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart) ||
            (D3DDevice_->GetDeviceRemovedReason() < 0)) {
            
            // TODO: Apparently this can only happen for on mobile devices
            // when the application is killed by the OS. Add proper support
            throw new std::runtime_error("Device removal not implemented");
            D3DDevice_.Reset();
        }
    }

    void DX12Device::waitForGpu()
    {
        // Schedule a Signal command in the queue.
        DXCheckThrow(commandQueue_->Signal(fence_.Get(), fenceValues_[currentFrame_]));

        // Wait until the fence has been crossed.
        DXCheckThrow(fence_->SetEventOnCompletion(fenceValues_[currentFrame_], fenceEvent_));
        WaitForSingleObjectEx(fenceEvent_, INFINITE, FALSE);

        // Increment the fence value for the current frame.
        fenceValues_[currentFrame_]++;
    }
} // namespace Takoyaki