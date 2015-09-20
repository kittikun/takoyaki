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
#include "renderer_impl.h"

#include "constant_table_impl.h"
#include "../dx12/device_context.h"

namespace Takoyaki
{
    ConstantTableImpl::ConstantTableImpl(DX12ConstantBuffer& cbuffer, boost::shared_lock<boost::shared_mutex> lock)
        : cbuffer_(cbuffer)
        , bufferLock_(std::move(lock))
    {

    }

    ConstantTableImpl::~ConstantTableImpl() = default;

    void ConstantTableImpl::setMatrix4x4(const std::string& name, const glm::mat4x4& value)
    {
        cbuffer_.setMatrix4x4(name, value);
    }
}
// namespace Takoyaki