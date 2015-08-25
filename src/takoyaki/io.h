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

#pragma once

#include <functional>
#include <unordered_map>

#include "public/definitions.h"

namespace Takoyaki
{
    struct FrameworkDesc;

    class IO
    {
        IO(const IO&) = delete;
        IO& operator=(const IO&) = delete;
        IO(IO&&) = delete;
        IO& operator=(IO&&) = delete;

    public:
        using LoadResultFunc = std::function<void(const std::vector<uint8_t>&)>;

        IO();
        ~IO();

        void initialize(const FrameworkDesc&);

        void loadAsyncFile(const std::wstring&, const LoadResultFunc&);

        void loadAsyncFileResult(const std::wstring&, const std::vector<uint8_t>&);
    private:
        std::unordered_map<std::wstring, LoadResultFunc> mapQueued_;
        LoadFileAsyncFunc loadFileAsyncFunc_;
    };
} // namespace Takoyaki