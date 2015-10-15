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

#pragma warning(push)
#pragma warning(disable : 4521)

#include <atomic>
#include <boost/any.hpp>

#include "thread_safe_queue.h"
#include "utility/MoveOnlyFunc.h"
#include "utility/log.h"

namespace Takoyaki
{
    struct FrameworkDesc;

    // Allows the threads to be properly joined even if an exception occurred with the thread pool
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
        class IWorker
        {
        public:
            virtual void main(ThreadPool*) = 0;
        };

        ThreadPool() noexcept;
        ~ThreadPool() noexcept;

        using CreateWorkerFunc = std::function<std::unique_ptr<IWorker>()>;

        template<typename WorkerType, typename DescType>
        void initialize(uint_fast32_t threadCount, const DescType& desc)
        {
            auto fmt = boost::format{ "Initializing thread pool with %1% threads" } % threadCount;
            LOGC << boost::str(fmt);

            workers_.reserve(threadCount);

            try {
                for (unsigned i = 0; i < threadCount; ++i) {
                    workers_.push_back(std::make_unique<WorkerType>(desc));

                    auto thread = std::thread{ &IWorker::main, workers_.back().get(), this };

                    fmt = boost::format{ "Takoyaki Worker %1%" } % i;

                    setThreadName(thread.native_handle(), boost::str(fmt));
                    threads.push_back(std::move(thread));
                }
            } catch (...) {
                done_ = true;
                throw new std::runtime_error{ "Could not create ThreadPool" };
            }
        }

        inline bool isDone() const { return done_.load(); }

        template<typename Func>
        void submitGeneric(Func f)
        {
            genericWorkQueue_.push(std::move(f));
        }

        template<typename Func>
        void submitGPU(Func f)
        {
            // specialized submit
            gpuWorkQueue_.push(std::move(f));
        }

        inline bool tryPopGenericTask(MoveOnlyFunc& task) { return genericWorkQueue_.tryPop(task); }
        inline bool tryPopGPUTask(MoveOnlyFuncParam& task) { return gpuWorkQueue_.tryPop(task); }

    private:
        void workerMain();

    private:
        std::atomic<bool> done_;
        std::vector<std::unique_ptr<IWorker>> workers_;
        ThreadSafeQueue<MoveOnlyFunc> genericWorkQueue_;
        ThreadSafeQueue<MoveOnlyFuncParam> gpuWorkQueue_;
        std::vector<std::thread> threads;
        JoinThreads joiner;
    };
} // namespace Takoyaki

#pragma warning(pop) 