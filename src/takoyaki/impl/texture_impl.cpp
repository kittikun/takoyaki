// Copyright(c) 2015-2016 Kitti Vongsay
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

#include "pch.h"
#include "texture_impl.h"

#include "../dx12/dx12_context.h"

namespace Takoyaki
{
    TextureImpl::TextureImpl(const std::shared_ptr<DX12Context>& context, const DX12Texture& texture, uint_fast32_t handle) noexcept
        : context_{ context }
        , texture_{ texture }
        , handle_{ handle }
    {
    }

    TextureImpl::~TextureImpl()
    {
        if (handle_ != UINT_FAST32_MAX) {
            auto context = context_.lock();

            context->destroyResource(DX12Context::EResourceType::TEXTURE, handle_);
        }
    }

    uint_fast64_t TextureImpl::getSizeByte() const
    {
        return texture_.getSizeByte();
    }

    void TextureImpl::read(uint8_t* dst, uint_fast32_t size) const
    {
        texture_.read(dst, size);
    }
}
// namespace Takoyaki