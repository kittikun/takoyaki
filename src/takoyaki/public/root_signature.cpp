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
#include "root_signature.h"

#include <glm/glm.hpp>

#include "../impl/root_signature_impl.h"

namespace Takoyaki
{
    RootSignature::RootSignature(std::unique_ptr<RootSignatureImpl> rs) noexcept
        : impl_{ std::move(rs) }
    {
    }

    RootSignature::~RootSignature() = default;


    void RootSignature::addConstant(uint_fast32_t numValues, uint_fast32_t shaderRegister)
    {
        impl_->addConstant(numValues, shaderRegister);
    }

    void RootSignature::addDescriptorConstantBuffer(uint_fast32_t shaderRegister)
    {
        impl_->addDescriptorConstantBuffer(shaderRegister);
    }

    void RootSignature::addDescriptorUnorderedAccess(uint_fast32_t shaderRegister)
    {
        impl_->addDescriptorUnorderedAccess(shaderRegister);
    }

    void RootSignature::addDescriptorShaderResource(uint_fast32_t shaderRegister)
    {
        impl_->addDescriptorShaderResource(shaderRegister);
    }

    uint_fast32_t RootSignature::addDescriptorTable()
    {
        return impl_->addDescriptorTable();
    }

    void RootSignature::addDescriptorRange(uint_fast32_t index, D3D12_DESCRIPTOR_RANGE_TYPE type, uint_fast32_t numDescriptors, uint_fast32_t baseShaderRegister)
    {
        impl_->addDescriptorRange(index, type, numDescriptors, baseShaderRegister);
    }

    void RootSignature::setFlags(D3D12_ROOT_SIGNATURE_FLAGS flags)
    {
        impl_->setFlags(flags);
    }

}
// namespace Takoyaki