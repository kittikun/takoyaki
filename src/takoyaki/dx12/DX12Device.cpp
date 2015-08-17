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

#include "utility.h"
#include "../utility/log.h"

namespace Takoyaki
{
    DX12Device::DX12Device()
        : window_(nullptr)
        , bufferCount_(0)
        , currentFrame_(0)
        , rtvDescriptorSize_(0)
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
        renderTargets_.resize(bufferCount_);

        createDevice(bufferCount_);
        createSwapChain();
    }

    void DX12Device::createDevice(uint_fast32_t bufferCount)
    {
        LOGC << "Creating D3D Device...";

#if defined(_DEBUG)
        // If the project is in a debug build, enable debugging via SDK Layers.
        {
            Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
            }
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
    }

    void DX12Device::createSwapChain()
    {
        LOGC << "Creating swap chain...";

        waitForGpu();

        // clear old render targets
        for (int i = 0; i < renderTargets_.size(); ++i)
            renderTargets_[i] = nullptr;

        rtvHeap_ = nullptr;

        // Calculate the necessary render target size in pixels.
        glm::vec2 outSize;

        outSize.x = ConvertDipsToPixels(windowSize_.x, dpi_);
        outSize.y = ConvertDipsToPixels(windowSize_.y, dpi_);

        // The width and height of the swap chain must be based on the window's
        // natively-oriented width and height. If the window is not in the native
        // orientation, the dimensions must be reversed.
        DXGI_MODE_ROTATION displayRotation = GetDXGIOrientation();

        if (displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270)
            std::swap(outSize.x, outSize.y);

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
                throw std::runtime_error("displayRotation");
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

        // Create a render target view of the swap chain back buffer.
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = bufferCount_;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            DXCheckThrow(D3DDevice_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap_)));
            rtvHeap_->SetName(L"Render Target View Descriptor Heap");

            // All pending GPU work was already finished. Update the tracked fence values
            // to the last value signaled.
            for (int i = 0; i < bufferCount_; i++) {
                fenceValues_[i] = fenceValues_[currentFrame_];
            }

            currentFrame_ = 0;

        }
    }

    DXGI_MODE_ROTATION DX12Device::GetDXGIOrientation() const
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        // Note: NativeOrientation can only be Landscape or Portrait even though
        // the DisplayOrientations enum has other values.
        switch (nativeOrientation_) {
            case DisplayOrientation::LANDSCAPE:
                switch (currentOrientation_) {
                    case DisplayOrientation::LANDSCAPE:
                        rotation = DXGI_MODE_ROTATION_IDENTITY;
                        break;

                    case DisplayOrientation::PORTRAIT:
                        rotation = DXGI_MODE_ROTATION_ROTATE270;
                        break;

                    case DisplayOrientation::LANDSCAPE_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE180;
                        break;

                    case DisplayOrientation::PORTRAIT_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE90;
                        break;

                    default:
                        throw std::runtime_error("nativeOrientation_");
                }
                break;

            case DisplayOrientation::PORTRAIT:
                switch (currentOrientation_) {
                    case DisplayOrientation::LANDSCAPE:
                        rotation = DXGI_MODE_ROTATION_ROTATE90;
                        break;

                    case DisplayOrientation::PORTRAIT:
                        rotation = DXGI_MODE_ROTATION_IDENTITY;
                        break;

                    case DisplayOrientation::LANDSCAPE_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE270;
                        break;

                    case DisplayOrientation::PORTRAIT_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE180;
                        break;

                    default:
                        throw std::runtime_error("currentOrientation_");
                }
                break;
        }
        return rotation;
    }

    void DX12Device::setProperty(PropertyID id, const boost::any& value)
    {
        switch (id) {
            case Takoyaki::PropertyID::WINDOW_SIZE:
                windowSize_ = boost::any_cast<glm::vec2>(value);
                break;
            case Takoyaki::PropertyID::WINDOW_ORIENTATION:
                currentOrientation_ = boost::any_cast<DisplayOrientation>(value);
                break;
            case Takoyaki::PropertyID::WINDOW_DPI:
                dpi_ = boost::any_cast<float>(value);
                break;
            default:
                LOGE << "setProperty unknown property";
                throw new std::runtime_error("setProperty unknown property");
                break;
        }

        createSwapChain();
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