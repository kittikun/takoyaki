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

#pragma once

namespace Takoyaki
{
    class DX12Context;
    class DX12Texture;

    class TextureImpl
    {
        TextureImpl(const TextureImpl&) = delete;
        TextureImpl& operator=(const TextureImpl&) = delete;
        TextureImpl(TextureImpl&&) = delete;
        TextureImpl& operator=(TextureImpl&&) = delete;

    public:
        explicit TextureImpl(const std::shared_ptr<DX12Context>&, const DX12Texture&, uint_fast32_t) noexcept;
        ~TextureImpl();

        inline uint_fast32_t getHandle() const { return handle_; }
        uint_fast64_t getSizeByte() const;

        void read(uint8_t*, uint_fast32_t) const;

    private:
        // must own pointer to context for destruction
        std::weak_ptr<DX12Context> context_;
        const DX12Texture& texture_;
        uint_fast32_t handle_;
    };
}
// namespace Takoyaki
