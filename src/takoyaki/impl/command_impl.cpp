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
#include "command_impl.h"

#include "renderer_impl.h"

namespace Takoyaki
{
    CommandDesc::CommandDesc()
        : priority{ 0 }
    {
        commands.reserve(16);
    }

    CommandImpl::CommandImpl(const std::shared_ptr<RendererImpl>& renderer) noexcept
        : renderer_{ renderer }
    {
    }

    void CommandImpl::clearRenderTarget(const glm::vec4& color)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::CLEAR_COLOR, color));
    }

    void CommandImpl::drawIndexedInstanced()
    {
        auto renderer = renderer_.lock();

        renderer->buildCommand(desc_);
    }

    void CommandImpl::setDefaultRenderTarget()
    {
        desc_.commands.push_back(std::make_pair(ECommandType::RENDERTARGET_DEFAULT, boost::any()));
    }

    void CommandImpl::setPipelineState(const std::string& name)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::PIPELINE_STATE, name));
    }

    void CommandImpl::setRootSignature(const std::string& name)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::ROOT_SIGNATURE, name)); 
    }

    void CommandImpl::setRootSignatureConstantBuffer(uint_fast32_t index, const std::string& name)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::ROOT_SIGNATURE_CONSTANT_BUFFER, std::make_pair(index, name)));
    }

    void CommandImpl::setScissor(const glm::uvec4& scissor)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SCISSOR, scissor));
    }

    void CommandImpl::setViewport(const glm::vec4& viewport)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::VIEWPORT, viewport));
    }
}
// namespace Takoyaki