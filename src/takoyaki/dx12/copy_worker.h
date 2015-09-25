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

#pragma once

#include "../thread_pool.h"

namespace Takoyaki
{
    class DX12Device;
    class ThreadPool;

    // A copy worker that specialize in copy commands but do global work when local
    // queue is empty. Also Provides synchronization with other specialized workers
    class CopyWorker
    {
        CopyWorker(const CopyWorker&) = delete;
        CopyWorker& operator=(const CopyWorker&) = delete;
        CopyWorker& operator=(CopyWorker&&) = delete;
        CopyWorker(CopyWorker&&) = delete;

    public:
        CopyWorker() = default;
        ~CopyWorker() = default;

        void initialize(const std::shared_ptr<DX12Device>&);

        inline void setThreadPool(std::weak_ptr<ThreadPool> pool) { threadPool_ = pool; }

    private:
        std::weak_ptr<ThreadPool> threadPool_;
        ThreadSafeQueue<MoveOnlyFunc> workQueue_; 

        // local command queue
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>  commandQueue_;
        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators_;

        // shared synchronization
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        std::vector<uint64_t> fenceValues_;
        HANDLE fenceEvent_;
    };
} // namespace Takoyaki