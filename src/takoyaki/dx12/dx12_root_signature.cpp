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
#include "dx12_root_signature.h"

#include "descriptor_ranges.h"

namespace Takoyaki
{

    DX12RootSignature::DX12RootSignature(D3D12_ROOT_SIGNATURE_FLAGS flags)
        : flags_(flags_)
        , size_(0)
    {
    }

    DX12RootSignature::~DX12RootSignature() = default;

    void DX12RootSignature::addConstant(uint_fast32_t numValues, uint_fast32_t shaderRegister, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        param.ShaderVisibility = visibility;
        param.Constants.Num32BitValues = numValues;
        param.Constants.ShaderRegister = shaderRegister;
        param.Constants.RegisterSpace = 0;

        // Root constants cost 1 DWORD each, since they are 32-bit values.
        size_++;

        params_.push_back(std::move(param));
        size_ += numValues * 4;
    }

    void DX12RootSignature::addDescriptorConstantBuffer(uint_fast32_t shaderRegister, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        param.ShaderVisibility = visibility;
        param.Descriptor.ShaderRegister = shaderRegister;

        // Root descriptors (64-bit GPU virtual addresses) cost 2 DWORDs each.
        size_ += 2;

        params_.push_back(std::move(param));
    }

    void DX12RootSignature::addDescriptorUnorderedAccess(uint_fast32_t shaderRegister, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        param.ShaderVisibility = visibility;
        param.Descriptor.ShaderRegister = shaderRegister;

        // Root descriptors (64-bit GPU virtual addresses) cost 2 DWORDs each.
        size_ += 2;

        params_.push_back(std::move(param));
    }

    void DX12RootSignature::addDescriptorShaderResource(uint_fast32_t shaderRegister, D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        param.ShaderVisibility = visibility;
        param.Descriptor.ShaderRegister = shaderRegister;

        // Root descriptors (64-bit GPU virtual addresses) cost 2 DWORDs each.
        size_ += 2;

        params_.push_back(std::move(param));
    }

    uint_fast32_t DX12RootSignature::addDescriptorTable(D3D12_SHADER_VISIBILITY visibility)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = visibility;

        // Descriptor tables cost 1 DWORD each.
        size_ ++;

        uint_fast32_t res = static_cast<uint_fast32_t>(ranges_.size());

        ranges_.resize(res + 1);

        // hide index in ranges_ here
        param.DescriptorTable.NumDescriptorRanges = res;

        params_.push_back(std::move(param));

        return static_cast<uint_fast32_t>(res);
    }

    void DX12RootSignature::addDescriptorRange(uint_fast32_t index, D3D12_DESCRIPTOR_RANGE_TYPE type, uint_fast32_t numDescriptors, uint_fast32_t baseShaderRegister)
    {
        auto& range = ranges_[index];

        range.add(type, numDescriptors, baseShaderRegister);
    }

    void DX12RootSignature::create()
    {
        assert(size_ <= 64);
        
        // the descriptors tables should be fully defined by now
        for (auto& param : params_) {
            if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
                auto& range = ranges_[param.DescriptorTable.NumDescriptorRanges];

                param.DescriptorTable.NumDescriptorRanges = range.size();
                param.DescriptorTable.pDescriptorRanges = range.getDescs();
            }
        }

        D3D12_ROOT_SIGNATURE_DESC sigDesc;

        sigDesc.NumParameters = static_cast<uint_fast32_t>(params_.size());
        sigDesc.pParameters = &params_.front();
        sigDesc.Flags = flags_;

        // TODO: add sampler support
        sigDesc.NumStaticSamplers = 0;
        sigDesc.pStaticSamplers = nullptr;
    }
} // namespace Takoyaki
