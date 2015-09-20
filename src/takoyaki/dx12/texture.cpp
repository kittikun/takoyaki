// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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
#include "texture.h"

#include <intsafe.h>

#include "device_context.h"

namespace Takoyaki
{
    DX12Texture::DX12Texture(std::weak_ptr<DX12DeviceContext> owner)
        : owner_{ owner }
    {
        rtv_.ptr = ULONG_PTR_MAX;
    }

    DX12Texture::DX12Texture(DX12Texture&& other) noexcept
        : owner_{ std::move(other.owner_) }
        , rtv_{ std::move(other.rtv_) }
    {

    }

    DX12Texture::~DX12Texture()
    {
        if ((rtv_.ptr != ULONG_PTR_MAX) && (!owner_.expired())) {
            owner_.lock()->getRTVDescHeapCollection().releaseOne(rtv_);
        }
    }

    const D3D12_CPU_DESCRIPTOR_HANDLE& DX12Texture::getRenderTargetView()
    {
        if (rtv_.ptr == ULONG_PTR_MAX)
            rtv_ = owner_.lock()->getRTVDescHeapCollection().createOne();

        return rtv_;
    }

} // namespace Takoyaki
