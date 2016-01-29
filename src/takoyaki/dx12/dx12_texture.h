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

#include "../public/definitions.h"

namespace Takoyaki
{
    class DX12Device;
    class DX12Context;

    class DX12Texture
    {
        DX12Texture(const DX12Texture&) = delete;
        DX12Texture& operator=(const DX12Texture&) = delete;
        DX12Texture& operator=(DX12Texture&&) = delete;

    public:
        DX12Texture(DX12Context*) noexcept; // for swap chain creation
        DX12Texture(DX12Context*, const TextureDesc&, D3D12_RESOURCE_STATES) noexcept;
        DX12Texture(DX12Texture&&) noexcept;
        ~DX12Texture();

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        void create(DX12Device*);
        bool destroy(void*, void*);

        inline bool isReady() const { return resource_.Get() != nullptr; }
        inline Microsoft::WRL::ComPtr<ID3D12Resource>& getCOM() { return resource_; } // for swap chain creation
        inline D3D12_RESOURCE_STATES getInitialState() const { return initialState_; }
        inline ID3D12Resource* getResource() { return resource_.Get(); }

        //////////////////////////////////////////////////////////////////////////
        // Internal & External:

        const D3D12_CPU_DESCRIPTOR_HANDLE& getRenderTargetView();

        //////////////////////////////////////////////////////////////////////////
        // External:
        void read(uint8_t*, uint_fast32_t size) const;

        uint_fast64_t getSizeByte() const;

    private:
        struct Intermediate
        {
            TextureDesc desc;
        };

        DX12Context* owner_;
        std::unique_ptr<Intermediate> intermediate_;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
        D3D12_RESOURCE_STATES initialState_;
    };
} // namespace Takoyaki