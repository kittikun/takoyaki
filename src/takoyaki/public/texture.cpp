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
#include "texture.h"

#include "../impl/texture_impl.h"

namespace Takoyaki
{
    Texture::Texture(std::unique_ptr<TextureImpl> impl) noexcept
        : impl_{ std::move(impl) }
    {
    }

    Texture::~Texture() noexcept = default;

    uint_fast32_t Texture::getHandle() const
    {
        return impl_->getHandle();
    }

    uint_fast64_t Texture::getSizeByte() const
    {
        return impl_->getSizeByte();
    }

    void Texture::read(uint8_t* dst, uint_fast32_t size) const
    {
        impl_->read(dst, size);
    }
}
// namespace Takoyaki