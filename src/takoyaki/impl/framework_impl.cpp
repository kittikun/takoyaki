// Copyright(c) 2015 kittikun
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
#include "framework_impl.h"

#include "../dx12/device.h"
#include "../dx12/context.h"
#include "../dx12/shader_compiler.h"
#include "../dx12/texture.h"
#include "../impl/renderer_impl.h"
#include "../public/framework.h"
#include "../public/renderer.h"
#include "../thread_pool.h"
#include "../utility/log.h"
#include "../utility/win_utility.h"

extern void appMain(const std::shared_ptr<Takoyaki::Framework>&);
extern void appRender(Takoyaki::Renderer& renderer);

namespace Takoyaki
{
    FrameworkImpl::FrameworkImpl()
        : threadPool_{ std::make_shared<ThreadPool>() }
    {
#ifdef _DEBUG
        Log::Initialize();
#endif
    }

    void FrameworkImpl::compileShader(const ShaderDesc& desc)
    {
        // system will use shaderlist, this is for app
        threadPool_->submit(std::bind(&ShaderCompiler::compileShader, &io_, desc, context_));
    }

    void FrameworkImpl::initialize(const FrameworkDesc& desc, const std::shared_ptr<Framework>& framework)
    {
        LOG_IDENTIFY_THREAD;
        LOGC_INDENT_START << "Initializing Takoyaki Framework..";

        threadPool_->initialize(desc.numWorkerThreads);

        if (desc.type == EDeviceType::DX12) {
            device_.reset(new DX12Device());
        }

        context_ = std::make_shared<DX12Context>(device_, threadPool_);

        device_->create(desc, context_);

        if (!desc.loadAsyncFunc)
            throw new std::runtime_error{ "FrameworkDesc missing LoadFileAsyncFunc" };

        io_.initialize(desc.loadAsyncFunc);

        // start shader compilation
        threadPool_->submit(std::bind(&ShaderCompiler::main, &io_, threadPool_, context_));

        renderer_.reset(new RendererImpl(context_));

        LOGC_INDENT_END << "Initialization complete.";

        LOGC << "Launching application...";

        auto func = std::bind(appMain, framework);
        threadPool_->submit(func);
    }

    void FrameworkImpl::loadAsyncFileResult(const std::wstring& filename, const std::vector<uint8_t>& res)
    {
        io_.loadAsyncFileResult(makeUnixPath(filename), res);
    }

    void FrameworkImpl::render()
    {
        LOG_IDENTIFY_THREAD;

        Renderer renderer{ renderer_ };

        appRender(renderer);

        device_->present();
    }

    void FrameworkImpl::setProperty(EPropertyID id, const boost::any& value)
    {
        switch (id) {
            case Takoyaki::EPropertyID::WINDOW_SIZE:
            case Takoyaki::EPropertyID::WINDOW_ORIENTATION:
            case Takoyaki::EPropertyID::WINDOW_DPI:
                device_->setProperty(id, value);
                break;

            default:
                throw new std::runtime_error("FrameworkImpl::setProperty, id");
                break;
        }
    }

    void FrameworkImpl::terminate()
    {

    }

    void FrameworkImpl::validateDevice() const
    {
        device_->validate();
    }
}
// namespace Takoyaki