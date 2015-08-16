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
#include "DX12Renderer.h"

#include "utility.h"
#include "../utility/log.h"

namespace Takoyaki
{
    DX12Renderer::DX12Renderer(std::shared_ptr<DX12Device>&& device)
        : device_(device)
        , window_(nullptr)
    {
        renderTargets_.resize(device_->getBufferCount());
    }

    void DX12Renderer::createSwapChain()
    {
        device_->waitForGpu();

        // clear old render targets
        for (auto& rt : renderTargets_) {
            rt = nullptr;
        }

        rtvHeap_ = nullptr;

        // Calculate the necessary render target size in pixels.
        glm::vec2 outSize;

        outSize.x = ConvertDipsToPixels(windowSize_.x, dpi_);
        outSize.y = ConvertDipsToPixels(windowSize_.y, dpi_);

        // The width and height of the swap chain must be based on the window's
        // natively-oriented width and height. If the window is not in the native
        // orientation, the dimensions must be reversed.
        DXGI_MODE_ROTATION displayRotation = GetDXGIOrientation();

        bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;

        std::swap(outSize.x, outSize.y);
    }

    DXGI_MODE_ROTATION DX12Renderer::GetDXGIOrientation() const
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        // Note: NativeOrientation can only be Landscape or Portrait even though
        // the DisplayOrientations enum has other values.
        switch (nativeOrientation_) {
            case DisplayOrientation::LANDSCAPE:
                switch (currentOrientation_) {
                    case DisplayOrientation::LANDSCAPE:
                        rotation = DXGI_MODE_ROTATION_IDENTITY;
                        break;

                    case DisplayOrientation::PORTRAIT:
                        rotation = DXGI_MODE_ROTATION_ROTATE270;
                        break;

                    case DisplayOrientation::LANDSCAPE_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE180;
                        break;

                    case DisplayOrientation::PORTRAIT_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE90;
                        break;
                }
                break;

            case DisplayOrientation::PORTRAIT:
                switch (currentOrientation_) {
                    case DisplayOrientation::LANDSCAPE:
                        rotation = DXGI_MODE_ROTATION_ROTATE90;
                        break;

                    case DisplayOrientation::PORTRAIT:
                        rotation = DXGI_MODE_ROTATION_IDENTITY;
                        break;

                    case DisplayOrientation::LANDSCAPE_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE270;
                        break;

                    case DisplayOrientation::PORTRAIT_FLIPPED:
                        rotation = DXGI_MODE_ROTATION_ROTATE180;
                        break;
                }
                break;
        }
        return rotation;
    }

    void DX12Renderer::setProperty(PropertyID id, const boost::any& value)
    {
        switch (id) {
            case Takoyaki::PropertyID::WINDOW_SIZE:
                windowSize_ = boost::any_cast<glm::vec2>(value);
                break;
            case Takoyaki::PropertyID::WINDOW_ORIENTATION:
                currentOrientation_ = boost::any_cast<DisplayOrientation>(value);
                break;
            case Takoyaki::PropertyID::WINDOW_DPI:
                dpi_ = boost::any_cast<float>(value);
                break;
            default:
                LOGE << "setProperty unknown property";
                throw new std::runtime_error("setProperty unknown property");
                break;
        }

        createSwapChain();
    }

    void DX12Renderer::setup(const FrameworkDesc& desc)
    {
        window_ = reinterpret_cast<IUnknown*>(desc.windowHandle);
        currentOrientation_ = desc.currentOrientation;
        nativeOrientation_ = desc.nativeOrientation;
        dpi_ = desc.windowDpi;
        windowSize_ = desc.windowSize;
    }
}