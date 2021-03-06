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
// THE SOFTWARE.

#pragma once

#include <memory>
#include <string>
#include <glm/fwd.hpp>

namespace Takoyaki
{
    class ConstantBufferImpl;

    class ConstantBuffer
    {
        ConstantBuffer(const ConstantBuffer&) = delete;
        ConstantBuffer& operator=(const ConstantBuffer&) = delete;
        ConstantBuffer(ConstantBuffer&&) = delete;
        ConstantBuffer& operator=(ConstantBuffer&&) = delete;

    public:
        ConstantBuffer(std::unique_ptr<ConstantBufferImpl>) noexcept;
        ~ConstantBuffer() noexcept;

        void setMatrix4x4(const std::string& name, const glm::mat4x4& value);

    private:
        std::unique_ptr<ConstantBufferImpl> impl_;
    };
}
// namespace Takoyaki
