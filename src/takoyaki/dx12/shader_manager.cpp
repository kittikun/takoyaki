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
#include "shader_manager.h"

#include <d3dcompiler.h>
#include <boost/format.hpp>
#include <rapidjson/document.h>

#include "DXUtility.h"
#include "../io.h"
#include "../utility/log.h"
#include "../utility/winUtility.h"

namespace Takoyaki
{
    // Some definition for json shader list
    struct ShaderDesc
    {
        std::string type;
        std::string path;
        std::string main;
        uint_fast32_t flags;
    };

    struct ProgramDesc
    {
        std::string name;
        std::vector<ShaderDesc> shaders;
    };

    ShaderManager::ShaderManager() = default;
    ShaderManager::~ShaderManager() = default;

    void ShaderManager::compileShaders(IO* io, const std::vector<ProgramDesc>& programList)
    {
        // Compile shaders, might want to move this to a function so it can be called outside of framework ?
        for (auto programDesc : programList) {
            auto fmt = boost::format("Compiling program \"%1%\"") % programDesc.name;
            LOGS_INDENT_START << boost::str(fmt);

            Program prog;

            for (auto shaderDesc : programDesc.shaders) {
                auto fmt = boost::format("Path: %1%, Type: %2%, Entry: %3%") % shaderDesc.path % shaderDesc.type % shaderDesc.main;

                LOGS << boost::str(fmt);

                auto buffer = io->loadFile(shaderDesc.path);

                ID3DBlob* shaderBlob = nullptr;
                ID3DBlob* errorBlob = nullptr;

                auto hr = D3DCompile(buffer.c_str(), buffer.size(), shaderDesc.path.c_str(), nullptr, nullptr, shaderDesc.main.c_str(), getDXShaderType(shaderDesc.type).c_str(), shaderDesc.flags, 0, &shaderBlob, &errorBlob);

                if (FAILED(hr)) {
                    if (errorBlob != nullptr) {
                        LOGE << "Compilation failed" << std::endl << static_cast<char*>(errorBlob->GetBufferPointer());
                        errorBlob->Release();
                    }

                    if (shaderBlob != nullptr)
                        shaderBlob->Release();

                    fmt = boost::format("Shader compilation failed: %1%") % shaderDesc.path;

                    throw new std::runtime_error(boost::str(fmt));
                }

                D3D12_SHADER_BYTECODE bc;

                bc.BytecodeLength = shaderBlob->GetBufferSize();
                bc.pShaderBytecode = shaderBlob->GetBufferPointer();

                if (shaderDesc.type == "vs")
                    prog.vs = bc;
                else if (shaderDesc.type == "ps")
                    prog.ps = bc;

                getShaderBindings(shaderBlob);
            }

            // Get write privilege on the map
            {
                std::lock_guard<boost::shared_mutex> lock(rwMutex_);

                // a copy might happen here ?
                programList_.insert({ programDesc.name, prog });
            }

            LOGS_INDENT_END << "Program done.";
        }
    }

    std::string ShaderManager::getDXShaderType(const std::string& type) const
    {
        std::string res;

        if (type == "vs")
            res = "vs_5_0";
        else if (type == "ps")
            res = "ps_5_0";
        else
            throw new std::runtime_error("ShaderManager::getDXShaderType, unknown type");

        return res;
    }

    void ShaderManager::getShaderBindings(ID3DBlob* blob)
    {
        // use reflection to extract bind info
        ID3D12ShaderReflection* reflect = nullptr;
        D3D12_SHADER_DESC progDesc;

        D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&reflect));
        reflect->GetDesc(&progDesc);

        for (uint_fast32_t i = 0; i < progDesc.ConstantBuffers; ++i) {
            auto cb = reflect->GetConstantBufferByIndex(i);
            D3D12_SHADER_BUFFER_DESC cbDesc;

            cb->GetDesc(&cbDesc);

            LOGS << cbDesc.Name;

            for (uint_fast32_t j = 0; j < cbDesc.Variables; ++j) {
                auto var = cb->GetVariableByIndex(j);
                D3D12_SHADER_VARIABLE_DESC vardesc;

                var->GetDesc(&vardesc);
                LOGS << vardesc.Name;
            }
        }
    }

    const Program& ShaderManager::getProgram(const std::string& name) const
    {
        // RW lock so should block unless someone is updating the map
        boost::shared_lock<boost::shared_mutex> lock{ rwMutex_ };

        auto found = programList_.find(name);

        if (found == programList_.end())
            throw new std::runtime_error("ShaderManager::getProgram, unknown name");

        return found->second;
    }

    void ShaderManager::initialize(IO* io)
    {
        // shaders are processed in a different thread
        auto thread = std::thread{ std::bind(&ShaderManager::mainCompiler, this, std::placeholders::_1), io };

        setThreadName(thread.native_handle(), "Shader Compiler");

        //we don't need to track this one so let it run in the background
        thread.detach();
    }

    void ShaderManager::mainCompiler(IO* io)
    {
        LOGS_INDENT_START << "Starting shaders compilation..";

        auto buffer = io->loadFile("data/shaderlist.json");

        std::vector<ProgramDesc> programList;
        
        parseShaderList(buffer, programList);
        compileShaders(io, programList);

        LOGS_INDENT_END << "Shader compilation done.";
    }

    void ShaderManager::parseShaderList(const std::string& data, std::vector<ProgramDesc>& programList) const
    {
        rapidjson::Document doc;

        LOGS << "Parsing program list..";
        doc.Parse(data.c_str());

        for (auto iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter) {
            ProgramDesc program;

            program.name = iter->name.GetString();

            if (iter->value.IsArray()) {
                for (auto item = iter->value.Begin(); item != iter->value.End(); ++item) {
                    if (item->IsObject()) {
                        ShaderDesc shader;

                        shader.flags = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

                        for (auto member = item->MemberBegin(); member != item->MemberEnd(); ++member) {
                            // Parse attribute, not order does not matter
                            if (strcmp(member->name.GetString(), "type") == 0)
                                shader.type = member->value.GetString();
                            else if (strcmp(member->name.GetString(), "path") == 0)
                                shader.path = member->value.GetString();
                            else if (strcmp(member->name.GetString(), "main") == 0)
                                shader.main = member->value.GetString();
                        }   

                        program.shaders.push_back(shader);
                    } else {
                        throw new std::runtime_error("Shaderlist expected an object to descript shader");
                    }
                }
            } else {
                throw new std::runtime_error("Shaderlist expected an array after program name");
            }

            programList.push_back(program);
        }
    }
} // namespace Takoyaki