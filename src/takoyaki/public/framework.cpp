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

#include "pch.h"
#include "framework.h"

#include "renderer.h"
#include "../impl/framework_impl.h"

namespace Takoyaki
{
    Framework::Framework() noexcept
        : impl_{ std::make_unique<FrameworkImpl>() }
    {
    }

    Framework::~Framework() = default;

    std::unique_ptr<Renderer> Framework::getRenderer()
    {
        return std::make_unique<Renderer>(impl_->getRenderer());
    }

    const glm::vec2& Framework::getWindowSize() const
    {
        return impl_->getWindowSize();
    }

    void Framework::initialize(const FrameworkDesc& desc)
    {
        impl_->initialize(desc);
    }

    void Framework::present()
    {
        impl_->present();
    }

    void Framework::setDisplayDpi(float dpi)
    {
        impl_->setWindowDpi(dpi);
    }

    void Framework::setDisplayOrientation(EDisplayOrientation orientation)
    {
        impl_->setWindowOrientation(orientation);
    }

    void Framework::setWindowSize(const glm::vec2& size)
    {
        impl_->setWindowSize(size);
    }

    void Framework::terminate()
    {
        impl_->terminate();
    }

    void Framework::validateDevice() const
    {
        impl_->validateDevice();
    }
}
// namespace Takoyaki