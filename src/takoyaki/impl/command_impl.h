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

#include "../dx12/dxcommon.h"

namespace Takoyaki
{
    class DX12Context;
    class DX12VertexBuffer;
    class RendererImpl;

    enum class ECommandType
    {
        ROOT_SIGNATURE
    };

    struct CommandDesc
    {
        CommandDesc();
        uint_fast32_t priority;
        std::vector<std::pair<ECommandType, std::string>> commands;
    };

    class CommandImpl
    {
        CommandImpl(const CommandImpl&) = delete;
        CommandImpl& operator=(const CommandImpl&) = delete;
        CommandImpl(CommandImpl&&) = delete;
        CommandImpl& operator=(CommandImpl&&) = delete;

    public:
        CommandImpl(const std::shared_ptr<RendererImpl>&) noexcept;
        ~CommandImpl() noexcept;

        void drawIndexedInstanced();

        inline void setPriority(uint_fast32_t priority) { desc_.priority = priority; }
        inline void setRootSignature(const std::string& name) { desc_.commands.push_back(std::make_pair(ECommandType::ROOT_SIGNATURE, name)); };

    private:
        std::weak_ptr<RendererImpl> renderer_;
        CommandDesc desc_;
    };
}
// namespace Takoyaki
