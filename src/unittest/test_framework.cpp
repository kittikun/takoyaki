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

#include <takoyaki.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>

TestFramework::TestFramework() noexcept
    : takoyaki_{ std::make_unique<Takoyaki::Framework>() }
{

}

void TestFramework::initialize(Takoyaki::FrameworkDesc& desc)
{
    desc.loadAsyncFunc = std::bind(&TestFramework::loadAsync, this, std::placeholders::_1);

    takoyaki_->initialize(desc);

    renderer_ = takoyaki_->getRenderer();

    // test need to load various resources so better start tasks before main loop
    for (auto& test : tests_)
        test->initialize(takoyaki_.get());
}

void TestFramework::loadAsync(const std::wstring& filename)
{
    // async read file using c++17 N3634 improvements to std::future via boost

    // the actual reading happens here
    boost::future<std::vector<uint8_t>> f1 = boost::async([&filename]()
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
            throw new std::runtime_error("Could not open file");
        }

        return res;
    });

    // once the reading is finished, notify framework
    // the two get below won't block anymore thank to .then
    boost::future<void> f2 = f1.then([this, &filename](boost::future<std::vector<uint8_t>> f)
    {
        takoyaki_->loadAsyncFileResult(filename, f.get());
    });

    f2.get();
}

void TestFramework::process()
{
    tests_[0]->update(renderer_.get());
    tests_[0]->render(renderer_.get());
    takoyaki_->present();
}