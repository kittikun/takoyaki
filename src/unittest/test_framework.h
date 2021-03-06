// Copyright(c) 2015-2016 Kitti Vongsay
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

#include <chrono>
#include <fwd.h>
#include <memory>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

#include "test.h"

class TestFramework
{
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    TestFramework(TestFramework&&) = delete;
    TestFramework& operator=(TestFramework&&) = delete;
public:
    TestFramework() noexcept;
    ~TestFramework();

    std::vector<uint8_t> loadFile(const std::wstring&);
    void initialize(Takoyaki::FrameworkDesc&, bool);
    bool process();
    void save(const std::string&);

    using TestDesc = std::tuple<std::shared_ptr<Test>, uint_fast32_t>;
    inline void setTests(std::vector<TestDesc>& descs) { std::swap(descs_, descs); }

private:
    void updateTestResult(Test*, bool, std::chrono::milliseconds);

private:
    std::unique_ptr<Takoyaki::Framework> takoyaki_;
    std::unique_ptr<Takoyaki::Renderer> renderer_;
    std::unique_ptr<Takoyaki::Texture> tex_;
    boost::property_tree::ptree pt_;
    std::vector<TestDesc> descs_;
    std::vector<uint8_t> texCopy_;
    size_t current_;
    bool ciMode_;
};