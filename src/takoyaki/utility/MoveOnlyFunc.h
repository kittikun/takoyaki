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

        template<typename F>
        struct Impl : ImplBase
        {
            F f;
            Impl(F&& f_) noexcept
                : f(std::move(f_))
            {
            }

            void call() { f(); }
        };

    public:
        MoveOnlyFunc() = default;
        ~MoveOnlyFunc() = default;

        MoveOnlyFunc(MoveOnlyFunc&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlyFunc(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        inline void operator()() { impl->call(); }

        inline MoveOnlyFunc& operator=(MoveOnlyFunc&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

    private:
        std::unique_ptr<ImplBase> impl;
    };

    // Version with void* parameter
    class MoveOnlyFuncParam
    {
        MoveOnlyFuncParam(const MoveOnlyFuncParam&) = delete;
        MoveOnlyFuncParam(MoveOnlyFuncParam&) = delete;
        MoveOnlyFuncParam& operator=(const MoveOnlyFuncParam&) = delete;

        struct ImplBase
        {
            virtual void call(void *) = 0;
            virtual ~ImplBase() {}
        };

        template<typename F>
        struct Impl : ImplBase
        {
            F f;
            Impl(F&& f_) noexcept
                : f(std::move(f_)) {}

            void call(void* param) { f(param); }
        };

    public:
        MoveOnlyFuncParam() = default;
        ~MoveOnlyFuncParam() = default;

        MoveOnlyFuncParam(MoveOnlyFuncParam&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlyFuncParam(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        inline void operator()(void* var) { impl->call(var); }

        inline MoveOnlyFuncParam& operator=(MoveOnlyFuncParam&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

    private:
        std::unique_ptr<ImplBase> impl;
    };

    // Version with two void* parameter
    class MoveOnlyFuncParamTwo
    {
        MoveOnlyFuncParamTwo(const MoveOnlyFuncParamTwo&) = delete;
        MoveOnlyFuncParamTwo(MoveOnlyFuncParamTwo&) = delete;
        MoveOnlyFuncParamTwo& operator=(const MoveOnlyFuncParamTwo&) = delete;

        struct ImplBase
        {
            virtual void call(void*, void*) = 0;
            virtual ~ImplBase() {}
        };

        template<typename F>
        struct Impl : ImplBase
        {
            F f;
            Impl(F&& f_) noexcept
                : f(std::move(f_))
            {
            }

            void call(void* one, void* two) { f(one, two); }
        };

    public:
        MoveOnlyFuncParamTwo() = default;
        ~MoveOnlyFuncParamTwo() = default;

        MoveOnlyFuncParamTwo(MoveOnlyFuncParamTwo&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlyFuncParamTwo(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        inline void operator()(void* one, void* two) { impl->call(one, two); }

        inline MoveOnlyFuncParamTwo& operator=(MoveOnlyFuncParamTwo&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

    private:
        std::unique_ptr<ImplBase> impl;
    };

    // Version with two void* parameter and bool return for failure
    class MoveOnlyFuncParamTwoReturn
    {
        MoveOnlyFuncParamTwoReturn(const MoveOnlyFuncParamTwoReturn&) = delete;
        MoveOnlyFuncParamTwoReturn(MoveOnlyFuncParamTwoReturn&) = delete;
        MoveOnlyFuncParamTwoReturn& operator=(const MoveOnlyFuncParamTwoReturn&) = delete;

        struct ImplBase
        {
            virtual bool call(void*, void*) = 0;
            virtual ~ImplBase() {}
        };

        template<typename F>
        struct Impl : ImplBase
        {
            F f;
            Impl(F&& f_) noexcept
                : f(std::move(f_))
            {
            }

            bool call(void* one, void* two) { return f(one, two); }
        };

    public:
        MoveOnlyFuncParamTwoReturn() = default;
        ~MoveOnlyFuncParamTwoReturn() = default;

        MoveOnlyFuncParamTwoReturn(MoveOnlyFuncParamTwoReturn&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlyFuncParamTwoReturn(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        inline bool operator()(void* one, void* two) { return impl->call(one, two); }

        inline MoveOnlyFuncParamTwoReturn& operator=(MoveOnlyFuncParamTwoReturn&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

    private:
        std::unique_ptr<ImplBase> impl;
    };
} // namespace Takoyaki