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

#pragma warning(push)
#pragma warning(disable : 4251)

#include <memory>
#include <boost/any.hpp>

#include "definitions.h"

namespace Takoyaki
{
    class FrameworkImpl;
    class Renderer;

    class Framework
    {
        Framework(const Framework&) = delete;
        Framework& operator=(const Framework&) = delete;
        Framework(Framework&&) = delete;
        Framework& operator=(Framework&&) = delete;

    public:
        Framework() noexcept;
        ~Framework();

        //////////////////////////////////////////////////////////////////////////
        // To be used by main only

        void initialize(const FrameworkDesc& desc);
        void setProperty(EPropertyID id, const boost::any& value);
        void present();
        void terminate();
        void validateDevice() const;
        void loadAsyncFileResult(const std::wstring& filename, const std::vector<uint8_t>& result);

        //////////////////////////////////////////////////////////////////////////
        // App should use those
        void compileShader(const ShaderDesc& desc);

        //////////////////////////////////////////////////////////////////////////
        // Accessors
        std::unique_ptr<Renderer> getRenderer();

    private:
        std::unique_ptr<FrameworkImpl> impl_;
    };
}
// namespace Takoyaki

#pragma warning(pop) 
