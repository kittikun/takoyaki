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
    class DX12RootSignature;

    class RootSignatureImpl
    {
        RootSignatureImpl(const RootSignatureImpl&) = delete;
        RootSignatureImpl& operator=(const RootSignatureImpl&) = delete;
        RootSignatureImpl(RootSignatureImpl&&) = delete;
        RootSignatureImpl& operator=(RootSignatureImpl&&) = delete;

    public:
        explicit RootSignatureImpl(DX12RootSignature&, boost::shared_lock<boost::shared_mutex>) noexcept;
        ~RootSignatureImpl() = default;

        void addConstant(uint_fast32_t, uint_fast32_t);
        void addDescriptorConstantBuffer(uint_fast32_t);
        void addDescriptorUnorderedAccess(uint_fast32_t);
        void addDescriptorShaderResource(uint_fast32_t);
        uint_fast32_t addDescriptorTable();
        void addDescriptorRange(uint_fast32_t, D3D12_DESCRIPTOR_RANGE_TYPE, uint_fast32_t, uint_fast32_t);
        void setFlags(D3D12_ROOT_SIGNATURE_FLAGS);

    private:
        DX12RootSignature& rs_;
        boost::shared_lock<boost::shared_mutex> bufferLock_;    // to avoid removal while user is still using it
    };
}
// namespace Takoyaki
