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

#include <memory>
#include <string>
#include <d3d12.h>

#include "definitions.h"

namespace Takoyaki
{
    class RootSignatureImpl;

    class RootSignature
    {
        RootSignature(const RootSignature&) = delete;
        RootSignature& operator=(const RootSignature&) = delete;
        RootSignature(RootSignature&&) = delete;
        RootSignature& operator=(RootSignature&&) = delete;

    public:
        RootSignature(std::unique_ptr<RootSignatureImpl>) noexcept;
        ~RootSignature() noexcept;

        void addConstant(uint_fast32_t numValues, uint_fast32_t shaderRegister);
        void addDescriptorConstantBuffer(uint_fast32_t shaderRegister);
        void addDescriptorUnorderedAccess(uint_fast32_t shaderRegister);
        void addDescriptorShaderResource(uint_fast32_t shaderRegister);

        // return an index to be used with addDescriptorRange
        uint_fast32_t addDescriptorTable();

        // only when using descriptor tables
        void addDescriptorRange(uint_fast32_t index, EDescriptorType type, uint_fast32_t numDescriptors, uint_fast32_t baseShaderRegister);

        void setFlags(uint_fast32_t flags);


    private:
        std::unique_ptr<RootSignatureImpl> impl_;
    };
}
// namespace Takoyaki
