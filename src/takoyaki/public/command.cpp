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

#include "pch.h"
#include "command.h"

#include "../impl/command_impl.h"

namespace Takoyaki
{
    Command::Command(std::unique_ptr<CommandImpl> impl) noexcept
        : impl_{ std::move(impl) }
    {
    }

    Command::~Command() = default;

    void Command::clearRenderTarget(const glm::vec4& color)
    {
        impl_->clearRenderTarget(color);
    }

    void Command::copyRenderTargetToTexture(uint_fast32_t destTex)
    {
        impl_->copyRenderTargetToTexture(destTex);
    }

    void Command::drawIndexed(uint_fast32_t indexCount, uint_fast32_t startIndex, int_fast32_t baseVertex)
    {
        impl_->drawIndexed(indexCount, startIndex, baseVertex);
    }

    void Command::setIndexBuffer(uint_fast32_t handle)
    {
        impl_->setIndexBuffer(handle);
    }

    void Command::setPriority(uint_fast32_t priority)
    {
        impl_->setPriority(priority);
    }

    void Command::setRenderTarget(uint_fast32_t handle)
    {
        impl_->setRenderTarget(handle);
    }

    void Command::setRootSignature(const std::string& name)
    {
        impl_->setRootSignature(name);
    }

    void Command::setRootSignatureConstantBuffer(uint_fast32_t index, const std::string& name)
    {
        impl_->setRootSignatureConstantBuffer(index, name);
    }

    void Command::setScissor(const glm::uvec4& scissor)
    {
        impl_->setScissor(scissor);
    }

    void Command::setTopology(ETopology topology)
    {
        impl_->setTopology(topology);
    }

    void Command::setVertexBuffer(uint_fast32_t handle)
    {
        impl_->setVertexBuffer(handle);
    }

    void Command::setViewport(const glm::vec4& viewport)
    {
        impl_->setViewport(viewport);
    }
}
// namespace Takoyaki