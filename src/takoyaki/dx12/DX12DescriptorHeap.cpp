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
#include "DX12DescriptorHeap.h"

#include <numeric>

namespace Takoyaki
{
    // Instance template here and use them elsewhere as extern
    template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;

    // Specializations
    template <>
    boost::wformat DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>::getFormatString()
    {
        return boost::wformat{ L"Render Target View Heap %1%" };
    }

    template <>
    D3D12_DESCRIPTOR_HEAP_FLAGS DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>::getFlags() const
    {
        return D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    }
} // namespace Takoyaki