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
#include "shader_compiler.h"

#include <boost/format.hpp>
#include <rapidjson/document.h>

#include "DXUtility.h"
#include "DX12DeviceContext.h"
#include "../io.h"
#include "../thread_pool.h"
#include "../utility/log.h"
#include "../utility/win_utility.h"

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

    ShaderCompiler::ShaderCompiler() = default;
    ShaderCompiler::~ShaderCompiler() = default;

    void ShaderCompiler::compileProgram(IO* io, const ProgramDesc& programDesc, std::weak_ptr<DX12DeviceContext> context)
    {
        LOG_IDENTIFY_THREAD;
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

    std::string ShaderCompiler::getDXShaderType(const std::string& type) const
    {
        std::string res;

        if (type == "vs")
            res = "vs_5_0";
        else if (type == "ps")
            res = "ps_5_0";
        else
            throw new std::runtime_error("ShaderCompiler::getDXShaderType, unknown type");

        return res;
    }

    void ShaderCompiler::getShaderResources(ID3DBlob* blob, std::weak_ptr<DX12DeviceContext> context)
    {
        // use reflection to extract bind info
        ID3D12ShaderReflection* reflect = nullptr;
        D3D12_SHADER_DESC progDesc;

        D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&reflect));
        reflect->GetDesc(&progDesc);

        parseConstantBuffers(reflect, progDesc, context);

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

    void ShaderCompiler::main(IO* io, std::weak_ptr<ThreadPool> threadPool, std::weak_ptr<DX12DeviceContext> context)
    {
        LOG_IDENTIFY_THREAD;
        LOGS_INDENT_START << "Starting shaders compilation..";

        auto buffer = io->loadFile("data/shaderlist.json");

        std::vector<ProgramDesc> programList;

        parseShaderList(buffer, programList);
        auto pool = threadPool.lock();

        for (auto& programDesc : programList) {
            pool->submit(std::bind(&ShaderCompiler::compileProgram, this, io, programDesc, context));
        }

        LOGS_INDENT_END << "Shader compilation done.";
    }

    void ShaderCompiler::parseConstantBuffers(ID3D12ShaderReflection* reflect, const D3D12_SHADER_DESC& progDesc, std::weak_ptr<DX12DeviceContext> context)
    {
        auto fmt = boost::format("Number of constant buffers %1%") % progDesc.ConstantBuffers;
        LOGS << boost::str(fmt);

        for (uint_fast32_t i = 0; i < progDesc.ConstantBuffers; ++i) {
            auto cb = reflect->GetConstantBufferByIndex(i);
            D3D12_SHADER_BUFFER_DESC cbDesc;

            cb->GetDesc(&cbDesc);

            switch (cbDesc.Type) {
                case D3D_CBUFFER_TYPE::D3D10_CT_CBUFFER:
                {
                    LOGS << "Constant buffer : " << cbDesc.Name;

                    auto& cbuffer = context.lock()->CreateConstanBuffer(cbDesc.Name);

                    for (uint_fast32_t j = 0; j < cbDesc.Variables; ++j) {
                        auto var = cb->GetVariableByIndex(j);

                        D3D12_SHADER_VARIABLE_DESC vardesc;

                        var->GetDesc(&vardesc);
                        auto fmt = boost::format("CB var : %1%, Offset: %2%, Size: %3%") % vardesc.Name % vardesc.StartOffset % vardesc.Size;
                        LOGS << boost::str(fmt);

                        cbuffer.addVariable(vardesc.Name, vardesc.StartOffset, vardesc.Size);
                    }
                }
                break;
                
                case D3D_CBUFFER_TYPE::D3D_CT_RESOURCE_BIND_INFO:
                {
                    LOGS << "Resource bind info : " << cbDesc.Name;
                }
                break;

                default:
                    LOGS << cbDesc.Type;
                    break;
            }
        }
    }

    void ShaderCompiler::parseShaderList(const std::string& data, std::vector<ProgramDesc>& programList) const
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
                        uint_fast32_t flags = 0;

                        for (auto member = item->MemberBegin(); member != item->MemberEnd(); ++member) {
                            // Parse attribute, not order does not matter
                            auto name = std::string{ member->name.GetString() };

                            if (name == "type")
                                shader.type = member->value.GetString();
                            else if (name == "path")
                                shader.path = member->value.GetString();
                            else if (name == "main")
                                shader.main = member->value.GetString();
                            else if (name == "flags") {
                                auto type = std::string{ member->value.GetString() };
                                if (type == "debug")
                                    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
                            }                            
                        }

                        if (flags == 0)
                            shader.flags = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

                        program.shaders.push_back(shader);
                    } else {
                        throw new std::runtime_error("Shaderlist expected an object to describe shader");
                    }
                }
            } else {
                throw new std::runtime_error("Shaderlist expected an array after program name");
            }

            programList.push_back(program);
        }
    }

} // namespace Takoyaki