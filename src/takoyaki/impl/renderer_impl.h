// Copyright(c) 2015 Kitti Vongsay
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

#include <atomic>

#include "../public/definitions.h"

namespace Takoyaki
{
    class ConstantTableImpl;
    class IndexBufferImpl;
    class InputLayoutImpl;
    class DX12Context;
    class DX12Device;
    class RootSignatureImpl;
    class VertexBufferImpl;
    struct PipelineStateDesc;

    class RendererImpl
    {
        RendererImpl(const RendererImpl&) = delete;
        RendererImpl& operator=(const RendererImpl&) = delete;
        RendererImpl(RendererImpl&&) = delete;
        RendererImpl& operator=(RendererImpl&&) = delete;

    public:
        RendererImpl(const std::shared_ptr<DX12Device>&, const std::shared_ptr<DX12Context>&) noexcept;
        ~RendererImpl() = default;

        //////////////////////////////////////////////////////////////////////////
        // External usage: 

        std::unique_ptr<IndexBufferImpl> createIndexBuffer(uint8_t*, EFormat, uint_fast32_t);
        std::unique_ptr<InputLayoutImpl> createInputLayout(const std::string&);
        std::unique_ptr<RootSignatureImpl> createRootSignature(const std::string&);
        std::unique_ptr<VertexBufferImpl> createVertexBuffer(uint8_t*, uint_fast32_t, uint_fast32_t);

        void createPipelineState(const std::string&, const PipelineStateDesc&);

        void compilePipelineStateObjects();

        std::unique_ptr<ConstantTableImpl> getConstantBuffer(const std::string&);

    private:
        std::shared_ptr<DX12Context> context_;
        std::shared_ptr<DX12Device> device_;

        // the "generator" is mostly for user resources that need to be
        // released upon destruction
        std::atomic<uint_fast32_t> uidGenerator_;
    };
}
// namespace Takoyaki
