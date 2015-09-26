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
#include "renderer.h"

#include "constant_table.h"
#include "input_layout.h"
#include "root_signature.h"

#include "../impl/constant_table_impl.h"
#include "../impl/input_layout_impl.h"
#include "../impl/pipeline_state_impl.h"
#include "../impl/renderer_impl.h"
#include "../impl/root_signature_impl.h"

namespace Takoyaki
{
    Renderer::Renderer(std::shared_ptr<RendererImpl>& impl) noexcept
        : impl_{ impl }
    {
    }

    Renderer::~Renderer() = default;

    void Renderer::commit()
    {
        impl_->commit();
    }

    std::unique_ptr<InputLayout> Renderer::createInputLayout(const std::string& name)
    {
        return std::make_unique<InputLayout>(impl_->createInputLayout(name));
    }

    void Renderer::createPipelineState(const std::string& name, const PipelineStateDesc& desc)
    {
        impl_->createPipelineState(name, desc);
    }

    std::unique_ptr<RootSignature> Renderer::createRootSignature(const std::string& name)
    {
        return std::make_unique<RootSignature>(impl_->createRootSignature(name));
    }

    void Renderer::createVertexBuffer(uint8_t* vertices, uint_fast64_t sizeVecticesByte, uint8_t* indices, uint_fast64_t sizeIndicesByte)
    {
        impl_->createVertexBuffer(vertices, sizeVecticesByte, indices, sizeIndicesByte);
    }

    std::unique_ptr<ConstantTable> Renderer::getConstantBuffer(const std::string& name)
    {
        return std::make_unique<ConstantTable>(std::move(impl_->getConstantBuffer(name)));
    }
}
// namespace Takoyaki