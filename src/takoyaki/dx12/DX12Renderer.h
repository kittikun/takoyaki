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

#include "DX12Device.h"

#include "../IRenderer.h"
#include "../pimpl/framework.h"

namespace Takoyaki
{
    class DX12Renderer final : public IRenderer
    {
        DX12Renderer(const DX12Renderer&) = delete;
        DX12Renderer& operator=(const DX12Renderer&) = delete;
        DX12Renderer(DX12Renderer&&) = delete;
        DX12Renderer& operator=(DX12Renderer&&) = delete;

    public:
        DX12Renderer(std::shared_ptr<DX12Device>&&);
        ~DX12Renderer() override = default;

        void createSwapChain() override;
        void setProperty(PropertyID, const boost::any&) override;
        void setup(const FrameworkDesc&) override;

    private:
        DXGI_MODE_ROTATION GetDXGIOrientation() const;

    private:
        std::shared_ptr<DX12Device> device_;

        // window related
        IUnknown* window_;
        glm::vec2 windowSize_;
        float dpi_;
        DisplayOrientation currentOrientation_;
        DisplayOrientation nativeOrientation_;

        // swap chain
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
    };
} // namespace Takoyaki