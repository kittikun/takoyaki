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
        , joiner{ threads }
    {
    }

    ThreadPool::~ThreadPool() noexcept
    {
        done_ = true;
    }

    std::vector<std::unique_lock<std::mutex>> ThreadPool::lockQueues()
    {
        std::vector<std::unique_lock<std::mutex>> locks;

        locks.reserve(genericWorkQueues_.size() + gpuWorkQueues_.size());

        for (auto& gen : genericWorkQueues_)
            locks.push_back(gen.getLock());

        for (auto& gpu : gpuWorkQueues_)
            locks.push_back(gpu.getLock());

        return locks;
    }

    void ThreadPool::submitGPUCommandLists()
    {
        for (auto& worker : workers_)
            worker->submitCommandList();
    }

    void ThreadPool::swapQueues()
    {
        //LOGC << "ThreadPool::swapQueues";

        // Barrier
        {
            bool done = false;

            while (!done) {
                bool queueDone = genericWorkQueues_[0].empty() && gpuWorkQueues_[0].empty();
                bool workerDone = true;

                for (auto& worker : workers_)
                    workerDone &= worker->isIdle();

                done = queueDone & workerDone;

                if (!done)
                    std::this_thread::yield();
            }
        }

        auto locks = lockQueues();

        genericWorkQueues_[0].swap(genericWorkQueues_[1]);
        genericWorkQueues_[1].swap(genericWorkQueues_[2]);
        gpuWorkQueues_[0].swap(gpuWorkQueues_[1]);
        gpuWorkQueues_[1].swap(gpuWorkQueues_[2]);
    }
} // namespace Takoyaki