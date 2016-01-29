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

namespace Takoyaki
{
    class DX12Context;
    class DX12ConstantBuffer;
    class DX12Device;

    class ConstantBufferImpl
    {
        ConstantBufferImpl(const ConstantBufferImpl&) = delete;
        ConstantBufferImpl& operator=(const ConstantBufferImpl&) = delete;
        ConstantBufferImpl(ConstantBufferImpl&&) = delete;
        ConstantBufferImpl& operator=(ConstantBufferImpl&&) = delete;

    public:
        ConstantBufferImpl(const std::shared_ptr<DX12Context>&, const std::shared_ptr<DX12Device>&, DX12ConstantBuffer&, std::shared_lock<std::shared_timed_mutex>) noexcept;
        ~ConstantBufferImpl() = default;

        void setMatrix4x4(const std::string&, const glm::mat4x4&);

    private:
        std::weak_ptr<DX12Context> context_;    // must own pointer to context for destruction
        std::weak_ptr<DX12Device> device_;      // to update correct frame CB
        DX12ConstantBuffer& cbuffer_;
        std::shared_lock<std::shared_timed_mutex> bufferLock_;    // to avoid removal while user is still using it
    };
}
// namespace Takoyaki
