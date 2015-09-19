// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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
#include "DX12InputLayout.h"

#include "DXUtility.h"

namespace Takoyaki
{
    DX12InputLayout::DX12InputLayout() = default;

    DX12InputLayout::DX12InputLayout(DX12InputLayout&& other)
        : inputs_{ std::move(other.inputs_) }
    {

    }

    DX12InputLayout::~DX12InputLayout() = default;

    void DX12InputLayout::addInput(const std::string& name, EFormat format, uint_fast32_t instanceStep)
    {
        D3D12_INPUT_ELEMENT_DESC desc = {};

        // Semantics names need to conform
        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb509647(v=vs.85).aspx
        desc.SemanticName = name.c_str();

        // For example POSITION[n], ignore for now
        desc.SemanticIndex = 0;

        // For MRT, ignore for now
        desc.InputSlot = 0;
        desc.AlignedByteOffset = 0;

        desc.Format = FormatToDXGIFormat(format);

        if (instanceStep > 0) {
            desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
            desc.InstanceDataStepRate = instanceStep;
        } else {
            desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            desc.InstanceDataStepRate = 0;
        }
    }
} // namespace Takoyaki
