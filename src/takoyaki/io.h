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

#include "rwlock_map.h"
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
        ~IO() = default;

        void initialize(const LoadFileAsyncFunc&);

        // Blocking load a file
        std::string loadFile(const std::string&);

        // Asynchronously load a file
        void loadAsyncFile(const std::string&, const LoadResultFunc&);

        // called by framework to return the async result by OS
        void loadAsyncFileResult(const std::string&, const std::vector<uint8_t>&);
    private:
        std::mutex mutex_;
        RWLockMap<std::string, std::vector<LoadResultFunc>> mapQueued_;
        LoadFileAsyncFunc loadFileAsyncFunc_;
    };
} // namespace Takoyaki