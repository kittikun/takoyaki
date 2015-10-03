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
#include "vertex_buffer_impl.h"

#include "../dx12/context.h"
#include "../dx12/dx12_vertex_buffer.h"

namespace Takoyaki
{
    VertexBufferImpl::VertexBufferImpl(const std::shared_ptr<DX12Context>& context, const DX12VertexBuffer& buffer, uint_fast32_t handle) noexcept
        : context_{ context }
        , buffer_{ buffer }
        , handle_{ handle }
    {

    }

    VertexBufferImpl::~VertexBufferImpl()
    {
        auto context = context_.lock();

        context->destroyResource(DX12Context::EResourceType::VERTEX_BUFFER, handle_);
    }
}
// namespace Takoyaki