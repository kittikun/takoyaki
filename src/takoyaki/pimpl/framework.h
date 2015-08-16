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

#include <boost/any.hpp>
#include <glm/glm.hpp>

#include "platform.h"
#include "definitions.h"

namespace Takoyaki
{
    class FrameworkImpl;

    struct FrameworkDesc
    {
        uint_fast32_t       bufferCount;
        DisplayOrientation  currentOrientation;
        DisplayOrientation  nativeOrientation;
        DeviceType          type;
        void*               windowHandle;
        glm::vec2           windowSize;
        float               windowDpi;
    };

    class TAKOYAKI_API Framework
    {
        Framework(const Framework&) = delete;
        Framework& operator=(const Framework&) = delete;
        Framework(Framework&&) = delete;
        Framework& operator=(Framework&&) = delete;

    public:
        Framework();
        ~Framework();

        void initialize(const FrameworkDesc&);
        void setProperty(PropertyID, const boost::any&);
        void terminate();

    private:
        std::unique_ptr<FrameworkImpl> impl_;
    };
}
// namespace Takoyaki