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

#include <memory>

#include "definitions.h"

namespace Takoyaki
{
    class CommandImpl;

    class Command
    {
        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;
        Command(Command&&) = delete;
        Command& operator=(Command&&) = delete;

    public:
        Command(std::unique_ptr<CommandImpl>) noexcept;
        ~Command() noexcept;

        void setPriority(uint_fast32_t priority);

        // copy
        void copyRenderTargetToTexture(uint_fast32_t destTex);

        // draw commands
        void drawIndexed(uint_fast32_t indexCount, uint_fast32_t startIndex, int_fast32_t baseVertex);

        // geometry
        void setTopology(ETopology topology);
        void setIndexBuffer(uint_fast32_t handle);
        void setVertexBuffer(uint_fast32_t handle);

        // root signature
        void setRootSignature(const std::string& name);
        void setRootSignatureConstantBuffer(uint_fast32_t index, const std::string& name);

        // viewport
        void setScissor(const glm::uvec4& scissor);
        void setViewport(const glm::vec4& viewport);

        // render target related
        void setRenderTarget(uint_fast32_t handle);
        void clearRenderTarget(const glm::vec4& color);

    private:
        std::unique_ptr<CommandImpl> impl_;
    };
}
// namespace Takoyaki
