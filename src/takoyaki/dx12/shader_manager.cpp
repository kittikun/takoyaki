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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>

#include "../io.h"
#include "../utility/log.h"

namespace Takoyaki
{
    ShaderManager::ShaderManager() = default;
    ShaderManager::~ShaderManager() = default;

    void ShaderManager::initialize(IO* io)
    {
        io->loadAsyncFile(L"data\\shaders.xml", std::bind(&ShaderManager::loadAsyncCallback, this, std::placeholders::_1));
    }

    void ShaderManager::loadAsyncCallback(const std::vector<uint8_t>& data)
    {
        boost::property_tree::ptree tree;

        boost::interprocess::bufferstream stream(reinterpret_cast<char*>(const_cast<uint8_t*>(&data.front())), buffer.size());
        boost::property_tree::read_xml(stream, tree);

        auto name = tree.get<std::string>("program.<xmlattr>.name");
        auto vertex = tree.get<std::string>("program.vertex.<xmlattr>.path"); 
        auto pixel = tree.get<std::string>("program.pixel.<xmlattr>.path");
    }
} // namespace Takoyaki