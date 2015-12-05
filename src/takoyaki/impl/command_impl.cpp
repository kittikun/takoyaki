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
        , renderTarget{ UINT_FAST32_MAX }
    {
        commands.reserve(16);
    }

    CommandImpl::CommandImpl(const std::shared_ptr<RendererImpl>& renderer, const std::string& pipelineState) noexcept
        : renderer_{ renderer }
        , pipelineState_{ pipelineState }
    {
    }

    CommandImpl::~CommandImpl()
    {
        auto renderer = renderer_.lock();

        renderer->buildCommand(desc_, pipelineState_);
    }

    void CommandImpl::clearRenderTarget(const glm::vec4& color)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::CLEAR_COLOR, color));
    }

    void CommandImpl::copyRenderTargetToTexture(uint_fast32_t destTex)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::COPY_RENDERTARGET, destTex));
    }

    void CommandImpl::copyRegion(uint_fast32_t destTex, const glm::i16vec2& dstTopLeft, uint_fast32_t srcTex, const glm::ivec4& srcRect)
    {
    }

    void CommandImpl::drawIndexed(uint_fast32_t indexCount, uint_fast32_t startIndex, int_fast32_t baseVertex)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::DRAW_INDEXED, std::make_tuple(indexCount, startIndex, baseVertex)));
    }

    void CommandImpl::setIndexBuffer(uint_fast32_t handle)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_INDEX_BUFFER, handle));
    }

    void CommandImpl::setRenderTarget(uint_fast32_t handle)
    {
        desc_.renderTarget = handle;
    }

    void CommandImpl::setRootSignature(const std::string& name)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_ROOT_SIGNATURE, name));
    }

    void CommandImpl::setRootSignatureConstantBuffer(uint_fast32_t index, const std::string& name)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_ROOT_SIGNATURE_CONSTANT_BUFFER, std::make_pair(index, name)));
    }

    void CommandImpl::setScissor(const glm::uvec4& scissor)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_SCISSOR, scissor));
    }

    void CommandImpl::setTopology(ETopology topology)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_PRIMITIVE_TOPOLOGY, topology));
    }

    void CommandImpl::setVertexBuffer(uint_fast32_t handle)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_VERTEX_BUFFER, handle));
    }

    void CommandImpl::setViewport(const glm::vec4& viewport)
    {
        desc_.commands.push_back(std::make_pair(ECommandType::SET_VIEWPORT, viewport));
    }
}
// namespace Takoyaki