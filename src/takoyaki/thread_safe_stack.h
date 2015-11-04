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

namespace Takoyaki
{
    template<typename T>
    class ThreadSafeStack
    {
        ThreadSafeStack(const ThreadSafeStack&) = delete;
        ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;
        ThreadSafeStack(ThreadSafeStack&&) = delete;
        ThreadSafeStack& operator=(ThreadSafeStack&&) = delete;

    public:
        using ValueType = typename std::vector<T>::value_type;

        ThreadSafeStack() = default;

        std::unique_lock<std::mutex> back(T& value) const
        {
            std::unique_lock<std::mutex> lock{ mutex_ };

            if (stack_.empty())
                throw std::runtime_error{ "Trying to get front while ThreadSafeStack is empty" };

            value = stack_.back();

            return lock;
        }

        // not thread-safe
        void clear() { stack_.clear(); }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock{ mutex_ };

            return stack_.empty();
        }

        void lock() { mutex_.lock(); }

        void pop(T& value)
        {
            std::lock_guard<std::mutex> lock{ mutex_ };

            if (stack_.empty())
                throw std::runtime_error{ "Trying to pop when ThreadSafeStack is empty" };

            value = std::move(stack_.back());
            stack_.pop_back();
        }

        T& push(T value)
        {
            std::lock_guard<std::mutex> lock{ mutex_ };

            stack_.push_back(std::move(value));

            return stack_.back();
        }

        // not thread-safe
        size_t size() const { return stack_.size(); }

        bool tryPop(T& value)
        {
            std::lock_guard<std::mutex> lock{ mutex_ };

            if (stack_.empty())
                return false;

            value = std::move(stack_.back());
            stack_.pop_back();
        }

        void unlock() { mutex_.unlock(); }

    private:
        mutable std::mutex mutex_;
        std::vector<T> stack_;
    };
} // namespace Takoyaki