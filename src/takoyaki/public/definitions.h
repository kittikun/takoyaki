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

#include <functional>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Takoyaki
{
    using LoadFileAsyncFunc = std::function<void(const std::wstring&)>;

    enum class EDeviceType
    {
        DX12
    };

    enum class EDisplayOrientation
    {
        UNKNOWN,
        LANDSCAPE,
        PORTRAIT,
        LANDSCAPE_FLIPPED,
        PORTRAIT_FLIPPED
    };

    enum class EFormat
    {
        R32G32B32_FLOAT
    };

    enum class EPropertyID
    {
        WINDOW_SIZE,            // glm::vec2
        WINDOW_ORIENTATION,     // DisplayOrientation
        WINDOW_DPI
    };

    enum class EShaderType
    {
        TYPE_COMPUTE,
        TYPE_DOMAIN,
        TYPE_GEOMETRY,
        TYPE_HULL,
        TYPE_PIXEL,
        TYPE_VERTEX
    };

    struct FrameworkDesc
    {
        uint_fast32_t           bufferCount;
        EDisplayOrientation     currentOrientation;
        EDisplayOrientation     nativeOrientation;
        uint_fast32_t           numWorkerThreads;
        EDeviceType             type;
        void*                   windowHandle;
        glm::vec2               windowSize;
        float                   windowDpi;
        LoadFileAsyncFunc       loadAsyncFunc;
    };

    struct PipelineStateDesc
    {
        std::string rootSignature;
        std::string inputLayout;
        std::unordered_map<EShaderType, std::string> shaders;
    };

    struct ShaderDesc
    {
        std::string name;
        EShaderType type;
        std::string path;
        std::string entry;
        bool debug;
    };
} // namespace Takoyaki