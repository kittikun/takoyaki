// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub license, and / or sell
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

namespace Takoyaki
{
    enum class EBufferType
    {
        // Best bandwidth for GPU but no CPU access
        NO_CPU_GPU_FAST,

        // best for CPU-write-once, good GPU performance
        // later CPU access is possible but not efficient 
        CPU_SLOW_GPU_GOOD,

        // slower GPU access but good CPU access
        // best for GPU-write-once
        CPU_FAST_GPU_SLOW
    };

    class DX12Device;

    class DX12Buffer
    {
        DX12Buffer(const DX12Buffer&) = delete;
        DX12Buffer& operator=(const DX12Buffer&) = delete;
        DX12Buffer& operator=(DX12Buffer&&) = delete;
        DX12Buffer(DX12Buffer&&) = delete;

    public:
        DX12Buffer(EBufferType, uint_fast64_t, D3D12_RESOURCE_STATES);
        
        ~DX12Buffer();

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:
        void Create(const std::shared_ptr<DX12Device>&);
        ID3D12Resource* getResource() { return resource_.Get(); }

        //////////////////////////////////////////////////////////////////////////
        // Internal & External

    private:
        D3D12_HEAP_TYPE bufferTypeToDX(EBufferType);

    private:
        struct Intermediate
        {
            EBufferType type;
            uint_fast64_t size;
            D3D12_RESOURCE_STATES initialState;
        };

        std::unique_ptr<Intermediate> intermediate_;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    };
} // namespace Takoyaki