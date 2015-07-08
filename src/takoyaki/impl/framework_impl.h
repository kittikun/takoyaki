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

#pragma once

#include <d3d11_2.h>
#include <memory>
#include <windows.h>

namespace Takoyaki
{
    class FrameworkImpl
    {
    public:
        FrameworkImpl();
         ~FrameworkImpl();

        HRESULT Initialize(HINSTANCE);
        void Terminate();

    private:
        HRESULT CreateDevice();
        HRESULT CreateWDAWindow(HINSTANCE);
        HRESULT CreateSwapChain(const SIZE& size);

        static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

    private:
        // DX
        ID3D11Device2* d3dDevice_;
        ID3D11DeviceContext2* d3dContext_;
        IDXGISwapChain2* swapChain_;

        ID3D11RenderTargetView* rtv_;

        // WDA
        HINSTANCE hInst_;
        HWND hWnd_;
    };

} // namespace Takoyaki
