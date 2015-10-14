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
        //std::function<void()> operator()(void* var);

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
        //std::function<void()> operator()(void* var);

        inline MoveOnlyFuncParamTwo& operator=(MoveOnlyFuncParamTwo&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

    private:
        std::unique_ptr<ImplBase> impl;
    };

    // Version with void* param and return value for specialized workers
    using VoidReturnFunc = std::function<void()>;

    class MoveOnlyFuncParamReturn
    {
        MoveOnlyFuncParamReturn(const MoveOnlyFuncParamReturn&) = delete;
        MoveOnlyFuncParamReturn(MoveOnlyFuncParamReturn&) = delete;
        MoveOnlyFuncParamReturn& operator=(const MoveOnlyFuncParamReturn&) = delete;

        struct ImplBase
        {
            virtual VoidReturnFunc call(void *) = 0;
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

            VoidReturnFunc call(void* param) { return f(param); }
        };

    public:
        MoveOnlyFuncParamReturn() = default;

        MoveOnlyFuncParamReturn(MoveOnlyFuncParamReturn&& other) :
            impl(std::move(other.impl))
        {
        }

        template<typename F>
        MoveOnlyFuncParamReturn(F&& f)
            : impl(new Impl<F>(std::move(f)))
        {
        }

        inline VoidReturnFunc operator()(void* var) { return impl->call(var); }

        inline MoveOnlyFuncParamReturn& operator=(MoveOnlyFuncParamReturn&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

    private:
        std::unique_ptr<ImplBase> impl;
    };

    //template<typename ReturnType, typename ParamType>
    //class MoveOnlyFuncImpl
    //{
    //    MoveOnlyFuncImpl(const MoveOnlyFuncImpl&) = delete;
    //    MoveOnlyFuncImpl(MoveOnlyFuncImpl&) = delete;
    //    MoveOnlyFuncImpl& operator=(const MoveOnlyFuncImpl&) = delete;

    //    struct ImplBase
    //    {
    //        virtual ReturnType call(ParamType) = 0;
    //        virtual ~ImplBase() {}
    //    };

    //    template<typename F>
    //    struct Impl : ImplBase
    //    {
    //        F f;
    //        Impl(F&& f_) noexcept
    //            : f(std::move(f_))
    //        {
    //        }

    //        ReturnType call(ParamType);
    //    };


    //public:
    //    MoveOnlyFuncImpl() = default;

    //    MoveOnlyFuncImpl(MoveOnlyFuncImpl&& other) noexcept
    //        : impl(std::move(other.impl))
    //    {
    //    }

    //    template<typename F>
    //    MoveOnlyFuncImpl(F&& f) noexcept
    //        : impl(new Impl<F>(std::move(f)))
    //    {
    //    }

    //    ReturnType operator()(ParamType);

    //    inline MoveOnlyFuncImpl<ReturnType, ParamType>& operator=(MoveOnlyFuncImpl<ReturnType, ParamType>&& other) noexcept
    //    {
    //        impl = std::move(other.impl);
    //        return *this;
    //    }

    //private:
    //    std::unique_ptr<ImplBase> impl;
    //};

    //// MoveOnlyFunc specialization
    //template <>
    //template<typename F>
    //struct MoveOnlyFuncImpl<void, void>::Impl
    //{
    //    void call() { f(); }
    //};

    //template <>
    //inline void MoveOnlyFuncImpl<void, void>::operator()() { impl->call(); }

    //// MoveOnlyFuncParam specialization
    //template <>
    //template<typename F>
    //struct MoveOnlyFuncImpl<void, void*>::Impl
    //{
    //    void call(void* p) { f(p); }
    //};

    //template <>
    //inline void MoveOnlyFuncImpl<void, void*>::operator()(void* p) { impl->call(p); }

    //// MoveOnlyFuncParamReturn specialization
    //using VoidReturnFunc = std::function<void()>;

    //template <>
    //template<typename F>
    //struct MoveOnlyFuncImpl<VoidReturnFunc, void*>::Impl
    //{
    //    VoidReturnFunc call(void* p) { return f(p); }
    //};

    //template <>
    //inline VoidReturnFunc MoveOnlyFuncImpl<VoidReturnFunc, void*>::operator()(void* p) { return impl->call(p); }


    //// aliases
    //using MoveOnlyFunc = MoveOnlyFuncImpl<void, void>;
    //using MoveOnlyFuncParam = MoveOnlyFuncImpl<void, void*>;
    //using MoveOnlyFuncParamReturn = MoveOnlyFuncImpl<VoidReturnFunc, void*>;
} // namespace Takoyaki