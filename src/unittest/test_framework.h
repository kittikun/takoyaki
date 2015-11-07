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

#pragma once

#include <fwd.h>
#include <memory>
#include <string>
#include <vector>

#include "test.h"

class TestFramework
{
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    TestFramework(TestFramework&&) = delete;
    TestFramework& operator=(TestFramework&&) = delete;
public:
    TestFramework() noexcept;

    void loadAsync(const std::wstring&);
    void initialize(Takoyaki::FrameworkDesc&);
    void process();

    inline void setTests(std::vector<std::shared_ptr<Test>>& tests) { std::swap(tests_, tests); }

private:
    std::unique_ptr<Takoyaki::Framework> takoyaki_;
    std::unique_ptr<Takoyaki::Renderer> renderer_;
    std::vector<std::shared_ptr<Test>> tests_;

    std::vector<uint8_t> texCopy_;
    std::unique_ptr<Takoyaki::Texture> tex_;
};