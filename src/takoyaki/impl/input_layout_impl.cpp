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

#include "pch.h"
#include "input_layout_impl.h"

#include "../dx12/dx12_input_layout.h"

namespace Takoyaki
{
    InputLayoutImpl::InputLayoutImpl(DX12InputLayout& layout, std::shared_lock<std::shared_timed_mutex> lock) noexcept
        : layout_(layout)
        , bufferLock_(std::move(lock))
    {

    }

    void InputLayoutImpl::addInput(const std::string& name, EFormat format, uint_fast32_t byteOffset, uint_fast32_t instanceStep)
    {
        layout_.addInput(name, format, byteOffset, instanceStep);
    }
}
// namespace Takoyaki