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

#include "../io.h"
#include "../dx12/device.h"
#include "../public/definitions.h"

namespace Takoyaki
{
    struct FrameworkDesc;
    class DX12Context;
    class DX12Device;
    class Framework;
    class RendererImpl;
    class ThreadPool;

    class FrameworkImpl
    {
        FrameworkImpl(const FrameworkImpl&) = delete;
        FrameworkImpl& operator=(const FrameworkImpl&) = delete;
        FrameworkImpl(FrameworkImpl&&) = delete;
        FrameworkImpl& operator=(FrameworkImpl&&) = delete;

    public:
        FrameworkImpl();
        ~FrameworkImpl() = default;

        void initialize(const FrameworkDesc&);
        void present();
        void terminate();
        void validateDevice() const;

        void loadAsyncFileResult(const std::wstring&, const std::vector<uint8_t>&);
        inline std::shared_ptr<RendererImpl>& getRenderer() { return renderer_; }

        void compileShader(const ShaderDesc&);

        inline const glm::vec2& getWindowSize() const { return device_->getWindowSize(); }

        void setWindowSize(const glm::vec2& size);
        inline void setWindowOrientation(EDisplayOrientation value) { device_->setWindowOrientation(value); }
        inline void setWindowDpi(float value) { device_->setWindowDpi(value); }

    private:
        IO io_;
        std::shared_ptr<ThreadPool> threadPool_;
        std::shared_ptr<DX12Context> context_;
        std::shared_ptr<RendererImpl> renderer_;
        std::shared_ptr<DX12Device> device_;
    };
} // namespace Takoyaki
