// Copyright(c) 2015-2016 Kitti Vongsay
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
    class DX12Buffer;
    class DX12Context;
    class DX12Device;
    struct DX12DescriptorHeap;

    class DX12ConstantBuffer
    {
        DX12ConstantBuffer(const DX12ConstantBuffer&) = delete;
        DX12ConstantBuffer& operator=(const DX12ConstantBuffer&) = delete;
        DX12ConstantBuffer& operator=(DX12ConstantBuffer&&) = delete;

    public:
        explicit DX12ConstantBuffer(DX12Context*, uint_fast32_t, uint_fast32_t);
        DX12ConstantBuffer(DX12ConstantBuffer&&) noexcept;
        ~DX12ConstantBuffer();

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        void addVariable(const std::string&, uint_fast32_t, uint_fast32_t);
        void create(const std::string&, DX12Device*);

        inline const D3D12_CPU_DESCRIPTOR_HANDLE& getCPUView(uint_fast32_t frame) { return cpuHandles_[frame]; }
        inline const D3D12_GPU_DESCRIPTOR_HANDLE& getGPUView(uint_fast32_t frame) { return gpuHandles_[frame]; }
        inline DX12DescriptorHeap* getHeap(uint_fast32_t frame) { return heaps_[frame]; }
        inline bool isReady() const { return ready_.load(); }

        //////////////////////////////////////////////////////////////////////////
        // External usage:

        void setMatrix4x4(const std::string&, const glm::mat4x4&, uint_fast32_t);

    private:
        struct CBVariable
        {
            uint_fast32_t offset;
            uint_fast32_t size;
        };

        DX12Context* owner_;
        std::unique_ptr<DX12Buffer> buffer_;
        std::unordered_map<std::string, CBVariable> offsetMap_;     // TODO: thread unsafe but should be ok
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuHandles_;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> gpuHandles_;
        std::vector<DX12DescriptorHeap*> heaps_;
        uint8_t* mappedAddr_;
        uint_fast32_t curOffset_;
        uint_fast32_t size_;
        std::atomic<bool> ready_;
    };
} // namespace Takoyaki