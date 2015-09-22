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

#include <boost/optional.hpp>
#include <d3d12.h>
#include <memory>

#include "constant_buffer.h"
#include "device.h"
#include "descriptor_heap.h"
#include "dx12_input_layout.h"
#include "dx12_pipeline_state.h"
#include "dx12_root_signature.h"
#include "texture.h"
#include "../rwlock_map.h"
#include "../thread_safe_stack.h"
#include "../public/definitions.h"

namespace Takoyaki
{
    class ThreadPool;

    class DX12Context : public std::enable_shared_from_this<DX12Context>
    {
        DX12Context(const DX12Context&) = delete;
        DX12Context& operator=(const DX12Context&) = delete;
        DX12Context(DX12Context&&) = delete;
        DX12Context& operator=(DX12Context&&) = delete;

    public:
        using DescriptorHeapRTV = DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
        using DescriptorHeapSR = DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
        using InputLayoutReturn = std::pair<DX12InputLayout&, boost::shared_lock<boost::shared_mutex>>;
        using ConstantBufferReturn = boost::optional<std::pair<DX12ConstantBuffer&, boost::shared_lock<boost::shared_mutex>>>;
        using PipelineStateReturn = std::pair<DX12PipelineState&, boost::shared_lock<boost::shared_mutex>>;
        using RootSignatureReturn = std::pair<DX12RootSignature&, boost::shared_lock<boost::shared_mutex>>;

        explicit DX12Context(const std::shared_ptr<DX12Device>&, const std::shared_ptr<ThreadPool>&);

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        // resource creation
        void addShader(EShaderType, const std::string&, D3D12_SHADER_BYTECODE&&);
        DX12ConstantBuffer& createConstanBuffer(const std::string&);
        DX12Texture& createTexture();

        // Get
        DescriptorHeapRTV& getRTVDescHeapCollection() { return descHeapRTV_; }
        DescriptorHeapSR& getSRVDescHeapCollection() { return descHeapSRV_; }
        D3D12_SHADER_BYTECODE getShader(EShaderType, const std::string&); // warning, will yield until shader is found

        //////////////////////////////////////////////////////////////////////////
        // Internal & External

        void createInputLayout(const std::string&);
        void createPipelineState(const std::string&, const PipelineStateDesc&);
        void createRootSignature(const std::string&);

        auto getInputLayout(const std::string&) -> InputLayoutReturn;
        auto getPipelineState(const std::string&) -> PipelineStateReturn;
        auto getRootSignature(const std::string&) -> RootSignatureReturn;

        //////////////////////////////////////////////////////////////////////////
        // External usage: 

        void commit();
        auto getConstantBuffer(const std::string&) -> ConstantBufferReturn;

    private:
        D3D12_SHADER_BYTECODE getShaderImpl(RWLockMap<std::string, D3D12_SHADER_BYTECODE>&, const std::string&);
        void commitMain(const std::string& name);

    private:
        std::shared_ptr<DX12Device> device_;
        std::shared_ptr<ThreadPool> threadPool_;

        DescriptorHeapRTV descHeapRTV_;
        DescriptorHeapSR descHeapSRV_;

        RWLockMap<std::string, DX12ConstantBuffer> constantBuffers_;
        RWLockMap<std::string, DX12InputLayout> inputLayouts_;
        RWLockMap<std::string, DX12PipelineState> pipelineStates_;
        RWLockMap<std::string, DX12RootSignature> rootSignatures_;

        // use multiples maps to allow same name in different categories
        RWLockMap<std::string, D3D12_SHADER_BYTECODE> shaderCompute_;
        RWLockMap<std::string, D3D12_SHADER_BYTECODE> shaderDomain_;
        RWLockMap<std::string, D3D12_SHADER_BYTECODE> shaderHull_;
        RWLockMap<std::string, D3D12_SHADER_BYTECODE> shaderGeometry_;
        RWLockMap<std::string, D3D12_SHADER_BYTECODE> shaderPixel_;
        RWLockMap<std::string, D3D12_SHADER_BYTECODE> shaderVertex_;

        ThreadSafeStack<DX12Texture> textures_;
    };
} // namespace Takoyaki