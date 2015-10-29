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
#include <boost/any.hpp>

#include "../dx12/dxcommon.h"
#include "../public/definitions.h"

namespace Takoyaki
{
    class DX12Context;
    class DX12VertexBuffer;
    class RendererImpl;

    enum class ECommandType
    {
        CLEAR_COLOR,
        DRAW_INDEXED,
        INDEX_BUFFER,
        PIPELINE_STATE,
        RENDERTARGET_DEFAULT,
        ROOT_SIGNATURE,
        ROOT_SIGNATURE_CONSTANT_BUFFER,
        PRIMITIVE_TOPOLOGY,
        SCISSOR,
        VERTEX_BUFFER,
        VIEWPORT
    };

    struct CommandDesc
    {
        using RSCBParams = std::pair<uint_fast32_t, std::string>;
        using DrawIndexedParams = std::tuple<uint_fast32_t, uint_fast32_t, int_fast32_t>;

        CommandDesc();
        uint_fast32_t priority;
        std::vector<std::pair<ECommandType, boost::any>> commands;
    };

    class CommandImpl
    {
        CommandImpl(const CommandImpl&) = delete;
        CommandImpl& operator=(const CommandImpl&) = delete;
        CommandImpl(CommandImpl&&) = delete;
        CommandImpl& operator=(CommandImpl&&) = delete;

    public:
        CommandImpl(const std::shared_ptr<RendererImpl>&) noexcept;
        ~CommandImpl() = default;

        void clearRenderTarget(const glm::vec4&);
        void drawIndexed(uint_fast32_t, uint_fast32_t, int_fast32_t);
        void setDefaultRenderTarget();
        void setIndexBuffer(uint_fast32_t);
        void setPipelineState(const std::string&);
        inline void setPriority(uint_fast32_t priority) { desc_.priority = priority; }
        void setRootSignature(const std::string&);
        void setRootSignatureConstantBuffer(uint_fast32_t, const std::string&);
        void setScissor(const glm::uvec4&);
        void setTopology(ETopology);
        void setVertexBuffer(uint_fast32_t);
        void setViewport(const glm::vec4&);

    private:
        std::weak_ptr<RendererImpl> renderer_;
        CommandDesc desc_;
    };
}
// namespace Takoyaki
