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

#pragma once

#include "../public/definitions.h"

namespace Takoyaki
{
    class DX12InputLayout;

    class InputLayoutImpl
    {
        InputLayoutImpl(const InputLayoutImpl&) = delete;
        InputLayoutImpl& operator=(const InputLayoutImpl&) = delete;
        InputLayoutImpl(InputLayoutImpl&&) = delete;
        InputLayoutImpl& operator=(InputLayoutImpl&&) = delete;

    public:
        InputLayoutImpl(DX12InputLayout&, boost::shared_lock<boost::shared_mutex>) noexcept;
        ~InputLayoutImpl() = default;

        void addInput(const std::string& name, EFormat format, uint_fast32_t instanceStep = 0);

    private:
        DX12InputLayout& layout_;
        boost::shared_lock<boost::shared_mutex> bufferLock_;    // to avoid removal while user is still using it
    };
}
// namespace Takoyaki
