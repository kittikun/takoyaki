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
#include "io.h"

#include "public/definitions.h"
#include "public/framework.h"
#include "utility/winUtility.h"

namespace Takoyaki
{
    IO::IO() = default;
    IO::~IO() = default;

    void IO::initialize(const FrameworkDesc& desc)
    {
        if (!desc.loadAsyncFunc)
            throw new std::runtime_error("FrameworkDesc missing LoadFileAsyncFunc");

        loadFileAsyncFunc_ = desc.loadAsyncFunc;
    }

    void IO::loadAsyncFile(const std::string& filename, const LoadResultFunc& func)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        mapQueued_.insert(std::make_pair(filename, func));
        loadFileAsyncFunc_(makeWinPath(filename));
    }

    void IO::loadAsyncFileResult(const std::string& filename, const std::vector<uint8_t>& res)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto found = mapQueued_.find(filename);

        if (found != mapQueued_.end()) {
            found->second(res);
            mapQueued_.erase(found);
        } else {
            throw new std::runtime_error("IO::loadAsyncFileResult, key not found");
        }
    }
} // namespace Takoyaki