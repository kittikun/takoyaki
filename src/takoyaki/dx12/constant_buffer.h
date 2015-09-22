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

#pragma once

#include <d3d12.h>
#include <memory>
#include <unordered_map>

namespace Takoyaki
{
    class DX12Context;

    class DX12ConstantBuffer
    {
        DX12ConstantBuffer(const DX12ConstantBuffer&) = delete;
        DX12ConstantBuffer& operator=(const DX12ConstantBuffer&) = delete;
        DX12ConstantBuffer& operator=(DX12ConstantBuffer&&) = delete;

    public:
        DX12ConstantBuffer(std::weak_ptr<DX12Context>);
        DX12ConstantBuffer(DX12ConstantBuffer&&) noexcept;
        ~DX12ConstantBuffer();

        const D3D12_CPU_DESCRIPTOR_HANDLE& getConstantBufferView() { return rtv_; }

        void setMatrix4x4(const std::string&, const glm::mat4x4&);

    private:
        void addVariable(const std::string& name, uint_fast32_t offset, uint_fast32_t size);

    private:
        struct CBVariable
        {
            uint_fast32_t offset;
            uint_fast32_t size;
        };

        std::weak_ptr<DX12Context> owner_;
        std::vector<uint8_t> buffer_;
        std::unordered_map<std::string, CBVariable> offsetMap_;     // TODO: thread unsafe
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_;

        friend class ShaderCompiler;
    };
} // namespace Takoyaki