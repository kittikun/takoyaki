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
#include <memory>

namespace Takoyaki
{
    class DX12Context;

    class DX12Texture
    {
        DX12Texture(const DX12Texture&) = delete;
        DX12Texture& operator=(const DX12Texture&) = delete;
        DX12Texture& operator=(DX12Texture&&) = delete;

    public:
        DX12Texture(std::weak_ptr<DX12Context>);
        DX12Texture(DX12Texture&&) noexcept;
        ~DX12Texture();

        // For external initialization, swap buffer only ?
        Microsoft::WRL::ComPtr<ID3D12Resource>& getResource() { return resource_; }
        const D3D12_CPU_DESCRIPTOR_HANDLE& getRenderTargetView();

    private:
        std::weak_ptr<DX12Context> owner_;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_;
    };
} // namespace Takoyaki