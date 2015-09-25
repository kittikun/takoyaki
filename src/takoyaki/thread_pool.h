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

#include <future>

#include "thread_safe_queue.h"

namespace Takoyaki
{
    class MoveOnlyFunc;
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

    // Wrapper to allow function pointers to be stored in a template container
    class MoveOnlyFunc
    {
        MoveOnlyFunc(const MoveOnlyFunc&) = delete;
        MoveOnlyFunc(MoveOnlyFunc&) = delete;
        MoveOnlyFunc& operator=(const MoveOnlyFunc&) = delete;

        struct ImplBase
        {
            virtual void call() = 0;
            virtual ~ImplBase() {}
        };
        std::unique_ptr<ImplBase> impl;

        template<typename F>
        struct Impl : ImplBase
        {
            F f;
            Impl(F&& f_) : f(std::move(f_)) {}
            void call() { f(); }
        };

    public:
        MoveOnlyFunc() = default;

        MoveOnlyFunc(MoveOnlyFunc&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlyFunc(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        void operator()() { impl->call(); }

        MoveOnlyFunc& operator=(MoveOnlyFunc&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }
    };

    class ThreadPool
    {
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

    public:
        struct SpecializedWorker
        {
            std::function<void()> func;
            std::string name;
        };

        ThreadPool() noexcept;
        ~ThreadPool() noexcept;

        void addWorkerFunc(const SpecializedWorker&);
        void initialize(uint_fast32_t);

        template<typename Func>
        std::future<std::result_of_t<Func()>> submitWithResult(Func f)
        {
            std::packaged_task<std::result_of_t<Func()>> task(std::move(f));
            std::future<std::result_of_t<Func()>> res(task.get_future());

            workQueue_.push(std::move(task));
            return res;
        }

        template<typename Func>
        void submit(Func f)
        {
            workQueue_.push(std::move(f));
        }

    private:
        void workerMain();

    private:
        std::atomic<bool> done_;
        ThreadSafeQueue<MoveOnlyFunc> workQueue_;
        std::vector<std::thread> threads;
        std::vector<std::function<void()>> specializedWorkers_;
        JoinThreads joiner;
    };
} // namespace Takoyaki

#pragma warning(pop) 