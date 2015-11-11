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

#include "pch.h"
#include "dx12_texture.h"

#include <intsafe.h>

#include "context.h"
#include "../public/definitions.h"

namespace Takoyaki
{
    DX12Texture::DX12Texture(DX12Context* owner) noexcept
        : owner_{ owner }
        , initialState_{ D3D12_RESOURCE_STATE_PRESENT }
    {
        // for swap chain creation
        cpuHandle_.ptr = ULONG_PTR_MAX;
    }

    DX12Texture::DX12Texture(DX12Context* owner, const TextureDesc& desc, D3D12_RESOURCE_STATES initialState) noexcept
        : owner_{ owner }
        , intermediate_{ std::make_unique<Intermediate>() }
        , initialState_{ initialState }
    {
        cpuHandle_.ptr = ULONG_PTR_MAX;
        intermediate_->desc = desc;
    }

    DX12Texture::DX12Texture(DX12Texture&& other) noexcept
        : owner_{ other.owner_ }
        , intermediate_{ std::move(other.intermediate_) }
        , resource_{ std::move(other.resource_) }
        , cpuHandle_{ std::move(other.cpuHandle_) }
        , initialState_{ other.initialState_ }
    {
        other.cpuHandle_.ptr = ULONG_PTR_MAX;
    }

    DX12Texture::~DX12Texture()
    {
        if (cpuHandle_.ptr != ULONG_PTR_MAX) {
            owner_->getRTVDescHeapCollection().releaseOne(cpuHandle_);
        }
    }

    void DX12Texture::create(DX12Device* device)
    {
        D3D12_HEAP_PROPERTIES prop;
        const TextureDesc& texDesc = intermediate_->desc;

        // default values let the drivers decided what is best
        D3D12_CPU_PAGE_PROPERTY cpu = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        if (texDesc.usage == EUsageType::CPU_READ) {
            //cpu = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
            //layout = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
        }

        prop.Type = UsageTypeToDX(texDesc.usage);
        prop.CPUPageProperty = cpu;
        prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        // no multi-gpu support for now
        prop.CreationNodeMask = 1;
        prop.VisibleNodeMask = 1;

        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903813(v=vs.85).aspx
        D3D12_RESOURCE_DESC desc;

        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Flags = ResourceFlagsToDX(texDesc.flags);
        desc.Format = FormatToDX(texDesc.format);
        desc.MipLevels = texDesc.mipmaps;
        desc.Layout = layout;

        // no MSAA for now
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        if (texDesc.height == 1) {
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            desc.DepthOrArraySize = texDesc.arraySize;
            desc.Height = 1;
            desc.Width = texDesc.width;
        } else if (texDesc.depth > 1) {
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            desc.DepthOrArraySize = texDesc.depth;
            desc.Height = texDesc.height;
            desc.Width = texDesc.width;
        } else {
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            desc.DepthOrArraySize = texDesc.arraySize;
            desc.Height = texDesc.height;
            desc.Width = texDesc.width;
        }

        // multi-thread safe, no need to lock
        device->getDXDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, initialState_, nullptr, IID_PPV_ARGS(&resource_));

        intermediate_.reset();
    }

    bool DX12Texture::destroy(void* command, void*)
    {
        auto cmd = static_cast<TaskCommand*>(command);

        cmd->commands->DiscardResource(resource_.Get(), nullptr);
        DXCheckThrow(cmd->commands->Close());

        return true;
    }

    const D3D12_CPU_DESCRIPTOR_HANDLE& DX12Texture::getRenderTargetView()
    {
        if (cpuHandle_.ptr == ULONG_PTR_MAX) {
            auto tuple = owner_->getRTVDescHeapCollection().createOne();

            cpuHandle_ = std::get<0>(tuple);

            ID3D12Device* pDevice;

            resource_->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
            pDevice->CreateRenderTargetView(resource_.Get(), nullptr, cpuHandle_);
        }

        return cpuHandle_;
    }

    uint_fast64_t DX12Texture::getSizeByte() const
    {
        auto desc = resource_->GetDesc();
        uint_fast64_t size = 0;
        ID3D12Device* pDevice;

        resource_->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
        pDevice->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &size);
        pDevice->Release();

        return size;
    }

    void DX12Texture::read(uint8_t* dest, uint_fast32_t size) const
    {
        auto desc = resource_->GetDesc();
        uint_fast64_t rowSize = 0;
        ID3D12Device* pDevice;

        resource_->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
        pDevice->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, &rowSize, nullptr);
        pDevice->Release();

        resource_->ReadFromSubresource(dest, (uint_fast32_t)rowSize, size, 0, nullptr);
    }
} // namespace Takoyaki