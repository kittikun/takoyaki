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

#include "pch.h"
#include "dx12_buffer.h"

#include "device.h"

namespace Takoyaki
{
    DX12Buffer::DX12Buffer(EBufferType type, uint_fast64_t sizeByte, D3D12_RESOURCE_STATES initialState) noexcept
        : intermediate_{ std::make_unique<Intermediate>() }
    {
        intermediate_->type = type;
        intermediate_->size = sizeByte;
        intermediate_->initialState = initialState;
    }

    void DX12Buffer::Create(const std::shared_ptr<DX12Device>& device)
    {
        D3D12_HEAP_PROPERTIES prop;

        prop.Type = bufferTypeToDX(intermediate_->type);

        // let the driver about those properties
        prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        // no multi-gpu support for now
        prop.CreationNodeMask = 1;
        prop.VisibleNodeMask = 1;

        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903813(v=vs.85).aspx
        D3D12_RESOURCE_DESC desc;

        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.DepthOrArraySize = 1;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Height = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        if (intermediate_->size == 0)
            throw new std::runtime_error{ "Buffer size must at least 1" };

        desc.Width = intermediate_->size;

        // multi-thread safe, no need to lock
        device->getDXDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, intermediate_->initialState, nullptr, IID_PPV_ARGS(&resource_));

        intermediate_.reset();
    }

    D3D12_HEAP_TYPE bufferTypeToDX(EBufferType type)
    {
        if (type == EBufferType::CPU_FAST_GPU_SLOW)
            return D3D12_HEAP_TYPE_READBACK;
        else if (type == EBufferType::CPU_SLOW_GPU_GOOD)
            return D3D12_HEAP_TYPE_UPLOAD;

        return D3D12_HEAP_TYPE_DEFAULT;
    }

} // namespace Takoyaki
