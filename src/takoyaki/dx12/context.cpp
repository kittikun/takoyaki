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

    DX12Context::DX12Context(const std::shared_ptr<DX12Device>& device, const std::shared_ptr<ThreadPool>& threadPool) noexcept
        : device_ { device }
        , threadPool_{ threadPool }
        , descHeapRTV_{ device }
        , descHeapSRV_{ device }
    {

    }

    void DX12Context::addShader(EShaderType type, const std::string& name, D3D12_SHADER_BYTECODE&& bc)
    {
        switch (type) {
            case Takoyaki::EShaderType::COMPUTE:
            {
                auto lock = shaderVertex_.getWriteLock();

                shaderCompute_.insert(std::make_pair(name, std::move(bc)));
            }
            break;

            case Takoyaki::EShaderType::DOMAIN:
            {
                auto lock = shaderVertex_.getWriteLock();

                shaderDomain_.insert(std::make_pair(name, std::move(bc)));
            }
            break;

            case Takoyaki::EShaderType::GEOMETRY:
            {
                auto lock = shaderVertex_.getWriteLock();

                shaderGeometry_.insert(std::make_pair(name, std::move(bc)));
            }
            break;

            case Takoyaki::EShaderType::HULL:
            {
                auto lock = shaderVertex_.getWriteLock();

                shaderPixel_.insert(std::make_pair(name, std::move(bc)));
            }
            break;

            case Takoyaki::EShaderType::PIXEL:
            {
                auto lock = shaderVertex_.getWriteLock();

                shaderPixel_.insert(std::make_pair(name, std::move(bc)));

            }
            break;

            case Takoyaki::EShaderType::VERTEX:
            {
                auto lock = shaderVertex_.getWriteLock();

                shaderVertex_.insert(std::make_pair(name, std::move(bc)));
            }
            break;
        }
    }

    void DX12Context::commit()
    {
        auto lock = device_->getLock();

        // create all root signatures
        {
            auto lock = rootSignatures_.getReadLock();

            for (auto& rs : rootSignatures_) {
                auto res = rs.second.create(device_);

                if (!res) {
                    auto fmt = boost::format("RootSignature contains no parameters: %1%") % rs.first;

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

    auto DX12Context::getConstantBuffer(const std::string& name) -> ConstantBufferReturn
    {
        auto lock = constantBuffers_.getReadLock();
        auto found = constantBuffers_.find(name);

        if (found == constantBuffers_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getConstantBuffer, cannot find key \"%1%\"") % name;

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
            auto fmt = boost::format("DX12DeviceContext::getInputLayout, cannot find key \"%1%\"") % name;

            throw new std::runtime_error(boost::str(fmt));
        }

        return std::pair<DX12InputLayout&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    auto DX12Context::getPipelineState(const std::string& name) -> PipelineStateReturn
    {
        auto lock = pipelineStates_.getReadLock();
        auto found = pipelineStates_.find(name);

        if (found == pipelineStates_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getPipelineState, cannot find key \"%1%\"") % name;

            throw new std::runtime_error(boost::str(fmt));
        }

        return std::pair<DX12PipelineState&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    auto DX12Context::getRootSignature(const std::string& name) -> RootSignatureReturn
    {
        auto lock = rootSignatures_.getReadLock();
        auto found = rootSignatures_.find(name);

        if (found == rootSignatures_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getRootSignature, cannot find key \"%1%\"") % name;

            throw new std::runtime_error(boost::str(fmt));
        }

        return std::pair<DX12RootSignature&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    D3D12_SHADER_BYTECODE DX12Context::getShader(EShaderType type, const std::string& name)
    {
        D3D12_SHADER_BYTECODE res;

        switch (type) {
            case Takoyaki::EShaderType::COMPUTE:
                res = getShaderImpl(shaderCompute_, name);
                break;

            case Takoyaki::EShaderType::DOMAIN:
                res = getShaderImpl(shaderDomain_, name);
                break;

            case Takoyaki::EShaderType::GEOMETRY:
                res = getShaderImpl(shaderGeometry_, name);
                break;

            case Takoyaki::EShaderType::HULL:
                res = getShaderImpl(shaderHull_, name);
                break;

            case Takoyaki::EShaderType::PIXEL:
                res = getShaderImpl(shaderPixel_, name);
                break;

            case Takoyaki::EShaderType::VERTEX:
                res = getShaderImpl(shaderVertex_, name);
                break;
        }

        return res;
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
} // namespace Takoyaki