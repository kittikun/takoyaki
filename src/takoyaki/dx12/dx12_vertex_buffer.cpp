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
#include "dx12_vertex_buffer.h"

#include "dx12_buffer.h"

namespace Takoyaki
{
    DX12VertexBuffer::DX12VertexBuffer(void* vertices, uint_fast64_t byteSizeVectices, void* indices, uint_fast64_t byteSizeIndices)
        : uploadVertexBuffer_{ std::make_unique<DX12Buffer>(EBufferType::CPU_SLOW_GPU_GOOD, byteSizeVectices, D3D12_RESOURCE_STATE_GENERIC_READ) }
        , vertexBuffer_{std::make_unique<DX12Buffer>(EBufferType::NO_CPU_GPU_FAST, byteSizeVectices, D3D12_RESOURCE_STATE_COPY_DEST)}
    {
        
    }

    DX12VertexBuffer::~DX12VertexBuffer() = default;


} // namespace Takoyaki
