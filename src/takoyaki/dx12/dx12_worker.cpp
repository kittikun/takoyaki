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
#include "dx12_worker.h"

#include "device.h"
#include "dxutility.h"

namespace Takoyaki
{
    DX12Worker::DX12Worker(const DX12WorkerDesc& desc)
        : context_(std::move(desc.context))
        , device_(std::move(desc.device))
        , threadPool_{ desc.threadPool }
    {
        auto lock = desc.device->getDeviceLock();

        DXCheckThrow(desc.device->getDXDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_)));
    }

    void DX12Worker::main(class ThreadPool*)
    {
        MoveOnlyFunc generic;
        MoveOnlyFuncParam gpu;

        while (!threadPool_->isDone()) {
            if (threadPool_->tryPopGPUTask(gpu)) {
                Command cmd;

                cmd.device = device_.get();
                cmd.priority = 0;
                {
                    auto lock = device_->getDeviceLock();

                    DXCheckThrow(device_->getDXDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&cmd.commands)));
                }
                gpu(&cmd);

                commandList_.push_back(std::move(cmd));
            } else if (threadPool_->tryPopGenericTask(generic)) {
                generic();
            } else {
                std::this_thread::yield();
            }
        }
    }
} // namespace Takoyaki
