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

#include "resource.h"
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

        int createDeviceFlags = 0;

#ifdef _DEBUG
        // Disable GPU timeout for NSight
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            //D3D_DRIVER_TYPE_WARP,
            //D3D_DRIVER_TYPE_REFERENCE,
        };

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            // Restrict to 11.1+ devices
            D3D_FEATURE_LEVEL_11_1
        };

        UINT numFeatureLevels = ARRAYSIZE(featureLevels);
        D3D_FEATURE_LEVEL featureLevel;

        // Create classic device first
        ID3D11Device* device;
        ID3D11DeviceContext* context;

        hr = D3D11CreateDevice(nullptr, driverTypes[0], nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &device, &featureLevel, &context);

        if (SUCCEEDED(hr)) {
            // Then query it for ID3D11Device2
            hr = device->QueryInterface(__uuidof(IDXGIDevice2), reinterpret_cast<void**>(&d3dDevice_));

            if (SUCCEEDED(hr)) {
                // And ID3D11DeviceContext2
                context->QueryInterface(__uuidof(ID3D11DeviceContext2), reinterpret_cast<void**>(&d3dContext_));

                if (SUCCEEDED(hr)) {
                    context->Release();
                } else {
                    auto err = boost::format("Failed to get ID3D11DeviceContext2. Error: %1%") % Utility::GetDXGIError(hr);
                    LOGE << boost::str(err);

                    return hr;
                }

                device->Release();
            } else {
                auto err = boost::format("Failed to get ID3D11Device2. Error: %1%") % Utility::GetDXGIError(hr);
                LOGE << boost::str(err);

                return hr;
            }
        } else {
            auto err = boost::format("D3D11CreateDevice failed with %1%") % Utility::GetDXGIError(hr);
            LOGE << boost::str(err);

            return hr;
        }

        return S_OK;
    }

    HRESULT FrameworkImpl::CreateSwapChain(const SIZE& size)
    {
        LOGC << "- Creating D3D SwapChain";

        HRESULT hr = S_OK;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        IDXGIFactory1* dxgiFactory = nullptr;
        IDXGIDevice* dxgiDevice = nullptr;

        hr = d3dDevice_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));

        if (SUCCEEDED(hr)) {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);

            if (SUCCEEDED(hr)) {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            } else {
                LOGE << "Could not get IDXGIFactory1";
                return hr;
            }

            dxgiDevice->Release();
        } else {
            LOGE << "Could not get IDXGIDevice";
            return hr;
        }

        IDXGIFactory2* dxgiFactory2 = nullptr;

        hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));

        if (SUCCEEDED(hr)) {
            DXGI_SWAP_CHAIN_DESC1 sd;

            ZeroMemory(&sd, sizeof(sd));
            sd.Width = size.cx;
            sd.Height = size.cy;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            IDXGISwapChain1* swapChain1;
            hr = dxgiFactory2->CreateSwapChainForHwnd(d3dDevice_, hWnd_, &sd, nullptr, nullptr, &swapChain1);

            if (SUCCEEDED(hr)) {
                hr = swapChain1->QueryInterface(__uuidof(IDXGISwapChain2), reinterpret_cast<void**>(&swapChain_));

                if (SUCCEEDED(hr)) {
                    swapChain1->Release();
                } else {
                    LOGE << "Could not get IDXGISwapChain2";
                    return hr;
                }

                dxgiFactory2->Release();
            } else {
                LOGE << "Could not get IDXGISwapChain1";
                return hr;
            }

            dxgiFactory2->Release();
        } else {
            LOGE << "Could not get IDXGIFactory2";
            return hr;
        }

        // Doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd_, DXGI_MWA_NO_ALT_ENTER);

        dxgiFactory->Release();

        return S_OK;
    }

    HRESULT FrameworkImpl::CreateWDAWindow(HINSTANCE hInstance)
    {
        LOGC << "- Creating window";

        hInst_ = hInstance;

        // Register class
        WNDCLASSEX wcex;

        auto test = LoadLibrary(L"Lib.dll");
        auto hIcon = LoadIcon(test, MAKEINTRESOURCE(IDI_ICON1));

        if (hIcon == nullptr) {
            LOGE << Utility::GetWDAError();
            return E_FAIL;
        }

        // Register class
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &FrameworkImpl::WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = hIcon;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"Takoyaki";
        wcex.hIconSm = nullptr;

        if (!RegisterClassEx(&wcex)) {
            LOGE << Utility::GetWDAError();
            return E_FAIL;
        }

        // Create window
        RECT rc = { 0, 0, 1280, 800 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        hWnd_ = CreateWindow(L"Takoyaki", L"Takoyaki FrameworkImpl",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            static_cast<LPVOID>(this));

        if (hWnd_ == nullptr) {
            LOGE << Utility::GetWDAError();
            return E_FAIL;
        }

        ShowWindow(hWnd_, SW_SHOWDEFAULT);

        return S_OK;
    }

    HRESULT FrameworkImpl::Initialize(HINSTANCE hInstance)
    {
        LOGC << "Initializing Takoyaki FrameworkImpl..";

        HRESULT hr = S_OK;

        if (FAILED(CreateWDAWindow(hInstance)))
            return E_FAIL;

        SIZE size;
        RECT rc;

        GetClientRect(hWnd_, &rc);
        size.cx = rc.right - rc.left;
        size.cy = rc.bottom - rc.top;

        if (FAILED(CreateDevice()))
            return E_FAIL;

        if (FAILED(CreateSwapChain(size)))
            return E_FAIL;


        // Create a render target view
        ID3D11Texture2D* backbuffer = nullptr;
        hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));

        if (FAILED(hr)) {
            LOGE << "Failed to create ID3D11Texture2D";
            return hr;
        }

        hr = d3dDevice_->CreateRenderTargetView(backbuffer, nullptr, &rtv_);
        backbuffer->Release();

        if (FAILED(hr)) {
            auto err = boost::format("Failed to get ID3D11RenderTargetView. Error: %1%") % Utility::GetDXError(hr);
            LOGE << boost::str(err);
            return hr;
        }

        d3dContext_->OMSetRenderTargets(1, &rtv_, nullptr);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)size.cx;
        vp.Height = (FLOAT)size.cy;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        d3dContext_->RSSetViewports(1, &vp);

        LOGC << "Initialization complete.";

        return S_OK;
    }

    void FrameworkImpl::Terminate()
    {

    }

    LRESULT CALLBACK FrameworkImpl::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;

        //FrameworkImpl* pWnd = reinterpret_cast<FrameworkImpl*>(GetWindowLongPtrW(hWnd, 0));

        switch (message) {
            case WM_PAINT:
                hdc = BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                break;

            case WM_DESTROY:
                PostQuitMessage(0);
                break;

                // Note that this tutorial does not handle resizing (WM_SIZE) requests,
                // so we created the window without the resize border.

            case WM_NCCREATE:
            {
                // Store instance pointer while handling the first message
                CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
                LPVOID pThis = pCS->lpCreateParams;

                SetWindowLongPtrW(hWnd, 0, reinterpret_cast<LONG_PTR>(pThis));

                return true;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }
}
// namespace Takoyaki