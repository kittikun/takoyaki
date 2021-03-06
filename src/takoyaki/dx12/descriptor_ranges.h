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
    class DX12DescriptorRanges
    {
        DX12DescriptorRanges(const DX12DescriptorRanges&) = delete;
        DX12DescriptorRanges& operator=(const DX12DescriptorRanges&) = delete;
        DX12DescriptorRanges& operator=(DX12DescriptorRanges&&) = delete;

    public:
        DX12DescriptorRanges() = default;
        DX12DescriptorRanges(DX12DescriptorRanges&&) noexcept;
        ~DX12DescriptorRanges() = default;

        void add(D3D12_DESCRIPTOR_RANGE_TYPE, uint_fast32_t, uint_fast32_t);

        inline uint_fast32_t size() const { return static_cast<uint_fast32_t>(descs_.size()); }
        inline D3D12_DESCRIPTOR_RANGE* getDescs() { return &descs_.front(); }

    private:
        std::vector<D3D12_DESCRIPTOR_RANGE> descs_;
    };
} // namespace Takoyaki