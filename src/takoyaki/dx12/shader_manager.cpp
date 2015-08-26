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

#include <boost/format.hpp>

#include <D3Dcompiler.h>
#include <rapidjson/document.h>

#include "../io.h"
#include "../utility/log.h"

namespace Takoyaki
{
    ShaderManager::ShaderManager() = default;

    ShaderManager::~ShaderManager()
    {
    }

    void ShaderManager::loadCallback(const std::vector<uint8_t>& data)
    {
        // data will expire when we leave this function so pass a copy
        p.set_value(data);
    }

    void ShaderManager::test(std::future<std::vector<uint8_t>> data)
    {
        auto& res = data.get();
        rapidjson::Document doc;

        std::string test(res.begin(), res.end());

        doc.Parse(test.c_str());

        for (auto iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter) {
            LOGC << iter->name.GetString();

            if (iter->value.IsArray()) {
                for (auto item = iter->value.Begin(); item != iter->value.End(); ++item) {
                    if (item->IsObject()) {
                        for (auto member = item->MemberBegin(); member != item->MemberEnd(); ++member)
                            if (member->value.IsString())
                                LOGC << member->name.GetString() << ", " << member->value.GetString();
                    }
                }
            }
        }
    }

    void ShaderManager::initialize(std::weak_ptr<IO> io)
    {
        compilerThread_ = std::thread(std::bind(&ShaderManager::test, this, std::placeholders::_1), p.get_future());

        //detach the thread so it will not try to be destroyed at manager destruction
        compilerThread_.detach();


        auto lock = io.lock();

        lock->loadAsyncFile(L"data\\shaderlist.json", std::bind(&ShaderManager::loadCallback, this, std::placeholders::_1));
    }
} // namespace Takoyaki