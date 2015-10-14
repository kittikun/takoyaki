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

#include "../thread_pool.h"

namespace Takoyaki
{
    class DX12Device;
    class ThreadPool;

    constexpr uint_fast32_t WORKER_COPY = 0;

    // A copy worker that specialize in copy commands but do global work when local
    // queue is empty. Also Provides synchronization with other specialized workers
    class CopyWorker
    {
        CopyWorker(const CopyWorker&) = delete;
        CopyWorker& operator=(const CopyWorker&) = delete;
        CopyWorker& operator=(CopyWorker&&) = delete;
        CopyWorker(CopyWorker&&) = delete;

    public:
        struct Context
        {
            std::weak_ptr<DX12Device> device;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;            
        };

        enum class ReturnType
        {
            NONE,
            NOTIFY,
            TASK
        };

        struct Result
        {
            ReturnType type;
            MoveOnlyFunc funcNotify;
            MoveOnlyFuncParamTwo funcTask;
        };

        CopyWorker() noexcept;
        ~CopyWorker() noexcept;

        void initialize(std::weak_ptr<DX12Device>, std::weak_ptr<ThreadPool>);
        void submit(MoveOnlyFuncParamTwo);

    private:
        void main();

    private:
        bool done_;
        std::weak_ptr<DX12Device> device_;
        std::weak_ptr<ThreadPool> threadPool_;
        ThreadSafeQueue<MoveOnlyFuncParamTwo> workQueue_;

        // local command queue
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>  commandQueue_;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;

        // shared synchronization
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        HANDLE fenceEvent_;
        uint64_t fenceValue_;
    };
} // namespace Takoyaki