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

namespace Takoyaki
{
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
            Impl(F&& f_) noexcept
                : f(std::move(f_)) {}
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

    // Version with parameters for specialized workers
    class MoveOnlySpecializedFunc
    {
        MoveOnlySpecializedFunc(const MoveOnlySpecializedFunc&) = delete;
        MoveOnlySpecializedFunc(MoveOnlySpecializedFunc&) = delete;
        MoveOnlySpecializedFunc& operator=(const MoveOnlySpecializedFunc&) = delete;

        struct ImplBase
        {
            virtual void call(void *) = 0;
            virtual ~ImplBase() {}
        };
        std::unique_ptr<ImplBase> impl;

        template<typename F>
        struct Impl : ImplBase
        {
            F f;
            Impl(F&& f_) noexcept
                : f(std::move(f_)) {}

            void call(void* param) { f(param); }
        };

    public:
        MoveOnlySpecializedFunc() = default;

        MoveOnlySpecializedFunc(MoveOnlySpecializedFunc&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlySpecializedFunc(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        void operator()(void* var) { impl->call(var); }

        MoveOnlySpecializedFunc& operator=(MoveOnlySpecializedFunc&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }
    };
} // namespace Takoyaki