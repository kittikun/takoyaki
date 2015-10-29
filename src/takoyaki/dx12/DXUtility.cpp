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

#include "pch.h"
#include "dxutility.h"

#include "../utility/log.h"

namespace Takoyaki
{
    D3D12_BLEND BlendToDX(EBlend blend)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770338(v=vs.85).aspx
        // some values are missing
        switch (blend) {
            case Takoyaki::EBlend::ZERO:
                return D3D12_BLEND_ZERO;
            case Takoyaki::EBlend::SRC_COLOR:
                return D3D12_BLEND_SRC_COLOR;
            case Takoyaki::EBlend::INV_SRC_COLOR:
                return D3D12_BLEND_INV_SRC_COLOR;
            case Takoyaki::EBlend::SRC_ALPHA:
                return D3D12_BLEND_SRC_ALPHA;
            case Takoyaki::EBlend::INV_SRC_ALPHA:
                return D3D12_BLEND_INV_SRC_ALPHA;
            case Takoyaki::EBlend::DEST_ALPHA:
                return D3D12_BLEND_DEST_ALPHA;
            case Takoyaki::EBlend::INV_DEST_ALPHA:
                return D3D12_BLEND_INV_DEST_ALPHA;
            case Takoyaki::EBlend::DEST_COLOR:
                return D3D12_BLEND_DEST_COLOR;
            case Takoyaki::EBlend::INV_DEST_COLOR:
                return D3D12_BLEND_INV_DEST_COLOR;
        }

