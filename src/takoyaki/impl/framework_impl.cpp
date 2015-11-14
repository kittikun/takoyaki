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

#include "pch.h"
#include "framework_impl.h"

#include "../dx12/context.h"
#include "../dx12/shader_compiler.h"
#include "../dx12/dx12_texture.h"
#include "../dx12/dx12_worker.h"
#include "../impl/renderer_impl.h"
#include "../public/framework.h"
#include "../public/renderer.h"
#include "../thread_pool.h"
#include "../utility/log.h"
#include "../utility/win_utility.h"

namespace Takoyaki
{
    FrameworkImpl::FrameworkImpl()
    {
    }

    void FrameworkImpl::compileShader(const ShaderDesc& desc)
    {
        threadPool_->submitGeneric(std::bind(&ShaderCompiler::compileShader, &io_, desc, context_), 0);
    }

    void FrameworkImpl::initialize(const FrameworkDesc& desc)
    {
#ifdef _DEBUG
        Log::Initialize(desc);
#endif

        LOG_IDENTIFY_THREAD;
        LOGC_INDENT_START << "Initializing Takoyaki Framework..";

        threadPool_.reset(new ThreadPool(desc.numWorkerThreads));

        if ((desc.type == EDeviceType::DX12_WIN_32) || (desc.type == EDeviceType::DX12_WIN_RT) || (desc.type == EDeviceType::WARP_WIN_32)) {
            device_.reset(new DX12Device());
            context_ = std::make_shared<DX12Context>(device_, threadPool_);

            device_->create(desc, context_);
            device_->createSwapChain();

            DX12WorkerDesc workerDesc;

            workerDesc.context = context_;
            workerDesc.device = device_.get();
            workerDesc.threadPool = threadPool_.get();
            workerDesc.numFrames = desc.bufferCount;

            threadPool_->initialize<DX12Worker, DX12WorkerDesc>(workerDesc);
        }

        if (!desc.loadAsyncFunc)
            throw new std::runtime_error{ "FrameworkDesc missing LoadFileAsyncFunc" };

        io_.initialize(desc.loadAsyncFunc);
        renderer_.reset(new RendererImpl{ device_, context_, threadPool_ });

        LOGC_INDENT_END << "Initialization complete.";
    }

    void FrameworkImpl::loadAsyncFileResult(const std::wstring& filename, const std::vector<uint8_t>& res)
    {
        io_.loadAsyncFileResult(makeUnixPath(filename), res);
    }

    void FrameworkImpl::present()
    {
        // TODO: this design is preventing the creation of multiples frame before they can me rendered
        // there is not point in having a multi-thread command generation otherwise.

        // prevent new tasks from being created while swapping queues
        {
            auto rendererLock = renderer_->getLock();

            threadPool_->swapQueues();
        }

        threadPool_->submitGPUCommandLists();
        device_->executeCommandList();
        device_->present();
    }

    void FrameworkImpl::setWindowSize(const glm::vec2& size)
    {
        // we need to lock the renderer and threadpool while we recreate the swap chain
        auto lock = renderer_->getLock();
        threadPool_->clear();
        device_->setWindowSize(size);
        device_->createSwapChain();
        threadPool_->resume();
    }

    void FrameworkImpl::terminate()
    {
        LOGC << "Terminating Takoyaki Framework..";
        // clear any remaining jobs
        threadPool_->clear();
    }

    void FrameworkImpl::validateDevice() const
    {
        device_->validate();
    }
}
// namespace Takoyaki