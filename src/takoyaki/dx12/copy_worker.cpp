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
        , fenceValue_(0)
    {

    }

    CopyWorker::~CopyWorker() noexcept
    {
        done_ = true;
    }

    void CopyWorker::initialize(const std::shared_ptr<DX12Device>& device, std::weak_ptr<ThreadPool> threadPool)
    {
        // register with thread pool
        threadPool_ = threadPool;

        auto pool = threadPool_.lock();
        ThreadPool::SpecializedWorkerDesc desc;

        desc.name = "Copy";
        desc.mainFunc = std::bind(&CopyWorker::main, this);
        desc.submitFunc = std::bind(&CopyWorker::submit, this, std::placeholders::_1);

        pool->addSpecializedWorker(desc);

        // create command queue, fence, etc..
        {
            auto lock = device->getLock();

            DXCheckThrow(device->getDXDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_)));
            DXCheckThrow(device->getDXDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_)));
            DXCheckThrow(device->getDXDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&fence_)));
            ++fenceValue_;
            fenceEvent_ = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        }
    }

    void CopyWorker::main()
    {
        LOG_IDENTIFY_THREAD;

        while (!done_) {
            MoveOnlyFunc task;
            // to avoid mutual reference counting lock, we cannot own the thread pool...
            auto threadPool = threadPool_.lock();

            // Run local queue, if empty do global work
            if (workQueue_.tryPop(task)) {
                task(commandList_.Get());
            } else if (threadPool->tryPopWork(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

    void CopyWorker::submit(MoveOnlyFunc func)
    {
        workQueue_.push(std::move(func));
    }

} // namespace Takoyaki
