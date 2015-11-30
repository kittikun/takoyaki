// Copyright(c) 2015 Kitti Vongsay
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

#include "test_framework.h"

#include <iostream>
#include <takoyaki.h>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/xml_parser.hpp>

TestFramework::TestFramework() noexcept
    : takoyaki_{ std::make_unique<Takoyaki::Framework>() }
    , current_{ 0 }
    , ciMode_{ false }
{
}

TestFramework::~TestFramework()
{
    takoyaki_->terminate();
}

void TestFramework::initialize(Takoyaki::FrameworkDesc& desc, bool ciMode)
{
    ciMode_ = ciMode;

    takoyaki_->initialize(desc);

    renderer_ = takoyaki_->getRenderer();

    // render target for off-line rendering
    Takoyaki::TextureDesc rtDesc;

    rtDesc.format = Takoyaki::EFormat::B8G8R8A8_UNORM;
    rtDesc.width = (uint_fast32_t)desc.windowSize.x;
    rtDesc.height = (uint_fast32_t)desc.windowSize.y;
    rtDesc.usage = Takoyaki::EUsageType::GPU_ONLY;
    rtDesc.flags = Takoyaki::RF_RENDERTARGET;

    // texture used to copy the render target to make a checksum
    Takoyaki::TextureDesc texDesc;

    texDesc.format = Takoyaki::EFormat::B8G8R8A8_UNORM;
    texDesc.width = (uint_fast32_t)desc.windowSize.x;
    texDesc.height = (uint_fast32_t)desc.windowSize.y;
    texDesc.usage = Takoyaki::EUsageType::CPU_READ;

    tex_ = renderer_->createTexture(texDesc);

    // cpu buffer to copy the texture into so we can checksum it
    texCopy_.resize(tex_->getSizeByte());

    auto fmt = boost::format("Processing %1% tests..") % descs_.size();

    std::cout << boost::str(fmt) << std::endl;

    // tests need to load various resources so better start tasks before main loop
    for (auto& desc : descs_)
        std::get<0>(desc)->initialize(takoyaki_.get());

    // wait for gpu to finish any tasks created by initialization
    takoyaki_->present();
}

std::vector<uint8_t> TestFramework::loadFile(const std::wstring& filename)
{
    std::vector<uint8_t> res;
    auto basePath(boost::filesystem::current_path());

    if (IsDebuggerPresent())
        basePath = boost::filesystem::system_complete("..");

    auto path = basePath / filename;
    boost::filesystem::ifstream stream;

    stream.open(path);

    if (stream) {
        // get length of file
        stream.seekg(0, stream.end);

        int length = (int)stream.tellg();

        stream.seekg(0, stream.beg);
        res.resize(length);
        stream.read(reinterpret_cast<char*>(&res.front()), length);
        stream.close();
    } else {
        throw std::runtime_error("Could not open file");
    }

    return res;
}

bool TestFramework::process()
{
    auto test = std::get<0>(descs_[current_]);
    auto start = std::chrono::high_resolution_clock::now();

    // render the test
    test->update(renderer_.get());
    test->render(renderer_.get(), tex_->getHandle());
    takoyaki_->present();

    tex_->read(&texCopy_.front(), (uint_fast32_t)texCopy_.size());

    auto end = std::chrono::high_resolution_clock::now();

    // compare checksums
    boost::crc_32_type crc;

    crc.process_bytes(&texCopy_.front(), texCopy_.size());

    // update results
    auto outcome = crc.checksum() == std::get<1>(descs_[current_]);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    updateTestResult(test.get(), outcome, duration);

    ++current_;

    return current_ < descs_.size();
}

void TestFramework::save(const std::string& filename)
{
    auto basePath(boost::filesystem::current_path());

    if (IsDebuggerPresent())
        basePath = boost::filesystem::system_complete("..");

    auto path = basePath / filename;

    boost::property_tree::write_xml(path.generic_string(), pt_);
}

void TestFramework::updateTestResult(Test* test, bool result, std::chrono::milliseconds duration)
{
    boost::property_tree::ptree res;

    res.add("test.name", test->getName());

    std::string outcome;

    if (result)
        outcome = "Passed";
    else
        outcome = "Failed";

    res.add("test.outcome", outcome);
    res.add("test.duration", duration.count());
    pt_.add_child("tests", res);

    // also output some text to the console
    auto fmt = boost::format("[%1%] %2%, %3%") % current_ % test->getName() % ((result) ? "Passed" : "Failed");

    std::cout << boost::str(fmt) << std::endl;
}