// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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

#include "framework_impl.h"

#include <boost/format.hpp>

#include "../utility/error.h"
#include "../utility/log.h"

namespace Takoyaki
{
    FrameworkImpl::FrameworkImpl()
        : d3dDevice_(nullptr)
        , d3dContext_(nullptr)
        , rtv_(nullptr)
        , hInst_(nullptr)
        , hWnd_(nullptr)
    {
        Log::Initialize();
    }

    FrameworkImpl::~FrameworkImpl()
    {

    }

    HRESULT FrameworkImpl::CreateDevice()
    {
        LOGC << "- Creating D3D Device";

        HRESULT hr = S_OK;


        return S_OK;
    }

    HRESULT FrameworkImpl::CreateSwapChain()
    {
        LOGC << "- Creating D3D SwapChain";

        HRESULT hr = S_OK;

        return S_OK;
    }

    HRESULT FrameworkImpl::Initialize(HINSTANCE hInstance)
    {
        LOGC << "Initializing Takoyaki FrameworkImpl..";

        HRESULT hr = S_OK;



        LOGC << "Initialization complete.";

        return S_OK;
    }

    void FrameworkImpl::Terminate()
    {

    }
}
// namespace Takoyaki