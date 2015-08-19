// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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
#include "DX12DescriptorHeap.h"

#include "DX12Device.h"
#include "utility.h"

#define MAX_DESCRIPTORS 128

namespace Takoyaki
{
    DescriptorHeap::DescriptorHeap(EDescriptorHeapType type, std::weak_ptr<DX12Device> device, uint_fast32_t id)
        : device_(device_)
        , count_(0)
    {
        auto locked = device.lock();
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};

        desc.NumDescriptors = MAX_DESCRIPTORS;

        switch (type) {
            case EDescriptorHeapType::DEPTHSTENCILVIEW:
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                descriptorSize_ = locked->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                break;

            case EDescriptorHeapType::SHADER_RESOURCE:
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                descriptorSize_ = locked->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                break;

            case EDescriptorHeapType::RENDERTARGETVIEW:
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                descriptorSize_ = locked->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                break;

            case EDescriptorHeapType::SAMPLER:
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                descriptorSize_ = locked->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                break;

            default:
                throw std::runtime_error("displayRotation");
                break;
        }

        DXCheckThrow(locked->getDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap_)));
        heap_->SetName(L"Render target view descriptor heap");
        handle_ = heap_->GetCPUDescriptorHandleForHeapStart();
    }

    DescriptorHeap::~DescriptorHeap() = default;

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Create()
    {        
        D3D12_CPU_DESCRIPTOR_HANDLE res;
        
        res.ptr = handle_.ptr + count_ * descriptorSize_;

        ++count_;

        return res;
    }
} // namespace Takoyaki