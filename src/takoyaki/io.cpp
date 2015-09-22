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

#include <atomic>

#include "public/definitions.h"
#include "public/framework.h"
#include "utility/win_utility.h"

namespace Takoyaki
{
    IO::IO() = default;
    IO::~IO() = default;

    void IO::initialize(const LoadFileAsyncFunc& func)
    {
        loadFileAsyncFunc_ = func;
    }

    std::string IO::loadFile(const std::string& path)
    {
        std::atomic<int> flag{ 0 };
        std::string buffer;

        loadAsyncFile(path, [&buffer, &flag](const std::vector<uint8_t>& data)
        {
            // copy data or it will expire after this scope
            buffer = std::string(data.begin(), data.end()).c_str();
            flag.store(1);
        });

        // we can afford to block this thread
        while (flag.load() == 0) {
            std::this_thread::yield();
        }

        return buffer;
    }

    void IO::loadAsyncFile(const std::string& filename, const LoadResultFunc& func)
    {
        size_t initial;

        // only protect access to the map
        {
            auto lock = mapQueued_.getWriteLock();
            initial = mapQueued_[filename].size();
            mapQueued_[filename].push_back(func);
        }

        if (initial == 0)
            loadFileAsyncFunc_(makeWinPath(filename));
    }

    void IO::loadAsyncFileResult(const std::string& filename, const std::vector<uint8_t>& res)
    {
        auto lock = mapQueued_.getWriteLock();
        auto found = mapQueued_.find(filename);

        if (found != mapQueued_.end()) {
            auto funcs = found->second;
            mapQueued_.erase(found);
            lock.unlock();

            for (auto func : funcs)
                func(res);
        } else {
            auto fmt = boost::format("IO::loadAsyncFileResult, key not found %1%") % filename;

            throw new std::runtime_error(boost::str(fmt));
        }
    }
} // namespace Takoyaki