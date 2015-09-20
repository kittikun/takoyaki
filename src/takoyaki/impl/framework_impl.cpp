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

#include "../dx12/device.h"
#include "../dx12/device_context.h"
#include "../dx12/texture.h"
#include "../impl/renderer_impl.h"
#include "../public/framework.h"
#include "../public/render_component.h"
#include "../thread_pool.h"
#include "../utility/log.h"
#include "../utility/win_utility.h"

extern void appMain(std::weak_ptr<Takoyaki::Framework>);

namespace Takoyaki
{
    FrameworkImpl::FrameworkImpl()
        : threadPool_{ std::make_shared<ThreadPool>() }
    {
#ifdef _DEBUG
        Log::Initialize();
#endif
    }

    FrameworkImpl::~FrameworkImpl() = default;

    void FrameworkImpl::addRenderComponent(std::shared_ptr<RenderComponent>&& component)
    {
        renderable_.push_back(std::move(component));
    }

    void FrameworkImpl::initialize(const FrameworkDesc& desc, std::weak_ptr<Framework> framework)
    {
        LOG_IDENTIFY_THREAD;
        LOGC_INDENT_START << "Initializing Takoyaki Framework..";

        if (desc.type == EDeviceType::DX12) {
            device_.reset(new DX12Device());
        }

        device_->create(desc);
        renderer_.reset(new RendererImpl(device_->getContext().lock()));

        if (!desc.loadAsyncFunc)
            throw new std::runtime_error{ "FrameworkDesc missing LoadFileAsyncFunc" };

        io_.initialize(desc.loadAsyncFunc);
        threadPool_->initialize(desc.numWorkerThreads);
        shaderManager_.initialize(&io_, threadPool_, device_->getContext());

        LOGC_INDENT_END << "Initialization complete.";

        LOGC << "Launching application...";

        auto func = std::bind(&FrameworkImpl::localAppMain, this, framework);
        threadPool_->submit(func);
    }

    void FrameworkImpl::loadAsyncFileResult(const std::wstring& filename, const std::vector<uint8_t>& res)
    {
        io_.loadAsyncFileResult(makeUnixPath(filename), res);
    }

    void FrameworkImpl::localAppMain(std::weak_ptr<Framework> framework)
    {
        // Do any extra initialization before calling the app main
        LOG_IDENTIFY_THREAD;

        appMain(framework);
    }

    void FrameworkImpl::render()
    {
        LOG_IDENTIFY_THREAD;
        Renderer renderer{ renderer_ };

        for (auto& renderable : renderable_)
            renderable->render(renderer);

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