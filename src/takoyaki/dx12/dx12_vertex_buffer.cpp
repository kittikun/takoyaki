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
#include "dx12_vertex_buffer.h"

#include "device.h"
#include "dx12_buffer.h"
#include "dxsystem.h"
#include "dxutility.h"

namespace Takoyaki
{
    DX12VertexBuffer::DX12VertexBuffer(uint8_t* vertices, uint_fast64_t sizeVecticesByte, uint8_t* indices, uint_fast64_t sizeIndicesByte) noexcept
        : uploadVertexBuffer_{ std::make_unique<DX12Buffer>(EBufferType::CPU_SLOW_GPU_GOOD, sizeVecticesByte, D3D12_RESOURCE_STATE_GENERIC_READ) }
        , vertexBuffer_{std::make_unique<DX12Buffer>(EBufferType::NO_CPU_GPU_FAST, sizeVecticesByte, D3D12_RESOURCE_STATE_COPY_DEST)}
        , intermediate_{ std::make_unique<Intermediate>() }
    {
        intermediate_->vertexData.pData = vertices;
        intermediate_->vertexData.RowPitch = sizeVecticesByte;
        intermediate_->vertexData.SlicePitch = sizeVecticesByte;
    }

    void DX12VertexBuffer::create(const std::shared_ptr<DX12Device>& device)
    {
        // TODO: probably better to add a resource creation queue in thread pool and use on command list for all
        // thus avoiding running out of threads because they are waiting for resources
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;

        // not thread safe
        {
            auto lock = device->getLock();

            DXCheckThrow(device->getDXDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));
            DXCheckThrow(device->getDXDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList)));
        }

        D3D12_RESOURCE_BARRIER barrier;

        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = vertexBuffer_->getResource();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        cmdList->ResourceBarrier(1, &barrier);
        cmdList->Close();

        // execute command

        // not a good idea to use waitForGPU here, need to think about something clever

        //how to free resource ?

        uploadVertexBuffer_.reset();
        intermediate_.reset();
    }

} // namespace Takoyaki
