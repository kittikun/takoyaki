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
#include "dx12_root_signature.h"

#include "descriptor_ranges.h"
#include "device.h"
#include "dxutility.h"

namespace Takoyaki
{
    DX12RootSignature::Intermediate::Intermediate() noexcept
        : flags{ D3D12_ROOT_SIGNATURE_FLAG_NONE }
        , size{ 0 }
    {

    }

    DX12RootSignature::DX12RootSignature() noexcept
        : intermediate_{ std::make_unique<Intermediate>() }
    {
    }

    DX12RootSignature::DX12RootSignature(DX12RootSignature&& other) noexcept
        : rootSignature_{ std::move(other.rootSignature_) }
        , intermediate_{ std::move(other.intermediate_) }
    {

    }

    DX12RootSignature::~DX12RootSignature() = default;

    void DX12RootSignature::addConstant(uint_fast32_t numValues, uint_fast32_t shaderRegister)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        param.Constants.Num32BitValues = numValues;
        param.Constants.ShaderRegister = shaderRegister;
        param.Constants.RegisterSpace = 0;

        // Root constants cost 1 DWORD each, since they are 32-bit values.
        intermediate_->size++;
        intermediate_->params.push_back(std::move(param));
    }

    void DX12RootSignature::addDescriptorConstantBuffer(uint_fast32_t shaderRegister)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        param.Descriptor.ShaderRegister = shaderRegister;

        // Root descriptors (64-bit GPU virtual addresses) cost 2 DWORDs each.
        intermediate_->size += 2;
        intermediate_->params.push_back(std::move(param));
    }

    void DX12RootSignature::addDescriptorUnorderedAccess(uint_fast32_t shaderRegister)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        param.Descriptor.ShaderRegister = shaderRegister;

        // Root descriptors (64-bit GPU virtual addresses) cost 2 DWORDs each.
        intermediate_->size += 2;
        intermediate_->params.push_back(std::move(param));
    }

    void DX12RootSignature::addDescriptorShaderResource(uint_fast32_t shaderRegister)
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        param.Descriptor.ShaderRegister = shaderRegister;

        // Root descriptors (64-bit GPU virtual addresses) cost 2 DWORDs each.
        intermediate_->size += 2;
        intermediate_->params.push_back(std::move(param));
    }

    uint_fast32_t DX12RootSignature::addDescriptorTable()
    {
        D3D12_ROOT_PARAMETER param;

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        // Descriptor tables cost 1 DWORD each.
        intermediate_->size++;

        uint_fast32_t res = static_cast<uint_fast32_t>(intermediate_->ranges.size());

        intermediate_->ranges.resize(res + 1);

        // hide index in ranges_ here
        param.DescriptorTable.NumDescriptorRanges = res;

        intermediate_->params.push_back(std::move(param));

        return static_cast<uint_fast32_t>(res);
    }

    void DX12RootSignature::addDescriptorRange(uint_fast32_t index, D3D12_DESCRIPTOR_RANGE_TYPE type, uint_fast32_t numDescriptors, uint_fast32_t baseShaderRegister)
    {
        auto& range = intermediate_->ranges[index];

        range.add(type, numDescriptors, baseShaderRegister);
    }

    bool DX12RootSignature::create(const std::shared_ptr<DX12Device>& device)
    {
        if ((intermediate_) && (intermediate_->params.size() > 0)) {
            if (intermediate_->size > 64) {
                throw new std::runtime_error{ "Root signature cannot be larger than 64 DWORD" };
            }

            // the descriptors tables should be fully defined by now
            for (auto& param : intermediate_->params) {
                if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
                    auto& range = intermediate_->ranges[param.DescriptorTable.NumDescriptorRanges];

                    param.DescriptorTable.NumDescriptorRanges = range.size();
                    param.DescriptorTable.pDescriptorRanges = range.getDescs();
                }
            }

            D3D12_ROOT_SIGNATURE_DESC desc;

            desc.NumParameters = static_cast<UINT>(intermediate_->params.size());
            desc.pParameters = &intermediate_->params.front();
            desc.Flags = intermediate_->flags;

            // TODO: add sampler support
            desc.NumStaticSamplers = 0;
            desc.pStaticSamplers = nullptr;

            Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
            Microsoft::WRL::ComPtr<ID3DBlob> pError;

            // device has already been locked from context
            DXCheckThrow(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf()));
            DXCheckThrow(device->getDXDevice()->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature_)));
            intermediate_.reset();

            return true;
        }

        return false;
    }
} // namespace Takoyaki
