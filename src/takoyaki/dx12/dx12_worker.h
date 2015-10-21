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

#include <memory>

#include "dxcommon.h"
#include "../thread_pool.h"

namespace Takoyaki
{
    class DX12Context;
    class DX12Device;
    class ThreadPool;

    struct DX12WorkerDesc
    {
        std::shared_ptr<DX12Context> context;
        std::shared_ptr<DX12Device> device;
        ThreadPool* threadPool;
    };

    class DX12Worker : public ThreadPool::IWorker
    {
        DX12Worker(const DX12Worker&) = delete;
        DX12Worker& operator=(const DX12Worker&) = delete;
        DX12Worker(DX12Worker&&) noexcept;
        DX12Worker& operator=(DX12Worker&&) = delete;

    public:
        DX12Worker(const DX12WorkerDesc&);

        void main() override;
        void submitCommandList() override;

    private:
        ThreadPool* threadPool_;
        std::shared_ptr<DX12Context> context_;
        std::shared_ptr<DX12Device> device_;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
        std::vector<TaskCommand> commandList_;
        DX12Synchronisation sync_;
    };
} // namespace Takoyaki