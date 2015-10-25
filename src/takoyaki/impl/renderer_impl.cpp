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
#include "renderer_impl.h"

#include "constant_buffer_impl.h"
#include "index_buffer_impl.h"
#include "input_layout_impl.h"
#include "root_signature_impl.h"
#include "vertex_buffer_impl.h"
#include "../thread_pool.h"
#include "../dx12/context.h"
#include "../dx12/device.h"

namespace Takoyaki
{
    RendererImpl::RendererImpl(const std::shared_ptr<DX12Device>& device, const std::shared_ptr<DX12Context>& context, const std::shared_ptr<ThreadPool>& threadPool) noexcept
        : context_{ context }
        , device_{ device }
        , threadPool_{ threadPool }
    {

    }

    void RendererImpl::buildCommand(const CommandDesc& desc) const
    {
        // capture by value is important here
        auto lamda = [this, desc](void* cmd, void* dev)
        {
            auto taskCmd = static_cast<TaskCommand*>(cmd);

            context_->buildCommand(desc, taskCmd);
        };

        threadPool_->submitGPU(std::bind(lamda, std::placeholders::_1, std::placeholders::_2), 0);
    }

    void RendererImpl::compilePipelineStateObjects()
    {
        std::shared_lock<std::shared_timed_mutex> readLock{ rwMutex_ };

        context_->compilePipelineStateObjects();
    }

    std::unique_ptr<CommandImpl> RendererImpl::createCommand()
    {
        return std::make_unique<CommandImpl>(shared_from_this());
    }

    std::unique_ptr<IndexBufferImpl> RendererImpl::createIndexBuffer(uint8_t* data, EFormat format, uint_fast32_t sizeByte)
    {
        auto id = uidGenerator_.fetch_add(1);
        std::shared_lock<std::shared_timed_mutex> readLock{rwMutex_};

        context_->createBuffer(DX12Context::EResourceType::INDEX_BUFFER, id, data, format, 0, sizeByte);

        return std::make_unique<IndexBufferImpl>(context_, context_->getIndexBuffer(id), id);
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

    std::unique_ptr<VertexBufferImpl> RendererImpl::createVertexBuffer(uint8_t* data, uint_fast32_t stride, uint_fast32_t sizeByte)
    {
        auto id = uidGenerator_.fetch_add(1);
        std::shared_lock<std::shared_timed_mutex> readLock{ rwMutex_ };

        context_->createBuffer(DX12Context::EResourceType::VERTEX_BUFFER, id, data, EFormat::UNKNOWN, stride, sizeByte);

        return std::make_unique<VertexBufferImpl>(context_, context_->getVertexBuffer(id), id);
    }

    std::unique_ptr<ConstantBufferImpl> RendererImpl::getConstantBuffer(const std::string& name)
    {
        auto pair = context_->getConstantBuffer(name);

        // is it possible that the constant haven't been added yet if the corresponding shader
        // hasn't been compiled yet
        if (pair)
            return std::make_unique<ConstantBufferImpl>(pair->first, std::move(pair->second), device_->getCurrentFrame());
        else
            return nullptr;
    }
}
// namespace Takoyaki