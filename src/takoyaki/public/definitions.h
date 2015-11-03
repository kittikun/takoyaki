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

#include <array>
#include <functional>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Takoyaki
{
    using LoadFileAsyncFunc = std::function<void(const std::wstring&)>;

    enum class EBlend
    {
        ZERO,
        ONE,
        SRC_COLOR,
        INV_SRC_COLOR,
        SRC_ALPHA,
        INV_SRC_ALPHA,
        DEST_ALPHA,
        INV_DEST_ALPHA,
        DEST_COLOR,
        INV_DEST_COLOR
    };

    enum class EBlendOp
    {
        ADD,
        SUBTRACT,
        REV_SUBTRACT,
        MIN,
        MAX
    };

    enum class EColorMask : uint8_t
    {
        RED = 1,
        GREEN = 2,
        BLUE = 4,
        ALPHA = 8,
        ALL = RED | GREEN | BLUE | ALPHA
    };

    enum class ECompFunc
    {
        NEVER,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS
    };

    enum class ECullMode
    {
        BACK,
        FRONT,
        NONE
    };

    enum class EDescriptorType
    {
        CONSTANT_BUFFER,
        SAMPLER,
        SHADER_RESOURCE,
        UNORDERED_ACCESS,
    };

    enum class EDeviceType
    {
        UNKNOWN,
        DX12_WIN_RT,
        DX12_WIN_32,
    };

    enum class EDisplayOrientation
    {
        LANDSCAPE,
        PORTRAIT,
        LANDSCAPE_FLIPPED,
        PORTRAIT_FLIPPED
    };

    enum class EFillMode
    {
        SOLID,
        WIREFRAME,
    };

    // will add as needed
    enum class EFormat
    {
        UNKNOWN,
        B8G8R8A8_UNORM,
        R16_UINT,
        R32G32B32_FLOAT,
    };

    enum class ELogicOp
    {
        CLEAR,
        SET,
        COPY,
        COPY_INVERTED,
        NOOP,
        INVERT,
        AND,
        NAND,
        OR,
        NOR,
        XOR,
        EQUIV,
        AND_REVERSE,
        AND_INVERTED,
        OR_REVERSE,
        OR_INVERTED
    };

    // just mirror dx12 for now
    enum ERootSignatureFlag
    {
        NONE                                = 0,
        ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT  = 0x1,
        DENY_VERTEX_SHADER_ROOT_ACCESS      = 0x2,
        DENY_HULL_SHADER_ROOT_ACCESS        = 0x4,
        DENY_DOMAIN_SHADER_ROOT_ACCESS      = 0x8,
        DENY_GEOMETRY_SHADER_ROOT_ACCESS    = 0x10,
        DENY_PIXEL_SHADER_ROOT_ACCESS       = 0x20,
        ALLOW_STREAM_OUTPUT                 = 0x40
    };

// undef the one from math.h
#undef DOMAIN

    enum class EShaderType
    {
        COMPUTE,
        DOMAIN,
        GEOMETRY,
        HULL,
        PIXEL,
        VERTEX
    };

    enum class EStencilOp
    {
        KEEP,
        ZERO,
        REPLACE,
        INCR_SAT,
        DECR_SAT,
        INVERT,
        INCR,
        DECR
    };

    // will add as needed
    enum class ETopology
    {
        TRIANGLELIST
    };

    enum class ETopologyType
    {
        POINT,
        LINE,
        TRIANGLE,
        PATCH
    };

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770338(v=vs.85).aspx
    struct RenderTargetBlendDesc
    {
        RenderTargetBlendDesc() noexcept;

        bool blendEnable;
        bool logicOpEnable;
        EBlend srcBlend;
        EBlend destBlend;
        EBlendOp blendOp;
        EBlend srcBlendAlpha;
        EBlend destBlendAlpha;
        EBlendOp blendOpAlpha;
        ELogicOp logicOp;
        EColorMask renderTargetWriteMask;
    };

    struct BlendDesc
    {
        BlendDesc() noexcept;

        bool alphaToCoverageEnable;
        bool independentBlendEnable;
        std::array<RenderTargetBlendDesc, 8> renderTarget;
    };

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770355(v=vs.85).aspx
    struct StencilOpDesc
    {
        StencilOpDesc() noexcept;

        EStencilOp fail;
        EStencilOp depthFail;
        EStencilOp pass;
        ECompFunc func;
    };

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770356%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
    struct DepthStencilDesc
    {
        DepthStencilDesc() noexcept;

        bool depthEnable;
        bool depthMaskEnable;
        ECompFunc depthFunc;
        bool stencilEnable;
        uint8_t stencilReadMask;
        uint8_t stencilWriteMask;
        StencilOpDesc frontFace;
        StencilOpDesc backFace;
        ECompFunc backFunc;
    };

    struct FrameworkDesc
    {
        FrameworkDesc() noexcept;

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

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770387(v=vs.85).aspx
    struct RasterizerDesc
    {
        RasterizerDesc() noexcept;

        EFillMode fillMode;
        ECullMode cullMode;
        bool frontCCW;
        int32_t depthBias;
        float depthBiasClamp;
        float slopeScaledDepthBias;
        bool depthClipEnable;
        bool multisampleEnable;
        bool antialiasedLineEnable;
        uint_fast32_t forcedSampleCount;
        bool useConservativeRaster;
    };

    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173072(v=vs.85).aspx
    struct MultiSampleDesc
    {
        MultiSampleDesc() noexcept;

        uint_fast32_t count;
        uint_fast32_t quality;
    };

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770370(v=vs.85).aspx
    struct PipelineStateDesc
    {
        PipelineStateDesc() noexcept;

        BlendDesc blendState;
        DepthStencilDesc depthStencilState;
        std::array<EFormat, 8> formatRenderTarget;
        EFormat formatDepthStencil;
        std::string inputLayout;
        uint_fast32_t numRenderTargets;
        RasterizerDesc rasterizerState;
        std::string rootSignature;
        MultiSampleDesc multiSampleState;
        uint_fast32_t sampleMask;
        std::unordered_map<EShaderType, std::string> shaders;
        ETopologyType topology;
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