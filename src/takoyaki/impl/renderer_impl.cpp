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
#include "input_layout_impl.h"
#include "root_signature_impl.h"
#include "vertex_buffer_impl.h"
#include "../thread_pool.h"
#include "../dx12/context.h"

namespace Takoyaki
{
    RendererImpl::RendererImpl(const std::shared_ptr<DX12Context>& context) noexcept
        : context_{ context }
    {

    }

    void RendererImpl::commit()
    {
        context_->commit();
    }

    std::unique_ptr<InputLayoutImpl> RendererImpl::createInputLayout(const std::string& name)
    {
        context_->createInputLayout(name);

        auto pair = context_->getInputLayout(name);

        return std::make_unique<InputLayoutImpl>(pair.first, std::move(pair.second));
    }

    void RendererImpl::createPipelineState(const std::string& name, const PipelineStateDesc& desc)
    {
        context_->createPipelineState(name, desc);
    }

    std::unique_ptr<RootSignatureImpl> RendererImpl::createRootSignature(const std::string& name)
    {
        context_->createRootSignature(name);

        auto pair = context_->getRootSignature(name);

        return std::make_unique<RootSignatureImpl>(pair.first, std::move(pair.second));
    }

    std::unique_ptr<VertexBufferImpl> RendererImpl::createVertexBuffer(uint8_t* vertices, uint_fast64_t sizeVecticesByte)
    {
        auto id = uidGenerator_.fetch_add(1);

        context_->createVertexBuffer(id, vertices, sizeVecticesByte);

        return std::make_unique<VertexBufferImpl>(context_, context_->getVertexBuffer(id), id);
    }

    std::unique_ptr<ConstantTableImpl> RendererImpl::getConstantBuffer(const std::string& name)
    {
        auto pair = context_->getConstantBuffer(name);

        // is it possible that the constant haven't been added yet if the corresponding shader
        // hasn't been compiled yet
        if (pair)
            return std::make_unique<ConstantTableImpl>(pair->first, std::move(pair->second));
        else
            return nullptr;
    }
}
// namespace Takoyaki