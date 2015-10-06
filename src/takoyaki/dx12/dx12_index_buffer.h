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

#include "../public/definitions.h"

namespace Takoyaki
{
    class DX12Buffer;
    class DX12Device;
    class ThreadPool;

    // For simplicity, bundle vertex and index buffer here
    class DX12IndexBuffer
    {
        DX12IndexBuffer(const DX12IndexBuffer&) = delete;
        DX12IndexBuffer& operator=(const DX12IndexBuffer&) = delete;
        DX12IndexBuffer& operator=(DX12IndexBuffer&&) = delete;

    public:
        explicit DX12IndexBuffer(uint8_t*, EFormat, uint_fast32_t, uint_fast32_t) noexcept;
        DX12IndexBuffer(DX12IndexBuffer&&) noexcept;
        ~DX12IndexBuffer() = default;

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        // tasks
        void create(void*, void*);
        void createCleanup(void*, void*);
        void destroy(ID3D12GraphicsCommandList*);
        void onCreateDone();

        //////////////////////////////////////////////////////////////////////////
        // Internal & External    

    private:
        struct Intermediate
        {
            std::vector<uint8_t> data;
            D3D12_SUBRESOURCE_DATA dataDesc;
            uint_fast32_t id;
        };

        std::unique_ptr<DX12Buffer> indexBuffer_;
        std::unique_ptr<DX12Buffer> uploadBuffer_;
        std::unique_ptr<Intermediate> intermediate_;
        D3D12_INDEX_BUFFER_VIEW view_;
    };
} // namespace Takoyaki