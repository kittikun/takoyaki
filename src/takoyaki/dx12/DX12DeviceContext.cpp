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
#include "DX12DeviceContext.h"

#include <boost/format.hpp>

#include "../utility/log.h"

namespace Takoyaki
{
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
    extern template DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;

    DX12DeviceContext::DX12DeviceContext(std::weak_ptr<DX12Device> owner)
        : owner_{ owner_ }
        , descHeapRTV_{ owner }
        , descHeapSRV_{ owner }
    {

    }

    DX12ConstantBuffer& DX12DeviceContext::CreateConstanBuffer(const std::string& name)
    {
        auto lock = constantBuffers_.getWriteLock();
        auto found = constantBuffers_.find(name);

        if (found != constantBuffers_.end())
            throw new std::runtime_error{"Constant buffers names must be unique"};

        auto res = constantBuffers_.insert(std::make_pair(name, DX12ConstantBuffer{ shared_from_this() }));

        return res.first->second;
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

        // somehow make_pair is not happy here..
        // transfer the lock to the ConstantTableImpl to avoid removal while in use
        // it will be released once the user is done with it
        return ConstantBufferReturn(std::pair<DX12ConstantBuffer&, boost::shared_lock<boost::shared_mutex>>(found->second, std::move(lock)));
    }

} // namespace Takoyaki