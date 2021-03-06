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

namespace Takoyaki
{
    class DX12Buffer;
    class DX12Device;
    class ThreadPool;

    // For simplicity, bundle vertex and index buffer here
    class DX12VertexBuffer
    {
        DX12VertexBuffer(const DX12VertexBuffer&) = delete;
        DX12VertexBuffer& operator=(const DX12VertexBuffer&) = delete;
        DX12VertexBuffer& operator=(DX12VertexBuffer&&) = delete;

    public:
        explicit DX12VertexBuffer(uint8_t*, uint_fast32_t, uint_fast32_t, uint_fast32_t) noexcept;
        DX12VertexBuffer(DX12VertexBuffer&&) noexcept;
        ~DX12VertexBuffer() = default;

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        // tasks
        bool create(void*, void*);
        bool cleanupCreate(void*, void*);
        void cleanupIntermediate();
        bool destroy(void*, void*);

        //////////////////////////////////////////////////////////////////////////
        // Internal & External

        inline D3D12_VERTEX_BUFFER_VIEW getView() const { return view_; }

    private:
        struct Intermediate
        {
            uint_fast32_t id;
            std::vector<uint8_t> data;
            D3D12_SUBRESOURCE_DATA dataDesc;
        };

        std::unique_ptr<DX12Buffer> vertexBuffer_;
        std::unique_ptr<DX12Buffer> uploadBuffer_;
        std::unique_ptr<Intermediate> intermediate_;
        D3D12_VERTEX_BUFFER_VIEW view_;
    };
} // namespace Takoyaki