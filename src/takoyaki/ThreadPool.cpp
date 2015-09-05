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
#include "ThreadPool.h"

#include "utility/winUtility.h"

namespace Takoyaki
{
    ThreadPool::ThreadPool()
        : done_(false)
        , joiner(threads)
    {
        auto threadCount = std::thread::hardware_concurrency();
        try {
            for (unsigned i = 0; i < threadCount; ++i) {
                auto thread = std::thread(&ThreadPool::workerMain, this);
                auto fmt = boost::format("Worker thread %1%") % i;

                setThreadName(thread.native_handle(), boost::str(fmt));
                threads.push_back(std::move(thread));
            }
        } catch (...) {
            done_ = true;
            throw new std::runtime_error("Could not create ThreadPool");
        }
    }

    ThreadPool::~ThreadPool()
    {
        done_ = true;
    }

    void ThreadPool::workerMain()
    {
        while (!done_) {
            MoveOnlyFunc task;

            if (workQueue_.tryPop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }
} // namespace Takoyaki