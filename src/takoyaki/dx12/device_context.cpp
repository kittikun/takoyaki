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
#include "device_context.h"

#include <boost/format.hpp>

#include "../utility/log.h"

namespace Takoyaki
{
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;

    DX12DeviceContext::DX12DeviceContext(std::weak_ptr<DX12Device> owner)
        : owner_ { owner }
        , descHeapRTV_{ owner }
        , descHeapSRV_{ owner }
    {
    }

    void DX12DeviceContext::commit()
    {
        auto device = owner_.lock();
        auto lock = device->getLock();

        // create all root signatures
        {
            auto lock = rootSignatures_.getReadLock();

            for (auto& rs : rootSignatures_) {
                auto res = rs.second.create(device);

                if (!res) {
                    auto fmt = boost::format("RootSignature contains no parameters: %1%") % rs.first;

                    LOGW << boost::str(fmt);
                }
            }
        }
    }

    DX12ConstantBuffer& DX12DeviceContext::createConstanBuffer(const std::string& name)
    {
        auto lock = constantBuffers_.getWriteLock();
        auto found = constantBuffers_.find(name);

        if (found != constantBuffers_.end())
            throw new std::runtime_error{"Constant buffers names must be unique"};

        auto res = constantBuffers_.insert(std::make_pair(name, DX12ConstantBuffer{ shared_from_this() }));

        return res.first->second;
    }

    void DX12DeviceContext::createInputLayout(const std::string& name)
    {
        auto lock = inputLayouts_.getWriteLock();

        inputLayouts_.insert(std::make_pair(name, DX12InputLayout()));
    }

    void DX12DeviceContext::createRootSignature(const std::string& name)
    {
        auto lock = rootSignatures_.getWriteLock();

        rootSignatures_.insert(std::make_pair(name, DX12RootSignature()));
    }

    DX12Texture& DX12DeviceContext::CreateTexture()
    {
        return textures_.push(DX12Texture{ shared_from_this() });
    }

    auto DX12DeviceContext::getConstantBuffer(const std::string& name) -> ConstantBufferReturn
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

    auto DX12DeviceContext::getInputLayout(const std::string& name) -> InputLayoutReturn
    {
        auto lock = inputLayouts_.getReadLock();
        auto found = inputLayouts_.find(name);

        if (found == inputLayouts_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getInputLayout, cannot find key \"%1%\"") % name;

            throw new std::runtime_error(boost::str(fmt));
        }

        return std::pair<DX12InputLayout&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }

    auto DX12DeviceContext::getRootSignature(const std::string& name)->RootSignatureReturn
    {
        auto lock = rootSignatures_.getReadLock();
        auto found = rootSignatures_.find(name);

        if (found == rootSignatures_.end()) {
            auto fmt = boost::format("DX12DeviceContext::getRootSignature, cannot find key \"%1%\"") % name;

            throw new std::runtime_error(boost::str(fmt));
        }

        return std::pair<DX12RootSignature&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock));
    }
} // namespace Takoyaki