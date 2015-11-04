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
    ThreadPool::ThreadPool() noexcept
        : done_{ false }
        , joiner{ threads_ }
    {
    }

    ThreadPool::~ThreadPool() noexcept
    {
        done_ = true;
    }

    void ThreadPool::barrier()
    {
        bool done = false;

        while (!done) {
            bool queueDone = genericWorkQueues_[0].empty() && gpuQueues_[0].empty();
            bool workerDone = false;

            if (queueDone) {
                workerDone = true;

                for (auto& worker : workers_)
                    workerDone &= worker->isIdle();
            }

            done = queueDone & workerDone;

            // yield while we wait for the workers to finish working
            if (!done)
                std::this_thread::yield();
        }
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
    }
} // namespace Takoyaki