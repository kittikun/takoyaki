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

#include "pch.h"
#include "context.h"

#include <boost/format.hpp>

#include "../thread_pool.h"
#include "../utility/log.h"

namespace Takoyaki
{
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;

    DX12Context::DX12Context(const std::shared_ptr<DX12Device>& device, const std::shared_ptr<ThreadPool>& threadPool)
        : device_ { device }
        , threadPool_{ threadPool }
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

    void DX12Context::commit()
    {
        auto lock = device_->getDeviceLock();

        // create all root signatures
        {
            auto lock = rootSignatures_.getReadLock();

            for (auto& rs : rootSignatures_) {
                auto res = rs.second.create(device_);

                if (!res) {
                    auto fmt = boost::format{ "RootSignature contains no parameters: %1%" } % rs.first;

                    LOGW << boost::str(fmt);
                }
            }
        }

        // create all pipeline state
        {
            auto lock = pipelineStates_.getReadLock();

            for (auto& state : pipelineStates_)
                threadPool_->submit(std::bind(&DX12Context::commitMain, this, state.first));
        }
    }

    void DX12Context::commitMain(const std::string& name)
    {
        auto pair = getPipelineState(name);

        pair.first.create(device_, shared_from_this());
    }

    DX12ConstantBuffer& DX12Context::createConstanBuffer(const std::string& name)
    {
        auto lock = constantBuffers_.getWriteLock();
        auto found = constantBuffers_.find(name);

        if (found != constantBuffers_.end())
            throw new std::runtime_error{"Constant buffers names must be unique"};

        auto res = constantBuffers_.insert(std::make_pair(name, DX12ConstantBuffer{ shared_from_this() }));

        return res.first->second;
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

    DX12Texture& DX12Context::createTexture()
    {
        return textures_.push(DX12Texture{ shared_from_this() });
    }

    void DX12Context::createVertexBuffer(uint_fast32_t id, uint8_t* vertices, uint_fast64_t sizeVecticesByte)
    {
        auto lock = vertexBuffers_.getWriteLock();

        auto pair = vertexBuffers_.insert(std::make_pair(id, DX12VertexBuffer{ vertices, sizeVecticesByte }));

        // then build a command to build underlaying resources
        threadPool_->submit(WORKER_COPY, std::bind(&DX12VertexBuffer::create, &pair.first->second, std::placeholders::_1));
    }

    void DX12Context::destroyResource(EResourceType type, uint_fast32_t id)
    {
        //copyWorker_.submit()
        //auto lock = vertexBuffers_.getWriteLock();

        //// then build a command to build underlaying resources
        //threadPool_->submit(WORKER_COPY, std::bind(&DX12VertexBuffer::create, &pair.first->second, std::placeholders::_1));

        //vertexBuffers_.erase(id);
    }

    std::function<void()> DX12Context::destroyMain(void* context)
    {

        return std::bind(&DX12Context::onDestroyDone, this);
    }

    auto DX12Context::getConstantBuffer(const std::string& name) -> ConstantBufferReturn
    {
        auto lock = constantBuffers_.getReadLock();
        auto found = constantBuffers_.find(name);

        if (found == constantBuffers_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getConstantBuffer, cannot find key \"%1%\"" } % name;

            LOGW << boost::str(fmt);
            return ConstantBufferReturn();
        }

        // Transfer the lock to the ConstantTableImpl to avoid removal while in use
        // it will be released once the user is done with it
        // somehow make_pair is not happy here..
        return ConstantBufferReturn(std::pair<DX12ConstantBuffer&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock)));
    }

    auto DX12Context::getInputLayout(const std::string& name) -> InputLayoutReturn
    {
        auto lock = inputLayouts_.getReadLock();
        auto found = inputLayouts_.find(name);

        if (found == inputLayouts_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getInputLayout, cannot find key \"%1%\"" } % name;

            throw new std::runtime_error{ boost::str(fmt) };
        }

        return std::pair<DX12InputLayout&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    auto DX12Context::getPipelineState(const std::string& name) -> PipelineStateReturn
    {
        auto lock = pipelineStates_.getReadLock();
        auto found = pipelineStates_.find(name);

        if (found == pipelineStates_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getPipelineState, cannot find key \"%1%\"") % name;

            throw new std::runtime_error{ boost::str(fmt) };
        }

        return std::pair<DX12PipelineState&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    auto DX12Context::getRootSignature(const std::string& name) -> RootSignatureReturn
    {
        auto lock = rootSignatures_.getReadLock();
        auto found = rootSignatures_.find(name);

        if (found == rootSignatures_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getRootSignature, cannot find key \"%1%\"" } % name;

            throw new std::runtime_error{ boost::str(fmt) };
        }

        return std::pair<DX12RootSignature&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    D3D12_SHADER_BYTECODE DX12Context::getShaderImpl(RWLockMap<std::string, D3D12_SHADER_BYTECODE>& map, const std::string& name)
    {
        // called when trying to create a PSO so if needed, wait until shader is compiled
        auto lock = map.getReadLock();
        auto found = map.find(name);

        while (found == map.end()) {
            lock.unlock();
            std::this_thread::yield();
            lock.lock();
            found = map.find(name);
        }

        return found->second;
    }

    const DX12VertexBuffer& DX12Context::getVertexBuffer(uint_fast32_t id)
    {
        auto lock = vertexBuffers_.getReadLock();
        auto found = vertexBuffers_.find(id);

        if (found == vertexBuffers_.end()) {
            auto fmt = boost::format{ "DX12DeviceContext::getVertexBuffer, cannot find key \"%1%\"" } % id;

            throw new std::runtime_error{ boost::str(fmt) };
        }
        
        return found->second;
    }

    void DX12Context::initializeWorkers()
    {
        copyWorker_.initialize(device_, threadPool_);
    }

    void DX12Context::onDestroyDone()
    {
        //++destroyCount_;

        //if (destroyCount_ == destroyStack_.size()) {
        //    for (auto& iter : destroyStack_) {
        //        switch (iter->first) {
        //            case EResourceType::VERTEX_BUFFER: 
        //            {
        //                auto lock = vertexBuffers_.getWriteLock();

        //                vertexBuffers_.erase(iter->second);
        //            }
        //            break;
        //        }
        //    }

        //    destroyStack_.clear();
        //    destroyStack_.unlock();
        //}
    }
} // namespace Takoyaki