        return D3D12_BLEND_ONE;
    }

    D3D12_BLEND_OP BlendOpToDX(EBlendOp op)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770340(v=vs.85).aspx
        switch (op) {
            case Takoyaki::EBlendOp::SUBTRACT:
                return D3D12_BLEND_OP_SUBTRACT;
            case Takoyaki::EBlendOp::REV_SUBTRACT:
                return D3D12_BLEND_OP_REV_SUBTRACT;
            case Takoyaki::EBlendOp::MIN:
                return D3D12_BLEND_OP_MIN;
            case Takoyaki::EBlendOp::MAX:
                return D3D12_BLEND_OP_MAX;
        }

        return D3D12_BLEND_OP_ADD;
    }

    D3D12_COMPARISON_FUNC CompFuncToDX(ECompFunc func)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770349(v=vs.85).aspx
        switch (func) {
            case Takoyaki::ECompFunc::NEVER:
                return D3D12_COMPARISON_FUNC_NEVER;
            case Takoyaki::ECompFunc::LESS:
                return D3D12_COMPARISON_FUNC_LESS;
            case Takoyaki::ECompFunc::EQUAL:
                return D3D12_COMPARISON_FUNC_EQUAL;
            case Takoyaki::ECompFunc::LESS_EQUAL:
                return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case Takoyaki::ECompFunc::GREATER:
                return D3D12_COMPARISON_FUNC_GREATER;
            case Takoyaki::ECompFunc::NOT_EQUAL:
                return D3D12_COMPARISON_FUNC_NOT_EQUAL;
            case Takoyaki::ECompFunc::GREATER_EQUAL:
                return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        }

        return D3D12_COMPARISON_FUNC_ALWAYS;
    }

    float ConvertDipsToPixels(float dips, float dpi)
    {
        // https://en.wikipedia.org/wiki/Device_independent_pixel
        constexpr float dipsPerInch = 96.0f;

        return std::max(floorf(dips * dpi / dipsPerInch + 0.5f), 1.f);
    }

    //////////////////////////////////////////////////////////////////////////
    // Enum conversions
    //////////////////////////////////////////////////////////////////////////

    D3D12_CULL_MODE CullModeToDX(ECullMode mode)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770354(v=vs.85).aspx
        if (mode == ECullMode::BACK)
            return D3D12_CULL_MODE_BACK;
        else if (mode == ECullMode::FRONT)
            return D3D12_CULL_MODE_FRONT;

        return D3D12_CULL_MODE_NONE;
    }

    D3D12_FILL_MODE FillModeToDX(EFillMode mode)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770366(v=vs.85).aspx
        if (mode == EFillMode::SOLID)
            return D3D12_FILL_MODE_SOLID;
         
        return D3D12_FILL_MODE_WIREFRAME;
    }

    DXGI_FORMAT FormatToDX(EFormat format)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
        switch (format) {
            case EFormat::B8G8R8A8_UNORM:
                return DXGI_FORMAT_B8G8R8A8_UNORM;

            case EFormat::R16_UINT:
                return DXGI_FORMAT_R16_UINT;

            case EFormat::R32G32B32_FLOAT:
                return DXGI_FORMAT_R32G32B32_FLOAT;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    std::string GetDXError(HRESULT code)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476174(v=vs.85).aspx
        static std::unordered_map<HRESULT, std::string> map = {
            { D3D11_ERROR_FILE_NOT_FOUND, "D3D11_ERROR_FILE_NOT_FOUND" },
            { D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS" },
            { D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS" },
            { D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD" },
            { E_FAIL, "E_FAIL" },
            { E_INVALIDARG, "E_INVALIDARG" },
            { E_OUTOFMEMORY, "E_OUTOFMEMORY" },
            { E_NOTIMPL, "E_NOTIMPL" },
            { S_FALSE, "S_FALSE" },
            // https://msdn.microsoft.com/en-us/library/windows/desktop/bb509553(v=vs.85).aspx
            { DXGI_ERROR_DEVICE_HUNG, "DXGI_ERROR_DEVICE_HUNG" },
            { DXGI_ERROR_DEVICE_REMOVED, "DXGI_ERROR_DEVICE_REMOVED" },
            { DXGI_ERROR_DEVICE_RESET, "DXGI_ERROR_DEVICE_RESET" },
            { DXGI_ERROR_DRIVER_INTERNAL_ERROR, "DXGI_ERROR_DRIVER_INTERNAL_ERROR" },
            { DXGI_ERROR_FRAME_STATISTICS_DISJOINT, "DXGI_ERROR_FRAME_STATISTICS_DISJOINT" },
            { DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE, "DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE" },
            { DXGI_ERROR_INVALID_CALL, "DXGI_ERROR_INVALID_CALL" },
            { DXGI_ERROR_MORE_DATA, "DXGI_ERROR_MORE_DATA" },
            { DXGI_ERROR_NONEXCLUSIVE, "DXGI_ERROR_NONEXCLUSIVE" },
            { DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE" },
            { DXGI_ERROR_NOT_FOUND, "DXGI_ERROR_NOT_FOUND" },
            { DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED, "DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED" },
            { DXGI_ERROR_REMOTE_OUTOFMEMORY, "DXGI_ERROR_REMOTE_OUTOFMEMORY" },
            { DXGI_ERROR_WAS_STILL_DRAWING, "DXGI_ERROR_WAS_STILL_DRAWING" },
            { DXGI_ERROR_UNSUPPORTED, "DXGI_ERROR_UNSUPPORTED" },
            { DXGI_ERROR_ACCESS_LOST, "DXGI_ERROR_ACCESS_LOST" },
            { DXGI_ERROR_WAIT_TIMEOUT, "DXGI_ERROR_WAIT_TIMEOUT" },
            { DXGI_ERROR_SESSION_DISCONNECTED, "DXGI_ERROR_SESSION_DISCONNECTED" },
            { DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE, "DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE" },
            { DXGI_ERROR_CANNOT_PROTECT_CONTENT, "DXGI_ERROR_CANNOT_PROTECT_CONTENT" },
            { DXGI_ERROR_ACCESS_DENIED, "DXGI_ERROR_ACCESS_DENIED" },
            { DXGI_ERROR_NAME_ALREADY_EXISTS, "DXGI_ERROR_NAME_ALREADY_EXISTS" },
            { DXGI_ERROR_SDK_COMPONENT_MISSING, "DXGI_ERROR_SDK_COMPONENT_MISSING" },
            { S_OK ,"S_OK" }
        };

        return map[code];
    }

    void DXCheckThrow(HRESULT hr)
    {
        if (FAILED(hr)) {
            throw std::runtime_error{ GetDXError(hr) };
        }
    }

    D3D12_LOGIC_OP LogicOpToDX(ELogicOp op)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770379(v=vs.85).aspx
        switch (op) {
            case ELogicOp::SET:
                return D3D12_LOGIC_OP_SET;
            case ELogicOp::COPY:
                return D3D12_LOGIC_OP_COPY;
            case ELogicOp::COPY_INVERTED:
                return D3D12_LOGIC_OP_COPY_INVERTED;
            case ELogicOp::NOOP:
                return D3D12_LOGIC_OP_NOOP;
            case ELogicOp::INVERT:
                return D3D12_LOGIC_OP_INVERT;
            case ELogicOp::AND:
                return D3D12_LOGIC_OP_AND;
            case ELogicOp::NAND:
                return D3D12_LOGIC_OP_NAND;
            case ELogicOp::OR:
                return D3D12_LOGIC_OP_OR;
            case ELogicOp::NOR:
                return D3D12_LOGIC_OP_NOR;
            case ELogicOp::XOR:
                return D3D12_LOGIC_OP_XOR;
            case ELogicOp::EQUIV:
                return D3D12_LOGIC_OP_EQUIV;
            case ELogicOp::AND_REVERSE:
                return D3D12_LOGIC_OP_AND_REVERSE;
            case ELogicOp::AND_INVERTED:
                return D3D12_LOGIC_OP_AND_INVERTED;
            case ELogicOp::OR_REVERSE:
                return D3D12_LOGIC_OP_OR_REVERSE;
            case ELogicOp::OR_INVERTED:
                return D3D12_LOGIC_OP_OR_INVERTED;
        }

        return D3D12_LOGIC_OP_CLEAR;
    }

    D3D12_STENCIL_OP StencilOpToDX(EStencilOp op)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770409(v=vs.85).aspx
        switch (op) {
            case EStencilOp::DECR:
                return D3D12_STENCIL_OP_DECR;
            case EStencilOp::DECR_SAT:
                return D3D12_STENCIL_OP_DECR_SAT;
            case EStencilOp::INCR:
                return D3D12_STENCIL_OP_INCR;
            case EStencilOp::INCR_SAT:
                return D3D12_STENCIL_OP_INCR_SAT;
            case EStencilOp::INVERT:
                return D3D12_STENCIL_OP_INVERT;
            case EStencilOp::REPLACE:
                return D3D12_STENCIL_OP_REPLACE;
            case EStencilOp::ZERO:
                return D3D12_STENCIL_OP_ZERO;
        }

        return D3D12_STENCIL_OP_KEEP;
    }

    D3D12_PRIMITIVE_TOPOLOGY TopologyToDX(ETopology value)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ff728726(v=vs.85).aspx
        switch (value) {
            case ETopology::TRIANGLELIST:
                return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        }

        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyTypeToDX(ETopologyType value)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770385(v=vs.85).aspx
        switch (value) {
            case ETopologyType::LINE:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case ETopologyType::PATCH:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
            case ETopologyType::POINT:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            case ETopologyType::TRIANGLE:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        }

        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }

    //////////////////////////////////////////////////////////////////////////
    // Struct conversions
    //////////////////////////////////////////////////////////////////////////

    D3D12_BLEND_DESC BlendDescToDX(const BlendDesc& desc)
    {
        D3D12_BLEND_DESC res;

        res.AlphaToCoverageEnable = desc.alphaToCoverageEnable;
        res.IndependentBlendEnable = desc.independentBlendEnable;

        for (size_t i = 0; i < desc.renderTarget.size(); ++i) {
            res.RenderTarget[i].BlendEnable = desc.renderTarget[i].blendEnable;
            res.RenderTarget[i].BlendOp = BlendOpToDX(desc.renderTarget[i].blendOp);
            res.RenderTarget[i].BlendOpAlpha = BlendOpToDX(desc.renderTarget[i].blendOpAlpha);
            res.RenderTarget[i].DestBlend = BlendToDX(desc.renderTarget[i].destBlend);
            res.RenderTarget[i].DestBlendAlpha = BlendToDX(desc.renderTarget[i].destBlendAlpha);
            res.RenderTarget[i].LogicOp = LogicOpToDX(desc.renderTarget[i].logicOp);
            res.RenderTarget[i].LogicOpEnable = desc.renderTarget[i].logicOpEnable;
            res.RenderTarget[i].RenderTargetWriteMask = static_cast<UINT8>(desc.renderTarget[i].renderTargetWriteMask);
            res.RenderTarget[i].SrcBlend = BlendToDX(desc.renderTarget[i].srcBlend);
            res.RenderTarget[i].SrcBlendAlpha = BlendToDX(desc.renderTarget[i].srcBlendAlpha);
        }

        return res;
    }

    D3D12_DEPTH_STENCIL_DESC DepthStencilDescToDX(const DepthStencilDesc& desc)
    {
        D3D12_DEPTH_STENCIL_DESC res;

        res.BackFace = StencilOpDescToDX(desc.backFace);
        res.DepthEnable = desc.depthEnable;
        res.DepthFunc = CompFuncToDX(desc.depthFunc);
        res.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(desc.depthMaskEnable);
        res.FrontFace = StencilOpDescToDX(desc.frontFace);
        res.StencilEnable = desc.stencilEnable;
        res.StencilReadMask = desc.stencilReadMask;
        res.StencilWriteMask = desc.stencilWriteMask;

        return res;
    }

    DXGI_SAMPLE_DESC  MultiSampleDescToDX(const MultiSampleDesc& desc)
    {
        DXGI_SAMPLE_DESC res;

        res.Count = desc.count;
        res.Quality = desc.quality;

        return res;
    }

    D3D12_RASTERIZER_DESC RasterizerDescToDX(const RasterizerDesc& desc)
    {
        D3D12_RASTERIZER_DESC res;

        res.AntialiasedLineEnable = desc.antialiasedLineEnable;
        res.ConservativeRaster = static_cast<D3D12_CONSERVATIVE_RASTERIZATION_MODE>(desc.useConservativeRaster);
        res.CullMode = CullModeToDX(desc.cullMode);
        res.DepthBias = desc.depthBias;
        res.DepthBiasClamp = desc.depthBiasClamp;
        res.DepthClipEnable = desc.depthClipEnable;
        res.FillMode = FillModeToDX(desc.fillMode);
        res.ForcedSampleCount = desc.forcedSampleCount;
        res.FrontCounterClockwise = desc.frontCCW;
        res.MultisampleEnable = desc.multisampleEnable;
        res.SlopeScaledDepthBias = desc.slopeScaledDepthBias;

        return res;
    }

    D3D12_DEPTH_STENCILOP_DESC StencilOpDescToDX(const StencilOpDesc& desc)
    {
        D3D12_DEPTH_STENCILOP_DESC res;

        res.StencilDepthFailOp = StencilOpToDX(desc.depthFail);
        res.StencilFailOp = StencilOpToDX(desc.fail);
        res.StencilPassOp = StencilOpToDX(desc.pass);
        res.StencilFunc = CompFuncToDX(desc.func);

        return res;
    }
} // namespace Takoyaki