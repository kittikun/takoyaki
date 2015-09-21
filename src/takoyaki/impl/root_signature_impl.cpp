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
#include "root_signature_impl.h"

#include "../dx12/dx12_root_signature.h"

namespace Takoyaki
{
    RootSignatureImpl::RootSignatureImpl(DX12RootSignature& rs, boost::shared_lock<boost::shared_mutex> lock)
        : rs_{ rs }
        , bufferLock_{ std::move(lock) }
    {

    }

    RootSignatureImpl::~RootSignatureImpl() = default;

    void RootSignatureImpl::addConstant(uint_fast32_t numValues, uint_fast32_t shaderRegister)
    {
        rs_.addConstant(numValues, shaderRegister);
    }

    void RootSignatureImpl::addDescriptorConstantBuffer(uint_fast32_t shaderRegister)
    {
        rs_.addDescriptorConstantBuffer(shaderRegister);
    }

    void RootSignatureImpl::addDescriptorUnorderedAccess(uint_fast32_t shaderRegister)
    {
        rs_.addDescriptorUnorderedAccess(shaderRegister);
    }

    void RootSignatureImpl::addDescriptorShaderResource(uint_fast32_t shaderRegister)
    {
        rs_.addDescriptorShaderResource(shaderRegister);
    }

    uint_fast32_t RootSignatureImpl::addDescriptorTable()
    {
        return rs_.addDescriptorTable();
    }

    void RootSignatureImpl::addDescriptorRange(uint_fast32_t index, D3D12_DESCRIPTOR_RANGE_TYPE type, uint_fast32_t numDescriptors, uint_fast32_t baseShaderRegister)
    {
        rs_.addDescriptorRange(index, type, numDescriptors, baseShaderRegister);
    }

    void RootSignatureImpl::setFlags(D3D12_ROOT_SIGNATURE_FLAGS flags)
    {
        rs_.setFlags(flags);
    }
}
// namespace Takoyaki