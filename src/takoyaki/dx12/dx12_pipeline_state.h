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

#include <d3d12.h>

#include "../public/definitions.h"

namespace Takoyaki
{
    class DX12Device;

    class DX12PipelineState
    {
        DX12PipelineState(const DX12PipelineState&) = delete;
        DX12PipelineState& operator=(const DX12PipelineState&) = delete;
        DX12PipelineState& operator=(DX12PipelineState&&) = delete;

    public:
        DX12PipelineState(const std::string&);
        DX12PipelineState(DX12PipelineState&&);
        ~DX12PipelineState();

        // device has already been locked from context
        bool create(const std::shared_ptr<DX12Device>&);

    private:
        struct Intermediate
        {
            std::string rootSignature;
        };

        Microsoft::WRL::ComPtr<ID3D12PipelineState>	state_;
        std::unique_ptr<Intermediate> intermediate_;
    };
} // namespace Takoyaki