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
#include "../thread_pool.h"

namespace Takoyaki
{
    class DX12Context;

    struct Command
    {
        uint_fast32_t priority;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commands;
    };

    struct WorkerDesc
    {
        std::weak_ptr<DX12Context> context;
        ThreadPool* threadPool;
    };

    class Worker : public ThreadPool::IWorker
    {
        Worker(const Worker&) = delete;
        Worker& operator=(const Worker&) = delete;
        Worker(Worker&&) noexcept;
        Worker& operator=(Worker&&) = delete;

    public:
        explicit Worker(const WorkerDesc&);

        void main(class ThreadPool*) override;

    private:
        ThreadPool* threadPool_;
        std::weak_ptr<DX12Context> context_;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
        std::vector<Command> commandList_;
    };
} // namespace Takoyaki