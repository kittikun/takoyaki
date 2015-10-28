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
#include "dx12_texture.h"

#include <intsafe.h>

#include "context.h"

namespace Takoyaki
{
    DX12Texture::DX12Texture(DX12Context* owner) noexcept
        : owner_{ owner }
    {
        cpuHandle_.ptr = ULONG_PTR_MAX;
    }

    DX12Texture::DX12Texture(DX12Texture&& other) noexcept
        : owner_{ other.owner_ }
        , resource_{ std::move(other.resource_) }
        , cpuHandle_{ std::move(other.cpuHandle_) }
    {
        other.cpuHandle_.ptr =  ULONG_PTR_MAX;
    }

    DX12Texture::~DX12Texture()
    {
        if (cpuHandle_.ptr != ULONG_PTR_MAX) {
            owner_->getRTVDescHeapCollection().releaseOne(cpuHandle_);
        }
    }

    const D3D12_CPU_DESCRIPTOR_HANDLE& DX12Texture::getRenderTargetView()
    {
        if (cpuHandle_.ptr == ULONG_PTR_MAX) {
            auto tuple = owner_->getRTVDescHeapCollection().createOne();

            cpuHandle_ = std::get<0>(tuple);
        }

        return cpuHandle_;
    }

} // namespace Takoyaki