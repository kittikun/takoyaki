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
#include "DX12DeviceContext.h"
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

    void ShaderManager::compileShaders(IO* io, const std::vector<ProgramDesc>& programList, std::weak_ptr<DX12DeviceContext> context)
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

                auto hr = D3DCompile(buffer.c_str(), buffer.size(), shaderDesc.path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderDesc.main.c_str(), getDXShaderType(shaderDesc.type).c_str(), shaderDesc.flags, 0, &shaderBlob, &errorBlob);

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

                getShaderResources(shaderBlob, context);
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

    void ShaderManager::getShaderResources(ID3DBlob* blob, std::weak_ptr<DX12DeviceContext> context)
    {
        // use reflection to extract bind info
        ID3D12ShaderReflection* reflect = nullptr;
        D3D12_SHADER_DESC progDesc;

        D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&reflect));
        reflect->GetDesc(&progDesc);

        // constant buffers
        for (uint_fast32_t i = 0; i < progDesc.ConstantBuffers; ++i) {
            auto cb = reflect->GetConstantBufferByIndex(i);
            D3D12_SHADER_BUFFER_DESC cbDesc;

            cb->GetDesc(&cbDesc);

            LOGS << "Constant buffer : " << cbDesc.Name;

            for (uint_fast32_t j = 0; j < cbDesc.Variables; ++j) {
                auto var = cb->GetVariableByIndex(j);
                D3D12_SHADER_VARIABLE_DESC vardesc;

                var->GetDesc(&vardesc);
                LOGS << "CB var : " << vardesc.Name << ", Size:" << vardesc.Size;
            }
        }

        // shader resources (textures and buffers) bound
        for (uint_fast32_t i = 0; i < progDesc.BoundResources; ++i) {
            D3D12_SHADER_INPUT_BIND_DESC resdesc;
            auto res = reflect->GetResourceBindingDesc(i, &resdesc);

            switch (resdesc.Type) {
                case D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_BYTEADDRESS";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_CBUFFER";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_SAMPLER";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_STRUCTURED";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_TBUFFER";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_TEXTURE";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_UAV_APPEND_STRUCTURED";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_UAV_CONSUME_STRUCTURED";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_UAV_RWBYTEADDRESS";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_UAV_RWSTRUCTURED";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_UAV_RWTYPED";
                    break;

                case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                    LOGS << "Resource binding : " << resdesc.Name << ", D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER";
                    break;

                default:
                    throw new std::runtime_error("Unknown D3D_SHADER_INPUT_TYPE");
                    break;
            }
        }
    }

    void ShaderManager::initialize(IO* io, std::weak_ptr<DX12DeviceContext> context)
    {
        // shaders are processed in a different thread
        auto thread = std::thread{ std::bind(&ShaderManager::mainCompiler, this, std::placeholders::_1, std::placeholders::_2), io, context };

        setThreadName(thread.native_handle(), "Shader Compiler");

        //we don't need to track this one so let it run in the background
        thread.detach();
    }

    void ShaderManager::mainCompiler(IO* io, std::weak_ptr<DX12DeviceContext> context)
    {
        LOGS_INDENT_START << "Starting shaders compilation..";

        auto buffer = io->loadFile("data/shaderlist.json");

        std::vector<ProgramDesc> programList;
        
        parseShaderList(buffer, programList);
        compileShaders(io, programList, context);

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