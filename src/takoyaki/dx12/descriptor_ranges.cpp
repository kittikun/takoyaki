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
#include "descriptor_ranges.h"

namespace Takoyaki
{
    DX12DescriptorRanges::DX12DescriptorRanges(DX12DescriptorRanges&& other) noexcept
        : descs_(std::move(other.descs_))
    {

    }

    void DX12DescriptorRanges::add(D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint_fast32_t numDescriptors, uint_fast32_t baseShaderRegister)
    {
        D3D12_DESCRIPTOR_RANGE desc;

        desc.RangeType = rangeType;
        desc.NumDescriptors = numDescriptors;
        desc.BaseShaderRegister = baseShaderRegister;
        desc.RegisterSpace = 0;
        desc.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        descs_.push_back(std::move(desc));
    }
} // namespace Takoyaki
