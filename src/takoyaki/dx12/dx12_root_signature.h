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
    class DX12DescriptorRanges;

    class DX12RootSignature
    {
        DX12RootSignature(const DX12RootSignature&) = delete;
        DX12RootSignature& operator=(const DX12RootSignature&) = delete;
        DX12RootSignature& operator=(DX12RootSignature&&) = delete;

    public:
        DX12RootSignature() noexcept;
        DX12RootSignature(DX12RootSignature&&) noexcept;
        ~DX12RootSignature();

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        bool create(DX12Device*);
        inline ID3D12RootSignature* getRootSignature() { return rootSignature_.Get(); }

        //////////////////////////////////////////////////////////////////////////
        // Internal & External

        void addConstant(uint_fast32_t, uint_fast32_t);
        void addDescriptorConstantBuffer(uint_fast32_t);
        void addDescriptorUnorderedAccess(uint_fast32_t);
        void addDescriptorShaderResource(uint_fast32_t);
        uint_fast32_t addDescriptorTable();

        // only when using descriptor tables
        void addDescriptorRange(uint_fast32_t, EDescriptorType, uint_fast32_t, uint_fast32_t);

        inline void setFlags(uint_fast32_t flags) { intermediate_->flags = static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(flags); }

    private:
        struct Intermediate
        {
            Intermediate() noexcept;
            std::vector<D3D12_ROOT_PARAMETER> params;
            std::vector<DX12DescriptorRanges> ranges;
            D3D12_ROOT_SIGNATURE_FLAGS flags;

            // The maximum size of a root signature is 64 DWORDs.
            // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899209(v=vs.85).aspx
            uint_fast32_t size;
        };

        Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature_;
        std::unique_ptr<Intermediate> intermediate_;
    };
} // namespace Takoyaki
