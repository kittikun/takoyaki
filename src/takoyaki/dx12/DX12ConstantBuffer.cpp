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
#include "DX12ConstantBuffer.h"

#include <intsafe.h>

#include "DX12DeviceContext.h"

namespace Takoyaki
{
    DX12ConstantBuffer::DX12ConstantBuffer(std::weak_ptr<DX12DeviceContext> owner)
        : owner_{ owner }
    {
        rtv_ = owner_.lock()->getSRVDescHeapCollection().createOne();
    }

    DX12ConstantBuffer::DX12ConstantBuffer(DX12ConstantBuffer&& other) noexcept
        : owner_{ std::move(other.owner_) }
        , rtv_{ std::move(other.rtv_) }
    {

    }

    DX12ConstantBuffer::~DX12ConstantBuffer()
    {
        if ((rtv_.ptr != ULONG_PTR_MAX) && (!owner_.expired())) {
            owner_.lock()->getRTVDescHeapCollection().releaseOne(rtv_);
        }
    }

    void DX12ConstantBuffer::addVariable(const std::string& name, uint_fast32_t offset, uint_fast32_t size)
    {
        CBVariable var;

        var.offset = offset;
        var.size = size;

        offsetMap_.insert(std::make_pair(name, std::move(var)));
    }
} // namespace Takoyaki
