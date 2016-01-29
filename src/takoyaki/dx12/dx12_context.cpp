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

#include "pch.h"
#include "dx12_context.h"

#include <boost/format.hpp>

#include "dx12_worker.h"
#include "../impl/command_impl.h"
#include "../thread_pool.h"
#include "../utility/log.h"

namespace Takoyaki
{
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;

    DX12Context::DX12Context(const std::shared_ptr<DX12Device>& device, const std::shared_ptr<ThreadPool>& threadPool)
        : device_{ device }
        , threadPool_{ threadPool }
        , cmdBuilder_{ this , device_.get() }
        , descHeapRTV_{ device }
        , descHeapSRV_{ device }
    {
        // somehow cannot default construct or move RWLockMap, oh well..
        shaders_.reserve(6);
        shaders_[EShaderType::COMPUTE];
        shaders_[EShaderType::DOMAIN];
        shaders_[EShaderType::GEOMETRY];
        shaders_[EShaderType::HULL];
        shaders_[EShaderType::PIXEL];
        shaders_[EShaderType::VERTEX];
    }

    void DX12Context::addShader(EShaderType type, const std::string& name, D3D12_SHADER_BYTECODE&& bc)
    {
        auto& map = shaders_[type];
        auto lock = map.getWriteLock();

        map.insert(std::make_pair(name, std::move(bc)));
    }

    bool DX12Context::buildCommand(const CommandDesc& desc, TaskCommand* cmd)
    {
        return cmdBuilder_.buildCommand(desc, cmd);
    }

    void DX12Context::compilePipelineStateObjects()
    {
        auto device = device_->getDeviceLock();

        // create all root signatures
        {
            auto lock = rootSignatures_.getReadLock();

            for (auto& rs : rootSignatures_) {
                auto res = rs.second.create(device_.get());

                if (!res) {
                    auto fmt = boost::format{ "RootSignature contains no parameters: %1%" } % rs.first;

                    LOGW << boost::str(fmt);
                }
            }
        }

        // create all pipeline state
        {
            auto lock = pipelineStates_.getReadLock();
            auto threadPool = threadPool_.lock();

            for (auto& state : pipelineStates_)
                threadPool->submitGeneric(std::bind(&DX12Context::compileMain, this, state.first), 0);
        }
    }

    void DX12Context::compileMain(const std::string& name)
    {
        auto pair = getPipelineState(name);

        pair.first.create(device_.get(), this);
    }

    void DX12Context::createBuffer(EResourceType type, uint_fast32_t id, uint8_t* data, EFormat format, uint_fast32_t stride, uint_fast32_t sizeByte)
    {
        auto threadPool = threadPool_.lock();

        switch (type) {
            case Takoyaki::DX12Context::EResourceType::INDEX_BUFFER:
            {
                auto lock = indexBuffers_.getWriteLock();
                auto pair = indexBuffers_.insert(std::make_pair(id, DX12IndexBuffer{ data, format, sizeByte, id }));

                // then build a command to build underlaying resources
                threadPool->submitGPU(std::bind(&DX12IndexBuffer::create, &pair.first->second, std::placeholders::_1, std::placeholders::_2), std::string(), 0);
                threadPool->submitGPU(std::bind(&DX12IndexBuffer::cleanupCreate, &pair.first->second, std::placeholders::_1, std::placeholders::_2), std::string(), 1);
                threadPool->submitGeneric(std::bind(&DX12IndexBuffer::cleanupIntermediate, &pair.first->second), 2);
            }
            break;

            case Takoyaki::DX12Context::EResourceType::VERTEX_BUFFER:
            {
                auto lock = vertexBuffers_.getWriteLock();
                auto pair = vertexBuffers_.insert(std::make_pair(id, DX12VertexBuffer{ data, stride, sizeByte, id }));

                // then build a command to build underlaying resources
                threadPool->submitGPU(std::bind(&DX12VertexBuffer::create, &pair.first->second, std::placeholders::_1, std::placeholders::_2), std::string(), 0);
                threadPool->submitGPU(std::bind(&DX12VertexBuffer::cleanupCreate, &pair.first->second, std::placeholders::_1, std::placeholders::_2), std::string(), 1);
                threadPool->submitGeneric(std::bind(&DX12VertexBuffer::cleanupIntermediate, &pair.first->second), 2);
            }
            break;
        }
    }

