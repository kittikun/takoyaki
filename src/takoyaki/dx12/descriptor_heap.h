// Copyright(c) 2015 Kitti Vongsay
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

#include "device.h"
#include "dxutility.h"

namespace Takoyaki
{
    class DX12Device;

    constexpr size_t MAX_DESCRIPTOR_HEAP_SIZE = 128;

    struct DX12DescriptorHeap
    {
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
        std::vector<uint_fast32_t> freelist;
    };

    // Heap descriptor with a pool of MAX_DESCRIPTORS
    // BENCHMARK: Is it better to create a pool here or just one per resource, use small pool for now
    // https://msdn.microsoft.com/en-us/library/windows/desktop/Dn899211(v=VS.85).aspx
    // Thread-safe when used via DeviceContext

    template <D3D12_DESCRIPTOR_HEAP_TYPE T>
    class DX12DescriptorHeapCollection
    {
        DX12DescriptorHeapCollection(const DX12DescriptorHeapCollection&) = delete;
        DX12DescriptorHeapCollection& operator=(const DX12DescriptorHeapCollection&) = delete;
        DX12DescriptorHeapCollection(DX12DescriptorHeapCollection&&) = delete;
        DX12DescriptorHeapCollection& operator=(DX12DescriptorHeapCollection&&) = delete;

    public:
        using HandleTuple = std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE, DX12DescriptorHeap*>;

        DX12DescriptorHeapCollection(std::weak_ptr<DX12Device> device) noexcept
            : device_{ device }
            , descriptorSize_{ UINT_FAST32_MAX }
        {
        }

        ~DX12DescriptorHeapCollection() = default;

        std::vector<HandleTuple> createRange(uint_fast32_t count)
        {
            std::lock_guard<std::mutex> lock{ mutex_ };
            std::vector<HandleTuple> res;

            res.reserve(count);

            // use create one since descriptors might end on different heaps
            for (uint_fast32_t i = 0; i < count; ++i)
                res.push_back(createOneInternal());

            return res;
        }

        HandleTuple createOne()
        {
            std::lock_guard<std::mutex> lock{ mutex_ };

            return createOneInternal();
        }

        void releaseOne(D3D12_CPU_DESCRIPTOR_HANDLE handle)
        {
            std::lock_guard<std::mutex> lock{ mutex_ };
            auto container = containerMap_[handle.ptr];

            if (containerMap_.erase(handle.ptr) != 1)
                throw std::runtime_error{ "DX12DescriptorHeapCollection::releaseOne, containerMap_ nothing erased" };

            heaps_[container].freelist.push_back((uint_fast32_t)(handle.ptr - heaps_[container].cpuHandle.ptr) / descriptorSize_);
        }

        template<typename Iter>
        void releaseRange(Iter begin, Iter end)
        {
            std::lock_guard<std::mutex> lock{ mutex_ };

            for (Iter i = begin; i != end; ++i) {
                auto container = containerMap_[i->ptr];

                if (containerMap_.erase(i->ptr) != 1)
                    throw std::runtime_error{ "DX12DescriptorHeapCollection::releaseRange, containerMap_ item not erased" };

                heaps_[container].freelist.push_back((uint_fast32_t)(i->ptr - heaps_[container].cpuHandle.ptr) / descriptorSize_);
            }
        }

    private:
        HandleTuple createOneInternal()
        {
            D3D12_CPU_DESCRIPTOR_HANDLE cpu;
            D3D12_GPU_DESCRIPTOR_HANDLE gpu;
            bool found = false;
            size_t i = 0;

            while (!found) {
                for (i; i < heaps_.size(); ++i) {
                    auto& heap = heaps_[i];

                    if (heap.freelist.size() > 0) {

                        auto offset = heap.freelist.back();

                        heap.freelist.pop_back();
                        cpu.ptr = heap.cpuHandle.ptr + offset * descriptorSize_;
                        gpu.ptr = heap.gpuHandle.ptr + offset * descriptorSize_;
                        found = true;
                        break;
                    }
                }

                if (!found)
                    allocateHeap();
            }

            // we need to store which container it belonged to for later release.
            // using cpu for that should be ok since gpu is mirroring it
            containerMap_.insert({ cpu.ptr, i });

            return std::make_tuple(cpu, gpu, &heaps_[i]);
        }

        void allocateHeap()
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};

            desc.NumDescriptors = MAX_DESCRIPTOR_HEAP_SIZE;
            desc.Type = T;
            desc.Flags = getFlags();

            heaps_.resize(heaps_.size() + 1);

            auto& heap = heaps_[heaps_.size() - 1];

            // Not thread-safe so lock device
            {
                auto device = device_.lock();
                auto lock = device->getDeviceLock();

                if (descriptorSize_ == UINT_FAST32_MAX)
                    descriptorSize_ = device->getDXDevice()->GetDescriptorHandleIncrementSize(T);

                DXCheckThrow(device->getDXDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap.descriptor)));
            }

            // Make a suitable name
            heap.descriptor->SetName(boost::str(getFormatString() % heaps_.size()).c_str());
            heap.cpuHandle = heap.descriptor->GetCPUDescriptorHandleForHeapStart();
            heap.gpuHandle = heap.descriptor->GetGPUDescriptorHandleForHeapStart();
            heap.freelist.resize(MAX_DESCRIPTOR_HEAP_SIZE);
            std::iota(heap.freelist.begin(), heap.freelist.end(), 0);
        }

        boost::wformat getFormatString();
        D3D12_DESCRIPTOR_HEAP_FLAGS getFlags() const;

    private:
        std::mutex mutex_;
        std::weak_ptr<DX12Device> device_;
        std::vector<DX12DescriptorHeap> heaps_;
        std::unordered_map<SIZE_T, size_t> containerMap_;
        uint_fast32_t descriptorSize_;
    };

    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_DSV>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
} // namespace Takoyaki