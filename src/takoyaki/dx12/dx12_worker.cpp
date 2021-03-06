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
#include "dx12_worker.h"

#include "dx12_context.h"
#include "dx12_device.h"
#include "dxutility.h"

namespace Takoyaki
{
    DX12Worker::DX12Worker(const DX12WorkerDesc& desc, boost::latch& latch, std::condition_variable& cv)
        : threadPool_{ desc.threadPool }
        , context_(desc.context)
        , device_(desc.device)
        , latch_{ latch }
        , cond_{ cv }
    {
        commandAllocators_.resize(desc.numFrames);

        auto lock = desc.device->getDeviceLock();

        for (uint_fast32_t i = 0; i < desc.numFrames; ++i)
            DXCheckThrow(desc.device->getDXDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators_[i])));
    }

    void DX12Worker::clear()
    {
        commandList_.clear();

        // also reset any memory that might have been used by the allocators
        for (auto& alloc : commandAllocators_)
            alloc->Reset();
    }

    void DX12Worker::main()
    {
        LOG_IDENTIFY_THREAD;

        MoveOnlyFunc genericCmd;
        ThreadPool::GPUDrawFunc gpuCmd;

        auto prevFrame = device_->getCurrentFrame();

        while (threadPool_->getStatus() != ThreadPool::TP_DONE) {
            auto frame = device_->getCurrentFrame();

            if (frame != prevFrame) {
                // frame changed, release memory used by previous allocator
                DXCheckThrow(commandAllocators_[frame]->Reset());
                prevFrame = frame;
            }

            if (threadPool_->tryPopGPUTask(gpuCmd)) {
                TaskCommand cmd;
                cmd.priority = 0;
                {
                    ID3D12PipelineState* ps = nullptr;

                    if (!gpuCmd.first.empty()) {
                        auto psPair = context_->getPipelineState(gpuCmd.first);

                        ps = psPair.first.getPipelineState();
                    }

                    auto lock = device_->getDeviceLock();
                    DXCheckThrow(device_->getDXDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators_[frame].Get(), ps, IID_PPV_ARGS(&cmd.commands)));
                }

                if (gpuCmd.second(&cmd, device_)) {
                    commandList_.push_back(std::move(cmd));
                } else {
                    // something went wrong, cancel current command creation
                    cmd.commands->Close();
                }
            } else if (threadPool_->tryPopGenericTask(genericCmd)) {
                genericCmd();
            } else {
                if (threadPool_->getStatus() == ThreadPool::TP_BARRIER) {
                    latch_.count_down_and_wait();

                    std::unique_lock<std::mutex> lck(mutex_);
                    cond_.wait(lck, [this] { return threadPool_->getStatus() != ThreadPool::TP_BARRIER; });
                } else {
                    std::this_thread::yield();
                }
            }
        }
    }

    void DX12Worker::submitCommandList()
    {
        if (commandList_.size() > 0) {
            auto pair = device_->getCommandList();

            for (auto& cmd : commandList_)
                pair.first.push_back(std::move(cmd));

            commandList_.clear();
        }
    }
} // namespace Takoyaki