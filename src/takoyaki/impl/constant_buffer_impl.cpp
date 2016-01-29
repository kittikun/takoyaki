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
#include "constant_buffer_impl.h"

#include "../dx12/dx12_context.h"
#include "../dx12/dx12_device.h"

namespace Takoyaki
{
    ConstantBufferImpl::ConstantBufferImpl(const std::shared_ptr<DX12Context>& context, const std::shared_ptr<DX12Device>& device, DX12ConstantBuffer& cbuffer, std::shared_lock<std::shared_timed_mutex> lock) noexcept
        : context_{ context }
        , device_{ device }
        , cbuffer_(cbuffer)
        , bufferLock_(std::move(lock))
    {
    }

    void ConstantBufferImpl::setMatrix4x4(const std::string& name, const glm::mat4x4& value)
    {
        auto context = context_.lock();
        auto device = device_.lock();

        cbuffer_.setMatrix4x4(name, value, device->getCurrentFrame());
    }
}
// namespace Takoyaki