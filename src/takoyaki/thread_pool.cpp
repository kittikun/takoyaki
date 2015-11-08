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

#include "pch.h"
#include "thread_pool.h"

#include "utility/win_utility.h"

namespace Takoyaki
{
    ThreadPool::ThreadPool(uint_fast32_t numWorkers) noexcept
        : status_{ TP_NONE }
        , numWorkers_{ numWorkers }
        , joiner{ threads_ }
        , latch_{ numWorkers }
    {
    }

    ThreadPool::~ThreadPool() noexcept
    {
        barrier();
        status_ = TP_DONE;
        cond_.notify_all();
    }

    void ThreadPool::barrier()
    {
        status_.store(TP_BARRIER);
        latch_.wait();
        latch_.reset(numWorkers_);
    }

    void ThreadPool::clear()
    {
        barrier();

        for (auto& queue : genericWorkQueues_)
            queue.clear();

        for (auto& queue : gpuQueues_)
            queue.clear();

        for (auto& worker : workers_)
            worker->clear();
    }

    void ThreadPool::submitGPUCommandLists()
    {
        for (auto& worker : workers_)
            worker->submitCommandList();
    }

    void ThreadPool::swapQueues()
    {
        barrier();

        genericWorkQueues_[0].swap(genericWorkQueues_[1]);
        genericWorkQueues_[1].swap(genericWorkQueues_[2]);
        gpuQueues_[0].swap(gpuQueues_[1]);
        gpuQueues_[1].swap(gpuQueues_[2]);
        status_.store(TP_RUNNING);
        cond_.notify_all();
    }
} // namespace Takoyaki