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

#include "dxutility.h"

namespace Takoyaki
{
    DX12PipelineState::DX12PipelineState(const std::string& rs)
        : intermediate_{ std::make_unique<Intermediate>() }
    {
        intermediate_->rootSignature = rs;
    }

    DX12PipelineState::DX12PipelineState(DX12PipelineState&& other)
        : state_{std::move(other.state_)}
        , intermediate_{ std::move(other.intermediate_) }
    {

    }

    DX12PipelineState::~DX12PipelineState() = default;

    // device has already been locked from context
    bool DX12PipelineState::create(const std::shared_ptr<DX12Device>& device)
    {

        return false;
    }

} // namespace Takoyaki
