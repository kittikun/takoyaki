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
#include "dx12_index_buffer.h"

#include "copy_worker.h"
#include "device.h"
#include "dx12_buffer.h"
#include "dxsystem.h"
#include "dxutility.h"
#include "../thread_pool.h"

namespace Takoyaki
{
    DX12IndexBuffer::DX12IndexBuffer(uint8_t* indexes, uint_fast64_t sizeByte, uint_fast32_t id) noexcept
        : indexBuffer_{ std::make_unique<DX12Buffer>(EBufferType::NO_CPU_GPU_FAST, sizeByte, D3D12_RESOURCE_STATE_COPY_DEST) }
        , uploadBuffer_{ std::make_unique<DX12Buffer>(EBufferType::CPU_SLOW_GPU_GOOD, sizeByte, D3D12_RESOURCE_STATE_GENERIC_READ) }
        , intermediate_{ std::make_unique<Intermediate>() }
    {
        // we cannot guarantee that data will still be valid so make a copy of data
        intermediate_->data.resize(sizeByte);

        memcpy_s(&intermediate_->data.front(), intermediate_->data.size(), indexes, sizeByte);

        intermediate_->dataDesc.pData = &intermediate_->data.front();
        intermediate_->dataDesc.RowPitch = sizeByte;
        intermediate_->dataDesc.SlicePitch = sizeByte;
        intermediate_->id = id;
    }

    DX12IndexBuffer::DX12IndexBuffer(DX12IndexBuffer&& other) noexcept
        : indexBuffer_{ std::move(other.indexBuffer_) }
        , uploadBuffer_{ std::move(other.uploadBuffer_) }
        , intermediate_{ std::move(other.intermediate_) }
    {

    }

    void DX12IndexBuffer::create(void* p, void* r)
    {
        auto context = static_cast<CopyWorker::Context*>(p);
        auto res = static_cast<CopyWorker::Result*>(r);
        auto device = context->device.lock();

        indexBuffer_->Create(device);
        uploadBuffer_->Create(device);

        // set a name for debug purposes
        auto fmt = boost::wformat{ L"Index Buffer %1%" } % intermediate_->id;

        indexBuffer_->getResource()->SetName(boost::str(fmt).c_str());
        fmt = boost::wformat{ L"Index Buffer %1% Intermediate" } % intermediate_->id;
        uploadBuffer_->getResource()->SetName(boost::str(fmt).c_str());

        // upload data to the gpu
        UpdateSubresourceDesc desc;

        desc.device = context->device;
        desc.cmdList = context->commandList.Get();
        desc.destinationResource = indexBuffer_->getResource();
        desc.intermediate = uploadBuffer_->getResource();
        desc.intermediateOffset = 0;
        desc.firstSubResource = 0;
        desc.numSubResource = 1;
        desc.srcData = &intermediate_->dataDesc;

        UpdateSubresourcesHeapAlloc(desc);

        // on the gpu, copy data from upload buffer to vertex buffer
        D3D12_RESOURCE_BARRIER barrier;

        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = indexBuffer_->getResource();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        context->commandList->ResourceBarrier(1, &barrier);

        res->type = CopyWorker::ReturnType::TASK;
        res->funcTask = std::bind(&DX12IndexBuffer::createCleanup, this, std::placeholders::_1, std::placeholders::_2);
    }

    void DX12IndexBuffer::createCleanup(void* p, void* r)
    {
        auto context = static_cast<CopyWorker::Context*>(p);
        auto res = static_cast<CopyWorker::Result*>(r);

        context->commandList->DiscardResource(uploadBuffer_->getResource(), nullptr);

        res->type = CopyWorker::ReturnType::NOTIFY;
        res->funcNotify = std::bind(&DX12IndexBuffer::onCreateDone, this);
    }

    void DX12IndexBuffer::onCreateDone()
    {
        intermediate_.reset();
    }

    void DX12IndexBuffer::destroy(ID3D12GraphicsCommandList* commandList)
    {
        commandList->DiscardResource(indexBuffer_->getResource(), nullptr);
    }
} // namespace Takoyaki
