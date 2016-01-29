// Copyright(c) 2015-2016 Kitti Vongsay
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
        //COPY_RENDERTARGET,
        COPY_REGION_TEXTURE2D,
        DRAW_INDEXED,
        SET_INDEX_BUFFER,
        SET_ROOT_SIGNATURE,
        SET_ROOT_SIGNATURE_CONSTANT_BUFFER,
        SET_PRIMITIVE_TOPOLOGY,
        SET_SCISSOR,
        SET_VERTEX_BUFFER,
        SET_VIEWPORT
    };

    struct CommandDesc
    {
        // index, name
        using RSCBParams = std::pair<uint_fast32_t, std::string>;

        // indexCount, startIndex, baseVertex
        using DrawIndexedParams = std::tuple<uint_fast32_t, uint_fast32_t, int_fast32_t>;

        CommandDesc();
        uint_fast32_t priority;
        uint_fast32_t renderTarget;
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
        CommandImpl(const std::shared_ptr<RendererImpl>&, const std::string&) noexcept;
        ~CommandImpl();

        void clearRenderTarget(const glm::vec4&);
        //void copyRenderTargetToTexture(uint_fast32_t);
        void copyTextureRegion(const CopyTexRegionParams&);
        void drawIndexed(uint_fast32_t, uint_fast32_t, int_fast32_t);
        void setIndexBuffer(uint_fast32_t);
        inline void setPriority(uint_fast32_t priority) { desc_.priority = priority; }
        void setRenderTarget(uint_fast32_t);
        void setRootSignature(const std::string&);
        void setRootSignatureConstantBuffer(uint_fast32_t, const std::string&);
        void setScissor(const glm::uvec4&);
        void setTopology(ETopology);
        void setVertexBuffer(uint_fast32_t);
        void setViewport(const glm::vec4&);

    private:
        std::weak_ptr<RendererImpl> renderer_;
        std::string pipelineState_;
        CommandDesc desc_;
    };
}
// namespace Takoyaki
