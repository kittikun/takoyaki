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

#include <atomic>
#include <D3Dcompiler.h>
#include <boost/format.hpp>
#include <rapidjson/document.h>

#include "DXUtility.h"
#include "../io.h"
#include "../utility/log.h"
#include "../utility/winUtility.h"

namespace Takoyaki
{
    ShaderManager::ShaderManager() = default;
    ShaderManager::~ShaderManager() = default;

    void ShaderManager::compilerMain(IO* io)
    {
        std::atomic<int> flag{0};
        std::string buffer;

        auto loadWait = [&buffer, &flag, &io](const std::string& path)
        {
            flag.store(0);
            io->loadAsyncFile(path, [&buffer, &flag](const std::vector<uint8_t>& data)
            {
                // copy data or it will expire after this scope
                buffer = std::string(data.begin(), data.end()).c_str();
                flag.store(1);
            });

            // we can afford to block this thread
            while (flag.load() == 0) {
                std::this_thread::yield();
            }
        };

        LOGS_INDENT_START << "Starting shaders compilation..";

        loadWait("data/shaderlist.json");

        auto programList = parseShaderList(buffer);

        // Compile shaders, might want to move this to a function so it can be called outside of framework ?
        for (auto program : programList) {
            auto fmt = boost::format("Compiling program \"%1%\"") % program.name;
            LOGS_INDENT_START << boost::str(fmt);

            for (auto shader : program.shaders) {
                auto fmt = boost::format("Path: %1%, Type: %2%, Entry: %3%") % shader.path % shader.type % shader.main;

                LOGS << boost::str(fmt);

                loadWait(shader.path);

                ID3DBlob* shaderBlob = nullptr;
                ID3DBlob* errorBlob = nullptr;

                auto hr = D3DCompile(buffer.c_str(), buffer.size(), shader.path.c_str(), nullptr, nullptr, shader.main.c_str(), shader.type.c_str(), shader.flags, 0, &shaderBlob, &errorBlob);

                if (FAILED(hr)) {
                    if (errorBlob != nullptr) {
                        LOGE << "Compilation failed" << std::endl << static_cast<char*>(errorBlob->GetBufferPointer());
                        errorBlob->Release();
                    }

                    if (shaderBlob != nullptr)
                        shaderBlob->Release();

                    fmt = boost::format("Shader compilation failed: %1%") % shader.path;

                    throw new std::runtime_error(boost::str(fmt));
                }
            }

            LOGS_INDENT_END << "Program done.";
        }

        LOGS_INDENT_END << "Shaders compilation done.";
    }

    void ShaderManager::initialize(IO* io)
    {
        // shaders are processed in a different thread
        auto thread = std::thread{ std::bind(&ShaderManager::compilerMain, this, std::placeholders::_1), io };

        setThreadName(thread.native_handle(), "Shader Compiler");

        //we don't need to track this one so let it run in the background
        thread.detach();
    }

    std::vector<ShaderManager::ProgramDesc> ShaderManager::parseShaderList(const std::string& data)
    {
        rapidjson::Document doc;
        std::vector<ProgramDesc> programList;

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

        return programList;
    }
} // namespace Takoyaki