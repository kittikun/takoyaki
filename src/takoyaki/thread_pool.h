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

#pragma warning(push)
#pragma warning(disable : 4521)

#include <atomic>
#include <boost/any.hpp>

#include "thread_safe_queue.h"
#include "utility/MoveOnlyFunc.h"

namespace Takoyaki
{
    struct FrameworkDesc;

    // Allows the threads to be properly joined even if an exception occured with the thread pool
    class JoinThreads
    {
        std::vector<std::thread>& threads;
    public:
        explicit JoinThreads(std::vector<std::thread>& threads_)
            : threads{ threads_ }
        {
        }

        ~JoinThreads()
        {
            for (unsigned long i = 0; i < threads.size(); ++i) {
                if (threads[i].joinable())
                    threads[i].join();
            }
        }
    };

    class ThreadPool
    {
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

    public:
        struct SpecializedWorkerDesc
        {
            std::function<void()> mainFunc;
            std::function<void(MoveOnlyFuncParamReturn)> submitFunc;
            std::string name;
            uint_fast32_t id;
        };

        ThreadPool() noexcept;
        ~ThreadPool() noexcept;

        void addSpecializedWorker(const SpecializedWorkerDesc&);
        void initialize(uint_fast32_t);

        template<typename Func>
        void submit(Func f)
        {
            // generic submit
            workQueue_.push(std::move(f));
        }

        template<typename Func>
        void submit(uint32_t specialId, Func f)
        {
            // specialized submit
            specializedWorkers_[specialId](std::move(f));
        }

        // for specialized workers
        inline bool tryPopWork(MoveOnlyFunc& func) { return workQueue_.tryPop(func); }

    private:
        void workerMain();

    private:
        std::atomic<bool> done_;
        ThreadSafeQueue<MoveOnlyFunc> workQueue_;
        std::vector<std::thread> threads;
        JoinThreads joiner;

        // map to specialized worker submit function
        std::unordered_map<uint_fast32_t, std::function<void(MoveOnlyFuncParamReturn)>> specializedWorkers_;
    };
} // namespace Takoyaki

#pragma warning(pop) 