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
#include "dx12_constant_buffer.h"

#include <intsafe.h>
#include <glm/glm.hpp>
#include <boost/format.hpp>

#include "dx12_buffer.h"
#include "context.h"
#include "device.h"
#include "../utility/log.h"

namespace Takoyaki
{
    DX12ConstantBuffer::DX12ConstantBuffer(DX12Context* context, uint_fast32_t sizeByte, uint_fast32_t numFrames)
        : owner_{ context }
        , buffer_{ std::make_unique<DX12Buffer>(D3D12_HEAP_TYPE_UPLOAD, sizeByte * numFrames, D3D12_RESOURCE_STATE_GENERIC_READ) }
        , mappedAddr_{ nullptr }
        , curOffset_{ 0 }
        , size_{ sizeByte }
        , ready_{ false }
    {
    }

    DX12ConstantBuffer::DX12ConstantBuffer(DX12ConstantBuffer&& other) noexcept
        : owner_{ other.owner_ }
        , buffer_{ std::move(other.buffer_) }
        , offsetMap_{ std::move(other.offsetMap_) }
        , cpuHandles_{ std::move(other.cpuHandles_) }
        , mappedAddr_{ other.mappedAddr_ }
        , curOffset_{ other.curOffset_ }
        , size_{ other.size_ }
        , ready_{ other.ready_.load() }
    {
        other.cpuHandles_.clear();
    }

    DX12ConstantBuffer::~DX12ConstantBuffer()
    {
        // only cpu handles are used to track descriptor heap usage
        if (!cpuHandles_.empty())
            owner_->getRTVDescHeapCollection().releaseRange(cpuHandles_.begin(), cpuHandles_.end());
    }

    void DX12ConstantBuffer::addVariable(const std::string& name, uint_fast32_t offset, uint_fast32_t size)
    {
        CBVariable var;

        var.offset = offset;
        var.size = curOffset_;

        offsetMap_.insert({ name, std::move(var) });
        curOffset_ += size;
    }

    void DX12ConstantBuffer::create(const std::string& name, DX12Device* device)
    {
        buffer_->create(device);

        auto res = buffer_->getResource();
        auto gpuAddress = res->GetGPUVirtualAddress();
        auto fmt = boost::wformat{ L"%1%" } % name.c_str();
        auto bufCount = device->getFrameCount();

        cpuHandles_.reserve(bufCount);
        cpuHandles_.reserve(bufCount);
        heaps_.reserve(bufCount);

        auto handles = owner_->getSRVDescHeapCollection().createRange(bufCount);

        for (auto& tuple : handles) {
            cpuHandles_.push_back(std::get<0>(tuple));
            gpuHandles_.push_back(std::get<1>(tuple));
            heaps_.push_back(std::get<2>(tuple));
        }

        res->SetName(boost::str(fmt).c_str());

        // create view, one per buffer in the swap-chain
        for (uint_fast32_t i = 0; i < bufCount; ++i) {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc;

            desc.BufferLocation = gpuAddress + (i * size_);
            desc.SizeInBytes = size_;

            {
                auto lock = device->getDeviceLock();

                // create constant buffer views to access the upload buffer
                device->getDXDevice()->CreateConstantBufferView(&desc, cpuHandles_[i]);
            }
        }

        // we don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.
        DXCheckThrow(res->Map(0, nullptr, reinterpret_cast<void**>(&mappedAddr_)));
        ZeroMemory(mappedAddr_, size_ * bufCount);

        ready_ = true;
    }

    void DX12ConstantBuffer::setMatrix4x4(const std::string& name, const glm::mat4x4& value, uint_fast32_t frame)
    {
        auto found = offsetMap_.find(name);

        if (found == offsetMap_.end()) {
            auto fmt = boost::format{ "DX12ConstantBuffer, could not find constant %1%" } % name;

            LOGW << boost::str(fmt);
            //throw new std::runtime_error(boost::str(fmt));
        } else {
            // TODO: doing this for each upload is potentially slow, maybe write in a temporary buffer first?
            // https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0
            uint8_t* dest = mappedAddr_ + (frame * size_) + found->second.offset;
            memcpy(dest, &value, sizeof(glm::mat4x4));
        }
    }
} // namespace Takoyaki