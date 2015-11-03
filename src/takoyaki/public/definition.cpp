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

#include "pch.h"
#include "definitions.h"

namespace Takoyaki
{
    BlendDesc::BlendDesc() noexcept
        : alphaToCoverageEnable{false}
        , independentBlendEnable{false}
    {

    }

    DepthStencilDesc::DepthStencilDesc() noexcept
        : depthEnable{ true }
        , depthMaskEnable{ true }
        , depthFunc{ ECompFunc::LESS }
        , stencilEnable{ false }
        , stencilReadMask{ 0xff }
        , stencilWriteMask{ 0xff }
        , frontFace{}
        , backFace{}
    {

    }

    FrameworkDesc::FrameworkDesc() noexcept
        : bufferCount{ 3 }
        , currentOrientation{ EDisplayOrientation::LANDSCAPE }
        , nativeOrientation{ EDisplayOrientation::LANDSCAPE }
        , numWorkerThreads{ 4 }
        , windowHandle{ nullptr }
        , windowDpi{ 96.f }
    {
    }

    MultiSampleDesc::MultiSampleDesc() noexcept
        : count{ 1 }
        , quality{ 0 }
    {

    }

    PipelineStateDesc::PipelineStateDesc() noexcept
        : blendState{}
        , depthStencilState{}
        , rasterizerState{}
        , sampleMask{ UINT_MAX }
    {
    }

    RasterizerDesc::RasterizerDesc() noexcept
        : fillMode{ EFillMode::SOLID }
        , cullMode{ ECullMode::BACK }
        , frontCCW{ false }
        , depthBias{ 0 }
        , depthBiasClamp{ 0.f }
        , slopeScaledDepthBias{ 0.f }
        , depthClipEnable{ true }
        , multisampleEnable{ false }
        , forcedSampleCount{ 0 }
        , useConservativeRaster{ false }
    {
    }

    RenderTargetBlendDesc::RenderTargetBlendDesc() noexcept
        : blendEnable{ false }
        , logicOpEnable{ false }
        , srcBlend{ EBlend::ONE }
        , destBlend{ EBlend::ZERO }
        , blendOp{ EBlendOp::ADD }
        , srcBlendAlpha{ EBlend::ONE }
        , destBlendAlpha{ EBlend::ZERO }
        , blendOpAlpha{ EBlendOp::ADD }
        , logicOp{ ELogicOp::NOOP }
        , renderTargetWriteMask{ EColorMask::ALL }
    {

    }

    StencilOpDesc::StencilOpDesc() noexcept
        : fail{ EStencilOp::KEEP }
        , depthFail{ EStencilOp::KEEP }
        , pass{ EStencilOp::KEEP }
        , func{ ECompFunc::ALWAYS }
    {

    }
} // namespace Takoyaki