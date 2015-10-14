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
#include "dx12_vertex_buffer.h"

#include "copy_worker.h"
#include "device.h"
#include "dx12_buffer.h"
#include "dxsystem.h"
#include "dxutility.h"
#include "../thread_pool.h"

namespace Takoyaki
{
    DX12VertexBuffer::DX12VertexBuffer(uint8_t* vertices, uint_fast32_t stride, uint_fast32_t sizeByte, uint_fast32_t id) noexcept
        : vertexBuffer_{ std::make_unique<DX12Buffer>(D3D12_HEAP_TYPE_DEFAULT, sizeByte, D3D12_RESOURCE_STATE_COPY_DEST) }
        , uploadBuffer_{ std::make_unique<DX12Buffer>(D3D12_HEAP_TYPE_UPLOAD, sizeByte, D3D12_RESOURCE_STATE_GENERIC_READ) }
        , intermediate_{ std::make_unique<Intermediate>() }
    {
        // we cannot guarantee that data will still be valid so make a copy of data
        // TODO: UpdateSubresourcesHeapAlloc will also make a copy, so merge them 
        intermediate_->data.resize(sizeByte);

        memcpy_s(&intermediate_->data.front(), intermediate_->data.size(), vertices, sizeByte);

        intermediate_->dataDesc.pData = &intermediate_->data.front();
        intermediate_->dataDesc.RowPitch = sizeByte;
        intermediate_->dataDesc.SlicePitch = sizeByte;
        intermediate_->id = id;
       
        view_.StrideInBytes = stride;
        view_.SizeInBytes = sizeByte;
    }

    DX12VertexBuffer::DX12VertexBuffer(DX12VertexBuffer&& other) noexcept
        : vertexBuffer_{ std::move(other.vertexBuffer_) }
        , uploadBuffer_{ std::move(other.uploadBuffer_) }
        , intermediate_{ std::move(other.intermediate_) }
    {

    }

    void DX12VertexBuffer::create(void* p, void* r)
    {
        auto context = static_cast<CopyWorker::Context*>(p);
        auto res = static_cast<CopyWorker::Result*>(r);
        auto device = context->device.lock();

        vertexBuffer_->create(device);
        uploadBuffer_->create(device);

        // finish the view
        view_.BufferLocation = vertexBuffer_->getResource()->GetGPUVirtualAddress();

        // set a name for debug purposes
        auto fmt = boost::wformat{ L"Vertex Buffer %1%" } % intermediate_->id;
        
        vertexBuffer_->getResource()->SetName(boost::str(fmt).c_str());
        fmt = boost::wformat{ L"Vertex Buffer %1% Intermediate" } % intermediate_->id;
        uploadBuffer_->getResource()->SetName(boost::str(fmt).c_str());

        // upload data to the gpu
        UpdateSubresourceDesc desc;

        desc.device = context->device;
        desc.cmdList = context->commandList.Get();
        desc.destinationResource = vertexBuffer_->getResource();
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
        barrier.Transition.pResource = vertexBuffer_->getResource();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        context->commandList->ResourceBarrier(1, &barrier);

        res->type = CopyWorker::ReturnType::TASK;
        res->funcTask = std::bind(&DX12VertexBuffer::createCleanup, this, std::placeholders::_1, std::placeholders::_2);
    }

    void DX12VertexBuffer::createCleanup(void* p, void* r)
    {
        auto context = static_cast<CopyWorker::Context*>(p);
        auto res = static_cast<CopyWorker::Result*>(r);

        context->commandList->DiscardResource(uploadBuffer_->getResource(), nullptr);

        res->type = CopyWorker::ReturnType::NOTIFY;
        res->funcNotify = std::bind(&DX12VertexBuffer::onCreateDone, this);
    }

    void DX12VertexBuffer::onCreateDone()
    {
        intermediate_.reset();
    }

    void DX12VertexBuffer::destroy(ID3D12GraphicsCommandList* commandList)
    {
        commandList->DiscardResource(vertexBuffer_->getResource(), nullptr);
    }
} // namespace Takoyaki
