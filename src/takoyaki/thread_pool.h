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
#include <future>
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
            virtual bool isIdle() = 0;
            virtual void main() = 0;
            virtual void submitCommandList() = 0;
        };

        ThreadPool() noexcept;
        ~ThreadPool() noexcept;

        using GPUDrawFunc = std::pair<std::string, MoveOnlyFuncParamTwoReturn>;
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

                    auto thread = std::thread{ &IWorker::main, workers_.back().get() };

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
        void submitGeneric(Func f, uint_fast32_t target)
        {
            genericWorkQueues_[target].push(std::move(f));
        }

        template<typename Func>
        void submitGPU(Func f, uint_fast32_t target)
        {
            // specialized submit
            gpuWorkQueues_[target].push(std::move(f));
        }

        template<typename Func>
        void submitGPUDraw(Func f, const std::string& pipelineState, uint_fast32_t target)
        {
            // specialized submit
            gpuDrawQueues_[target].push(std::make_pair(pipelineState, std::move(f)));
        }


        //template<typename Func>
        //std::future<std::result_of_t<Func()>> submitGPUWithResult(Func f)
        //{
        //    std::packaged_task<std::result_of_t<Func()>> task(std::move(f));
        //    std::future<std::result_of_t<Func()>> res(task.get_future());

        //    gpuWorkQueue_.push(std::move(task));
        //    return res;
        //}

        void submitGPUCommandLists();
        void swapQueues();

        inline bool tryPopGenericTask(MoveOnlyFunc& task) { return genericWorkQueues_[0].tryPop(task); }
        inline bool tryPopGPUTask(MoveOnlyFuncParamTwoReturn& task) { return gpuWorkQueues_[0].tryPop(task); }
        inline bool tryPopGPUDrawTask(GPUDrawFunc& task) { return gpuDrawQueues_[0].tryPop(task); }

    private:
        void workerMain();

    private:
        std::atomic<bool> done_;
        std::vector<std::unique_ptr<IWorker>> workers_;
        std::array<ThreadSafeQueue<MoveOnlyFunc>, 3> genericWorkQueues_;
        std::array<ThreadSafeQueue<MoveOnlyFuncParamTwoReturn>, 3> gpuWorkQueues_;
        std::array<ThreadSafeQueue<GPUDrawFunc>, 3> gpuDrawQueues_;
        std::vector<std::thread> threads;
        JoinThreads joiner;
    };
} // namespace Takoyaki

#pragma warning(pop) 