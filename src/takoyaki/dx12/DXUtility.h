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
    float ConvertDipsToPixels(float dips, float dpi);
    void DXCheckThrow(HRESULT);

    // enum conversions
    D3D12_BLEND BlendToDX(EBlend);
    D3D12_BLEND_OP BlendOpToDX(EBlendOp);
    D3D12_COMPARISON_FUNC CompFuncToDX(ECompFunc);
    D3D12_CULL_MODE CullModeToDX(ECullMode);
    D3D12_DESCRIPTOR_RANGE_TYPE DescriptorTypeToDX(EDescriptorType);
    DXGI_FORMAT FormatToDX(EFormat);
    D3D12_FILL_MODE FillModeToDX(EFillMode);
    std::string GetDXError(HRESULT);
    D3D12_LOGIC_OP LogicOpToDX(ELogicOp);
    D3D12_RESOURCE_FLAGS ResourceFlagsToDX(EResourceFlags);
    D3D12_STENCIL_OP StencilOpToDX(EStencilOp);
    D3D12_PRIMITIVE_TOPOLOGY TopologyToDX(ETopology);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyTypeToDX(ETopologyType);
    D3D12_HEAP_TYPE UsageTypeToDX(EUsageType);

    // struct conversions
    D3D12_BLEND_DESC BlendDescToDX(const BlendDesc&);
    D3D12_DEPTH_STENCIL_DESC DepthStencilDescToDX(const DepthStencilDesc&);
    DXGI_SAMPLE_DESC  MultiSampleDescToDX(const MultiSampleDesc&);
    D3D12_RASTERIZER_DESC RasterizerDescToDX(const RasterizerDesc&);
    D3D12_DEPTH_STENCILOP_DESC StencilOpDescToDX(const StencilOpDesc&);
} // namespace Takoyaki