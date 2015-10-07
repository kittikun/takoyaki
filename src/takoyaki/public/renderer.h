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

#include <memory>
#include <string>

#include "definitions.h"

namespace Takoyaki
{
    class ConstantTable;
    class IndexBuffer;
    class InputLayout;
    class RendererImpl;
    class RootSignature;
    class VertexBuffer;
    struct PipelineStateDesc;

    class Renderer
    {
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

    public:
        explicit Renderer(std::shared_ptr<RendererImpl>&) noexcept;
        ~Renderer() noexcept;

        std::unique_ptr<IndexBuffer> createIndexBuffer(uint8_t* indexes, EFormat format, uint_fast32_t sizeByte);
        std::unique_ptr<InputLayout> createInputLayout(const std::string& name);
        std::unique_ptr<RootSignature> createRootSignature(const std::string& name);
        std::unique_ptr<VertexBuffer> createVertexBuffer(uint8_t* vertices, uint_fast32_t stride, uint_fast32_t sizeByte);

        void createPipelineState(const std::string& name, const PipelineStateDesc&);

        // Compile pipeline state objects
        // Called once the root signatures and pipeline state objects have been defined
        // commit should happen only once per application
        void compilePipelineStateObjects();

        std::unique_ptr<ConstantTable> getConstantBuffer(const std::string&);

    private:
        std::shared_ptr<RendererImpl> impl_;
    };
}
// namespace Takoyaki