    void DX12Context::createConstanBuffer(const std::string& name, uint_fast32_t size)
    {
        auto lock = constantBuffers_.getWriteLock();
        auto found = constantBuffers_.find(name);

        if (found != constantBuffers_.end())
            throw std::runtime_error{ "Constant buffers names must be unique" };

        // Constant buffers must be 256-byte aligned.
        size = (size + 255) & ~255;

        // we need a copy for each buffer in the swap chain
        auto res = constantBuffers_.insert(std::make_pair(name, DX12ConstantBuffer{ this, size, device_->getFrameCount() }));
    }

    void DX12Context::createInputLayout(const std::string& name)
    {
        auto lock = inputLayouts_.getWriteLock();

        inputLayouts_.insert(std::make_pair(name, DX12InputLayout{}));
    }

    void DX12Context::createPipelineState(const std::string& name, const PipelineStateDesc& desc)
    {
        auto lock = pipelineStates_.getWriteLock();

        pipelineStates_.insert(std::make_pair(name, DX12PipelineState{ desc }));
    }

    void DX12Context::createRootSignature(const std::string& name)
    {
        auto lock = rootSignatures_.getWriteLock();

        rootSignatures_.insert(std::make_pair(name, DX12RootSignature{}));
    }

    void DX12Context::createSwapchainTexture(uint_fast32_t id)
    {
        auto lock = textures_.getWriteLock();

        textures_.insert(std::make_pair(id, DX12Texture{ this }));
    }

    void DX12Context::createTexture(uint_fast32_t id, const TextureDesc& desc)
    {
        auto threadPool = threadPool_.lock();
        auto lock = textures_.getWriteLock();
        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

        if (desc.usage == EUsageType::CPU_READ) {
            // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770374(v=vs.85).aspx
            initialState = D3D12_RESOURCE_STATE_COPY_DEST;
        }

        auto pair = textures_.insert(std::make_pair(id, DX12Texture{ this, desc, initialState }));

        // then build a command to build underlaying resources
        pair.first->second.create(device_.get());
        //threadPool->submitGPU(std::bind(&DX12Texture::create, &pair.first->second, std::placeholders::_1, std::placeholders::_2), std::string(), 0);
    }

    void DX12Context::destroyDone()
    {
        DestroyQueueType::ValueType pair;

        if (destroyQueue_.tryPop(pair)) {
            switch (pair.first) {
                case EResourceType::INDEX_BUFFER:
                {
                    auto lock = indexBuffers_.getWriteLock();

                    indexBuffers_.erase(pair.second);
                }
                break;

                case EResourceType::TEXTURE:
                {
                    auto lock = textures_.getWriteLock();

                    textures_.erase(pair.second);
                }
                break;

                case EResourceType::VERTEX_BUFFER:
                {
                    auto lock = vertexBuffers_.getWriteLock();

                    vertexBuffers_.erase(pair.second);
                }
                break;
            }
        } else {
            throw std::runtime_error{ "DX12Context::onDestroyDone called but queue empty" };
        }
    }

    bool DX12Context::destroyMain(void* cmd, void* dev)
    {
        auto command = static_cast<TaskCommand*>(cmd);
        DestroyQueueType::ValueType pair;

        destroyQueue_.front(pair);

        switch (pair.first) {
            case Takoyaki::DX12Context::EResourceType::INDEX_BUFFER:
            {
                auto lock = indexBuffers_.getReadLock();
                auto found = indexBuffers_.find(pair.second);

                if (found != indexBuffers_.end()) {
                    found->second.destroy(cmd, dev);
                }
            }
            break;

            case EResourceType::TEXTURE:
            {
                auto lock = textures_.getReadLock();
                auto found = textures_.find(pair.second);

                if (found != textures_.end()) {
                    found->second.destroy(cmd, dev);
                }
            }
            break;

            case EResourceType::VERTEX_BUFFER:
            {
                auto lock = vertexBuffers_.getReadLock();
                auto found = vertexBuffers_.find(pair.second);

                if (found != vertexBuffers_.end()) {
                    found->second.destroy(cmd, dev);
                }
            }
            break;
        }

        return true;
    }

