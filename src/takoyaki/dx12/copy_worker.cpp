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
#include "copy_worker.h"

#include "device.h"
#include "dxutility.h"
#include "../thread_pool.h"
#include "../utility/log.h"

namespace Takoyaki
{
    CopyWorker::CopyWorker() noexcept
        : done_(false)
    {

    }

    CopyWorker::~CopyWorker() noexcept
    {
        done_ = true;
    }

    void CopyWorker::initialize(std::weak_ptr<DX12Device> dev, std::weak_ptr<ThreadPool> threadPool)
    {
        device_ = dev;
        threadPool_ = threadPool;
        fenceEvent_ = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

        // create command queue, fence, etc..
        {
            auto device = dev.lock();
            auto lock = device->getDeviceLock();

            D3D12_COMMAND_QUEUE_DESC queueDesc = {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            DXCheckThrow(device->getDXDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_)));
            DXCheckThrow(device->getDXDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_)));
            DXCheckThrow(device->getDXDevice()->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&fence_)));
        }

        // register with thread pool, important to do this last since thread will be created right away
        auto pool = threadPool_.lock();

        ThreadPool::SpecializedWorkerDesc desc;

        desc.name = "Copy";
        desc.mainFunc = std::bind(&CopyWorker::main, this);
        desc.submitFunc = std::bind(&CopyWorker::submit, this, std::placeholders::_1);
        desc.id = WORKER_COPY;

        pool->addSpecializedWorker(desc);
    }

    void CopyWorker::main()
    {
        LOG_IDENTIFY_THREAD;

        Context context;

        {
            auto device = device_.lock();

            DXCheckThrow(device->getDXDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&context.commandList)));
        }

        context.device = device_;

        MoveOnlyFuncParamReturn specTask;
        MoveOnlyFunc task;
        std::vector<std::function<void()>> doneCallbacks;

        while (!done_) {            
            if (!workQueue_.empty()) {
                // run copy jobs if any
                context.commandList->Reset(commandAllocator_.Get(), nullptr);

                while (workQueue_.tryPop(specTask)) {
                    doneCallbacks.push_back(specTask(&context));
                }

                // Schedule a signal on completion
                DXCheckThrow(commandQueue_->Signal(fence_.Get(), fenceValue_));
                DXCheckThrow(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));

                DXCheckThrow(context.commandList->Close());

                ID3D12CommandList* list[] = { context.commandList.Get() };

                commandQueue_->ExecuteCommandLists(1, list);

                // Wait until the fence has been crossed.
                WaitForSingleObjectEx(fenceEvent_, INFINITE, FALSE);

                // Increment the fence value for the next round
                fenceValue_++;

                // notify users so they can clean up intermediate data
                for (auto func : doneCallbacks)
                    func();

                doneCallbacks.clear();
            } else {             
                // to avoid mutual reference counting lock, we cannot own the thread pool...
                auto threadPool = threadPool_.lock();

                if (threadPool->tryPopWork(task)) {
                    // run generic tasks
                    task();
                } else {
                    // or yield
                    std::this_thread::yield();
                }
            }
        }
    }

    void CopyWorker::submit(MoveOnlyFuncParamReturn func)
    {
        workQueue_.push(std::move(func));
    }
} // namespace Takoyaki
