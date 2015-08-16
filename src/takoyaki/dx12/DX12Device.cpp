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

#include "error.h"
#include "../utility/log.h"

namespace Takoyaki
{
    DX12Device::DX12Device()
        : currentFrame_(0)
    {
    }

    void DX12Device::CreateDevice()
    {
        LOGD << "Creating D3D Device";

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

        for (auto i = 0; i < commandAllocators_.size(); ++i) {
            DXCheckThrow(D3DDevice_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators_[i])));
        }

        // Create synchronization objects.
        DXCheckThrow(D3DDevice_->CreateFence(fenceValues_[currentFrame_], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
        fenceValues_[currentFrame_]++;
        fenceEvent_ = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    }

    void DX12Device::Initialize()
    {
        CreateDevice();
    }

    void DX12Device::WaitForGPU()
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