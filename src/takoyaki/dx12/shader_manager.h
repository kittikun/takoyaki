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

#include <boost/thread.hpp>

namespace Takoyaki
{
    class IO;
    struct ProgramDesc;

    struct Program
    {
        D3D12_SHADER_BYTECODE vs;
        D3D12_SHADER_BYTECODE ps;
    };

    class ShaderManager
    {
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;
        ShaderManager(ShaderManager&&) = delete;
        ShaderManager& operator=(ShaderManager&&) = delete;

    public:
        ShaderManager();
        ~ShaderManager();

        void initialize(IO*);
        const Program& getProgram(const std::string& name) const;

    private:
        void mainCompiler(IO* io);
        void compileShaders(IO* io, const std::vector<ProgramDesc>&);
        void getShaderBindings(ID3DBlob*);
        std::string getDXShaderType(const std::string& type) const;
        void parseShaderList(const std::string&, std::vector<ProgramDesc>&) const;

    private:
        mutable boost::shared_mutex rwMutex_;
        // Note: add some synchronization to allow recompilation during runtime
        std::unordered_map<std::string, Program> programList_;
    };
} // namespace Takoyaki