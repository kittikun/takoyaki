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

#pragma once

namespace Takoyaki
{
    class DX12Buffer;
    class DX12Device;

    class DX12VertexBuffer
    {
        DX12VertexBuffer(const DX12VertexBuffer&) = delete;
        DX12VertexBuffer& operator=(const DX12VertexBuffer&) = delete;
        DX12VertexBuffer& operator=(DX12VertexBuffer&&) = delete;
        DX12VertexBuffer(DX12VertexBuffer&&);

    public:
        DX12VertexBuffer(uint8_t*, uint_fast64_t, uint8_t*, uint_fast64_t);
        
        ~DX12VertexBuffer();

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:
        void create(const std::shared_ptr<DX12Device>&);

        //////////////////////////////////////////////////////////////////////////
        // Internal & External

    private:
        struct Intermediate
        {
            D3D12_SUBRESOURCE_DATA vertexData;            
        };

        std::unique_ptr<DX12Buffer> uploadVertexBuffer_;
        std::unique_ptr<DX12Buffer> vertexBuffer_;
        std::unique_ptr<Intermediate> intermediate_;
    };
} // namespace Takoyaki