    void DX12Context::destroyResource(EResourceType type, uint_fast32_t id)
    {
        // destruction is deferred so add to destroy queue and submit a job request
        auto threadPool = threadPool_.lock();

        destroyQueue_.push(std::make_pair(type, id));
        threadPool->submitGPU(std::bind(&DX12Context::destroyMain, this, std::placeholders::_1, std::placeholders::_2), std::string(), 0);
        threadPool->submitGeneric(std::bind(&DX12Context::destroyDone, this), 1);
    }

    auto DX12Context::getConstantBuffer(const std::string& name) -> ConstantBufferReturn
    {
        auto lock = constantBuffers_.getReadLock();
        auto found = constantBuffers_.find(name);

        if (found == constantBuffers_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getConstantBuffer, cannot find key \"%1%\"" } % name;

            LOGW << boost::str(fmt);
            return ConstantBufferReturn(found->second, std::move(lock));
        }

        // Transfer the lock to the ConstantTableImpl to avoid removal while in use
        // it will be released once the user is done with it
        // somehow make_pair is not happy here..
        return ConstantBufferReturn(std::pair<DX12ConstantBuffer&, std::shared_lock<std::shared_timed_mutex>>(found->second, std::move(lock)));
    }

    const DX12IndexBuffer& DX12Context::getIndexBuffer(uint_fast32_t id)
    {
        auto lock = indexBuffers_.getReadLock();
        auto found = indexBuffers_.find(id);

        if (found == indexBuffers_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getIndexBuffer, cannot find key \"%1%\"" } % id;

            throw std::runtime_error{ boost::str(fmt) };
        }

        return found->second;
    }

    auto DX12Context::getInputLayout(const std::string& name) -> InputLayoutReturn
    {
        auto lock = inputLayouts_.getReadLock();
        auto found = inputLayouts_.find(name);

        if (found == inputLayouts_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getInputLayout, cannot find key \"%1%\"" } % name;

            throw std::runtime_error{ boost::str(fmt) };
        }

        return InputLayoutReturn(found->second, std::move(lock));
    }

    auto DX12Context::getPipelineState(const std::string& name) -> PipelineStateReturn
    {
        auto lock = pipelineStates_.getReadLock();
        auto found = pipelineStates_.find(name);

        if (found == pipelineStates_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getPipelineState, cannot find key \"%1%\"") % name;

            throw std::runtime_error{ boost::str(fmt) };
        }

        return PipelineStateReturn(found->second, std::move(lock));
    }

    auto DX12Context::getRootSignature(const std::string& name) -> RootSignatureReturn
    {
        auto lock = rootSignatures_.getReadLock();
        auto found = rootSignatures_.find(name);

        if (found == rootSignatures_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getRootSignature, cannot find key \"%1%\"" } % name;

            throw std::runtime_error{ boost::str(fmt) };
        }

        return RootSignatureReturn(found->second, std::move(lock));
    }

    DX12Texture& DX12Context::getTexture(uint_fast32_t id)
    {
        auto lock = textures_.getReadLock();
        auto found = textures_.find(id);

        if (found == textures_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getTexture, cannot find key \"%1%\"" } % id;

            throw std::runtime_error{ boost::str(fmt) };
        }

        return found->second;
    }

    const DX12VertexBuffer& DX12Context::getVertexBuffer(uint_fast32_t id)
    {
        auto lock = vertexBuffers_.getReadLock();
        auto found = vertexBuffers_.find(id);

        if (found == vertexBuffers_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getVertexBuffer, cannot find key \"%1%\"" } % id;

            throw std::runtime_error{ boost::str(fmt) };
        }

        return found->second;
    }
} // namespace Takoyaki