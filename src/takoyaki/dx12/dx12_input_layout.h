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
    constexpr size_t MAX_INPUT_SEMANTICS = 8;

    class DX12InputLayout
    {
        DX12InputLayout(const DX12InputLayout&) = delete;
        DX12InputLayout& operator=(const DX12InputLayout&) = delete;
        DX12InputLayout& operator=(DX12InputLayout&&) = delete;

    public:
        DX12InputLayout() = default;
        DX12InputLayout(DX12InputLayout&&);
        ~DX12InputLayout() = default;

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:
        D3D12_INPUT_LAYOUT_DESC getInputLayout();

        //////////////////////////////////////////////////////////////////////////
        // Internal & External
        void addInput(const std::string&, EFormat, uint_fast32_t, uint_fast32_t);

    private:
        std::vector<std::string> names_; // we need to keep copies of semantic names for LPCSTR
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputs_;
    };
} // namespace Takoyaki