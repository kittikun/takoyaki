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

#pragma once

#include <memory>

namespace Takoyaki
{
    class DX12ConstantBuffer;

    class ConstantTableImpl
    {
        ConstantTableImpl(const ConstantTableImpl&) = delete;
        ConstantTableImpl& operator=(const ConstantTableImpl&) = delete;
        ConstantTableImpl(ConstantTableImpl&&) = delete;
        ConstantTableImpl& operator=(ConstantTableImpl&&) = delete;

    public:
        ConstantTableImpl(DX12ConstantBuffer&, boost::shared_lock<boost::shared_mutex>);
        ~ConstantTableImpl();

        void setMatrix4x4(const std::string&, const glm::mat4x4&);

    private:
        DX12ConstantBuffer& cbuffer_;
        boost::shared_lock<boost::shared_mutex> bufferLock_;    // to avoid removal while user is still using it
    };
}
// namespace Takoyaki
