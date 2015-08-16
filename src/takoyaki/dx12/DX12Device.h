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

#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include "../IDevice.h"

namespace Takoyaki
{
    class DX12Device final : public IDevice
    {
        DX12Device(const DX12Device&) = delete;
        DX12Device& operator=(const DX12Device&) = delete;
        DX12Device(DX12Device&&) = delete;
        DX12Device& operator=(DX12Device&&) = delete;

    public:
        DX12Device();
        ~DX12Device() override = default;

        void create(uint_fast32_t) override;

        //Wait for pending GPU work to complete.
        void waitForGpu();

        size_t getBufferCount() { return commandAllocators_.size(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> D3DDevice_;
        Microsoft::WRL::ComPtr<IDXGIFactory4> DXGIFactory_;

        // TODO: Move to another thread class
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>  commandQueue_;
        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators_;

        // CPU/GPU Synchronization.
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        std::vector<uint_fast32_t> fenceValues_;
        HANDLE fenceEvent_;

        uint_fast32_t currentFrame_;
    };
} // namespace Takoyaki