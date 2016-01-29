// Copyright(c) 2015-2016 Kitti Vongsay
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
#include <boost/thread/latch.hpp>

#include "thread_safe_queue.h"
#include "utility/MoveOnlyFunc.h"
#include "utility/log.h"

namespace Takoyaki
{
    struct FrameworkDesc;

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
            virtual ~IWorker() = default;
            virtual void clear() = 0;
            virtual void main() = 0;
            virtual void submitCommandList() = 0;
        };

        enum Status
        {
            TP_DONE,
            TP_NONE,
            TP_BARRIER,
            TP_RUNNING,
        };

        using GPUDrawFunc = std::pair<std::string, MoveOnlyFuncParamTwoReturn>;
        using CreateWorkerFunc = std::function<std::unique_ptr<IWorker>()>;

        ThreadPool(uint_fast32_t) noexcept;
        ~ThreadPool() noexcept;

        // clear all pending tasks and also clear workers gpu command queue
        void clear();

        template<typename WorkerType, typename DescType>
        void initialize(const DescType& desc)
        {
            auto fmt = boost::format{ "Initializing thread pool with %1% threads" } % numWorkers_;
            LOGC << boost::str(fmt);

            workers_.reserve(numWorkers_);

            try {
                for (unsigned i = 0; i < numWorkers_; ++i) {
                    workers_.push_back(std::make_unique<WorkerType>(desc, latch_, cond_));

                    auto thread = std::thread{ &IWorker::main, workers_.back().get() };

                    fmt = boost::format{ "Takoyaki Worker %1%" } % i;

                    setThreadName(thread.native_handle(), boost::str(fmt));
                    threads_.push_back(std::move(thread));
                }
                status_ = TP_RUNNING;
            } catch (...) {
                status_ = TP_DONE;
                throw std::runtime_error{ "Could not create ThreadPool" };
            }
        }

        inline uint_fast32_t getStatus() const { return status_; }

        template<typename Func>
        void submitGeneric(Func f, uint_fast32_t target)
        {
            genericWorkQueues_[target].push(std::move(f));
        }

        template<typename Func>
        void submitGPU(Func f, const std::string& pipelineState, uint_fast32_t target)
        {
            // specialized submit
            gpuQueues_[target].push(std::make_pair(pipelineState, std::move(f)));
        }

        void resume();
        void submitGPUCommandLists();
        void swapQueues();

        inline bool tryPopGenericTask(MoveOnlyFunc& task) { return genericWorkQueues_[0].tryPop(task); }
        inline bool tryPopGPUTask(GPUDrawFunc& task) { return gpuQueues_[0].tryPop(task); }

    private:
        void barrier();
        void workerMain();

    private:
        std::atomic<uint_fast32_t> status_;
        uint_fast32_t numWorkers_;
        std::vector<std::unique_ptr<IWorker>> workers_;
        std::array<ThreadSafeQueue<MoveOnlyFunc>, 3> genericWorkQueues_;
        std::array<ThreadSafeQueue<GPUDrawFunc>, 3> gpuQueues_;
        std::vector<std::thread> threads_;

        // latch is to wait for workers to finish executing jobs
        // condition_variable is to tell them to resume work
        boost::latch latch_;
        std::condition_variable cond_;
    };
} // namespace Takoyaki

#pragma warning(pop) 