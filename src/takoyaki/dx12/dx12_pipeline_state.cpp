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

#include "pch.h"
#include "dx12_pipeline_state.h"

#include "device.h"
#include "dxutility.h"
#include "context.h"
#include "dx12_root_signature.h"

namespace Takoyaki
{
    DX12PipelineState::DX12PipelineState(const PipelineStateDesc& desc)
        : intermediate_{ std::make_unique<PipelineStateDesc>(desc) }
    {

    }

    DX12PipelineState::DX12PipelineState(DX12PipelineState&& other)
        : state_{std::move(other.state_)}
        , intermediate_{ std::move(other.intermediate_) }
    {

    }

    DX12PipelineState::~DX12PipelineState() = default;

    // device has already been locked from context
    void DX12PipelineState::create(const std::shared_ptr<DX12Device>& device, const std::shared_ptr<DX12Context>& context)
    {
        if (intermediate_) {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

            // root signature
            {
                auto rs = context->getRootSignature(intermediate_->rootSignature);

                desc.pRootSignature = rs.first.getRootSignature();
            }

            // shaders
            auto found = intermediate_->shaders.find(EShaderType::TYPE_DOMAIN);

            if (found != intermediate_->shaders.end()) {
                desc.DS = context->getShader(found->first, found->second);
            }

            found = intermediate_->shaders.find(EShaderType::TYPE_GEOMETRY);

            if (found != intermediate_->shaders.end()) {
                desc.GS = context->getShader(found->first, found->second);
            }

            found = intermediate_->shaders.find(EShaderType::TYPE_HULL);

            if (found != intermediate_->shaders.end()) {
                desc.HS = context->getShader(found->first, found->second);
            }

            found = intermediate_->shaders.find(EShaderType::TYPE_PIXEL);

            if (found != intermediate_->shaders.end()) {
                desc.PS = context->getShader(found->first, found->second);
            }

            found = intermediate_->shaders.find(EShaderType::TYPE_VERTEX);

            if (found != intermediate_->shaders.end()) {
                desc.VS = context->getShader(found->first, found->second);
            }

            // input layouts
            {
                auto layout = context->getInputLayout(intermediate_->inputLayout);

                desc.InputLayout = layout.first.getInputLayout();
            }

            desc.BlendState = BlendDescToDX(intermediate_->blendState);
            desc.DepthStencilState = DepthStencilDescToDX(intermediate_->depthStencilState);
            desc.RasterizerState = RasterizerDescToDX(intermediate_->rasterizerState);
            desc.SampleMask = intermediate_->sampleMask;
            desc.PrimitiveTopologyType = TopologyToDX(intermediate_->topology);
            desc.NumRenderTargets = intermediate_->numRenderTargets;
            desc.SampleDesc = MultiSampleDescToDX(intermediate_->multiSampleState);
            
            for (size_t i = 0; i < intermediate_->formatRenderTarget.size(); ++i)
                desc.RTVFormats[i] = FormatToDX(intermediate_->formatRenderTarget[i]);

            DXCheckThrow(device->getDXDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&state_)));
            intermediate_.reset();
        }
    }

} // namespace Takoyaki
