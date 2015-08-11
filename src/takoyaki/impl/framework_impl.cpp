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

#include "pch.h"
#include "framework_impl.h"

#include <boost/format.hpp>

#include "../utility/error.h"
#include "../utility/log.h"

using namespace Microsoft::WRL;

namespace Takoyaki
{
    FrameworkImpl::FrameworkImpl()
    {
        Log::Initialize();
    }

    FrameworkImpl::~FrameworkImpl()
    {

    }

    void FrameworkImpl::CreateDevice()
    {
        LOGC << "- Creating D3D Device";

#if defined(_DEBUG)
        // If the project is in a debug build, enable debugging via SDK Layers.
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
            }
        }
#endif

        Utility::DXGICheckThrow(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));

        // Create the Direct3D 12 API device object
        HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3DDevice));

        if (FAILED(hr)) {
            LOGW << "ID3D12Device initialization fails, falling back to the WARP device.";

            ComPtr<IDXGIAdapter> warpAdapter;

            mDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
            Utility::DXGICheckThrow(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3DDevice)));
        }

    }

    void FrameworkImpl::CreateSwapChain()
    {
        LOGC << "- Creating D3D SwapChain";
    }

    void FrameworkImpl::Initialize()
    {
        LOGC << "Initializing Takoyaki FrameworkImpl..";

        CreateDevice();

        LOGC << "Initialization complete.";
    }

    void FrameworkImpl::Terminate()
    {

    }
}
// namespace Takoyaki