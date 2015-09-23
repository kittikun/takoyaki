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

#include "dxutility.h"
#include "context.h"
#include "../io.h"
#include "../thread_pool.h"
#include "../utility/log.h"
#include "../utility/win_utility.h"

namespace Takoyaki
{
    std::string getDXShaderType(EShaderType type)
    {
        std::string res;

        switch (type) {
            case Takoyaki::EShaderType::COMPUTE:
                res = "cs_5_1";
                break;
            case Takoyaki::EShaderType::DOMAIN:
                res = "ds_5_1";
                break;
            case Takoyaki::EShaderType::GEOMETRY:
                res = "gs_5_1";
                break;
            case Takoyaki::EShaderType::HULL:
                res = "hs_5_1";
                break;
            case Takoyaki::EShaderType::PIXEL:
                res = "ps_5_1";
                break;
            case Takoyaki::EShaderType::VERTEX:
                res = "vs_5_1";
                break;

            default:
                throw new std::runtime_error("ShaderCompiler::getDXShaderType, unknown type");
        }

        return res;
    }

    void parseConstantBuffers(ID3D12ShaderReflection* reflect, const D3D12_SHADER_DESC& progDesc, const std::shared_ptr<DX12Context>& context)
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

                    auto& cbuffer = context->createConstanBuffer(cbDesc.Name);

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

    void getShaderResources(ID3DBlob* blob, const std::shared_ptr<DX12Context>& context)
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

    void ShaderCompiler::compileShader(IO* io, const ShaderDesc& desc, const std::shared_ptr<DX12Context>& context)
    {
        auto fmt = boost::format("Compiling shader \"%1%\"") % desc.name;

        LOGS_INDENT_START << boost::str(fmt);

        fmt = boost::format("Path: %1%, Entry: %2%") % desc.path % desc.entry;

        LOGS << boost::str(fmt);

        auto buffer = io->loadFile(desc.path);

        ID3DBlob* shaderBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;

        UINT flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

        if (desc.debug)
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        else
            flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

        auto hr = D3DCompile(buffer.c_str(), buffer.size(), desc.path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            desc.entry.c_str(), getDXShaderType(desc.type).c_str(), flags, 0, &shaderBlob, &errorBlob);

        if (FAILED(hr)) {
            if (errorBlob != nullptr) {
                LOGE << "Compilation failed" << std::endl << static_cast<char*>(errorBlob->GetBufferPointer());
                errorBlob->Release();
            }

            if (shaderBlob != nullptr)
                shaderBlob->Release();

            fmt = boost::format("Shader compilation failed: %1%, %2%") % desc.path % GetDXError(hr);
            auto test = boost::str(fmt);
            throw new std::runtime_error(boost::str(fmt));
        }

        D3D12_SHADER_BYTECODE bc;

        bc.BytecodeLength = shaderBlob->GetBufferSize();
        bc.pShaderBytecode = shaderBlob->GetBufferPointer();

        context->addShader(desc.type, desc.name, std::move(bc));

        getShaderResources(shaderBlob, context);

        LOGS_INDENT_END << "Shader done.";
    }

    void parseShaderList(const std::string& data, std::vector<ShaderDesc>& shaderList)
    {
        rapidjson::Document doc;

        LOGS << "Parsing shader list..";
        doc.Parse(data.c_str());

        for (auto iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter) {
            auto name = std::string{ iter->name.GetString() };
            bool allDebug = false;

            if (name == "global") {
                if (name == "debug")
                    allDebug = true;
            } else {
                ShaderDesc shader = {};

                if (name == "vs")
                    shader.type = EShaderType::VERTEX;
                else if (name == "ps")
                    shader.type = EShaderType::PIXEL;

                if (iter->value.IsArray()) {
                    for (auto item = iter->value.Begin(); item != iter->value.End(); ++item) {
                        if (item->IsObject()) {

                            for (auto member = item->MemberBegin(); member != item->MemberEnd(); ++member) {
                                // Parse attribute, not order does not matter
                                name = std::string{ member->name.GetString() };

                                if (name == "name")
                                    shader.name = member->value.GetString();
                                else if (name == "path")
                                    shader.path = member->value.GetString();
                                else if (name == "entry")
                                    shader.entry = member->value.GetString();
                                else if (name == "flags") {
                                    auto type = std::string{ member->value.GetString() };

                                    if (type == "debug")
                                        shader.debug = true;
                                }
                            }

                            shader.debug |= allDebug;
                            shaderList.push_back(std::move(shader));
                        } else {
                            throw new std::runtime_error("Shaderlist expected an object to describe shader");
                        }
                    }
                } else {
                    throw new std::runtime_error("Shaderlist expected an array after type");
                }
            }
        }
    }

    void ShaderCompiler::main(IO* io, const std::shared_ptr<ThreadPool>& threadPool, const std::shared_ptr<DX12Context>& context)
    {
        LOGS_INDENT_START << "Starting shaders compilation..";

        auto buffer = io->loadFile("data/shaderlist.json");
        std::vector<ShaderDesc> shaderList;

        parseShaderList(buffer, shaderList);

        for (auto& shader : shaderList) {
            threadPool->submit(std::bind(&ShaderCompiler::compileShader, io, shader, context));
        }

        LOGS_INDENT_END << "Shader compilation done.";
    }
} // namespace Takoyaki