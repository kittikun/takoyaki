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
#include <memory>

namespace Takoyaki
{
    class DX12Device;

    // Vulkan probably have something similar, so use an internal type to map to dx types
    enum class EDescriptorHeapType
    {
        SHADER_RESOURCE,
        SAMPLER,
        RENDERTARGETVIEW,
        DEPTHSTENCILVIEW
    };

    // Heap descriptor with a pool of MAX_DESCRIPTORS
    // BENCHMARK: Is it better to create a pool here or just one per resource, use small pool for now
    // https://msdn.microsoft.com/en-us/library/windows/desktop/Dn899211(v=VS.85).aspx
    // TODO: How about reuse and fragmentation ? Don't allow reuse for now but might be work investigating
    class DescriptorHeap
    {
        DescriptorHeap(const DescriptorHeap&) = delete;
        DescriptorHeap& operator=(const DescriptorHeap&) = delete;
        DescriptorHeap(DescriptorHeap&&) = delete;
        DescriptorHeap& operator=(DescriptorHeap&&) = delete;

    public:
        DescriptorHeap(EDescriptorHeapType, std::weak_ptr<DX12Device>, uint_fast32_t);
        ~DescriptorHeap();
        D3D12_CPU_DESCRIPTOR_HANDLE Create();

        // TODO: Implement DX12Device notification for more heap creation

    private:
        // instead of having the device checking each time if a pool is maxed
        // have the pool notify the device it needs to create a new pool
        std::weak_ptr<DX12Device> device_;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
        D3D12_CPU_DESCRIPTOR_HANDLE handle_;
        uint_fast32_t descriptorSize_;
        uint_fast32_t count_;
    };
} // namespace Takoyaki