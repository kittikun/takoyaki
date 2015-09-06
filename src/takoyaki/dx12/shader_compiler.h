// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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

#include <d3dcompiler.h>

namespace Takoyaki
{
    class DX12DeviceContext;
    class IO;
    struct ProgramDesc;
    class ThreadPool;

    struct Program
    {
        D3D12_SHADER_BYTECODE vs;
        D3D12_SHADER_BYTECODE ps;
    };

    class ShaderCompiler
    {
        ShaderCompiler(const ShaderCompiler&) = delete;
        ShaderCompiler& operator=(const ShaderCompiler&) = delete;
        ShaderCompiler(ShaderCompiler&&) = delete;
        ShaderCompiler& operator=(ShaderCompiler&&) = delete;
    public:
        ShaderCompiler();
        ~ShaderCompiler();

        void main(IO*, std::weak_ptr<DX12DeviceContext>);

    private:
        void compileShaders(IO*, const std::vector<ProgramDesc>&, std::weak_ptr<DX12DeviceContext>);
        void getShaderResources(ID3DBlob*, std::weak_ptr<DX12DeviceContext>);
        std::string getDXShaderType(const std::string&) const;
        void parseConstantBuffers(ID3D12ShaderReflection*, const D3D12_SHADER_DESC&, std::weak_ptr<DX12DeviceContext>);
        void parseShaderList(const std::string&, std::vector<ProgramDesc>&) const;
    };
} // namespace Takoyaki