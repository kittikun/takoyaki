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

#include "device.h"
#include "descriptor_heap.h"
#include "dx12_buffer.h"
#include "dx12_constant_buffer.h"
#include "dx12_index_buffer.h"
#include "dx12_input_layout.h"
#include "dx12_pipeline_state.h"
#include "dx12_root_signature.h"
#include "dx12_vertex_buffer.h"
#include "dx12_texture.h"
#include "../rwlock_map.h"
#include "../thread_safe_stack.h"
#include "../thread_safe_queue.h"
#include "../public/definitions.h"

namespace Takoyaki
{
    class ThreadPool;
    struct CommandDesc;

    class DX12Context
    {
        DX12Context(const DX12Context&) = delete;
        DX12Context& operator=(const DX12Context&) = delete;
        DX12Context(DX12Context&&) = delete;
        DX12Context& operator=(DX12Context&&) = delete;

    public:
        enum class EResourceType
        {
            INDEX_BUFFER,
            VERTEX_BUFFER
        };

        using DescriptorHeapRTV = DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_RTV>;
        using DescriptorHeapSRV = DX12DescriptorHeapCollection<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>;
        using InputLayoutReturn = std::pair<DX12InputLayout&, std::shared_lock<std::shared_timed_mutex>>;
        using ConstantBufferReturn = boost::optional<std::pair<DX12ConstantBuffer&, std::shared_lock<std::shared_timed_mutex>>>;
        using PipelineStateReturn = std::pair<DX12PipelineState&, std::shared_lock<std::shared_timed_mutex>>;
        using RootSignatureReturn = std::pair<DX12RootSignature&, std::shared_lock<std::shared_timed_mutex>>;

        DX12Context(const std::shared_ptr<DX12Device>&, const std::shared_ptr<ThreadPool>&);
        ~DX12Context() = default;

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:

        // command creation
        bool buildCommand(const CommandDesc&, TaskCommand*);

        // resource creation
        void addShader(EShaderType, const std::string&, D3D12_SHADER_BYTECODE&&);
        DX12ConstantBuffer& createConstanBuffer(const std::string&, uint_fast32_t);
        void createTexture(uint_fast32_t);

        // Get
        inline DescriptorHeapRTV& getRTVDescHeapCollection() { return descHeapRTV_; }
        inline DescriptorHeapSRV& getSRVDescHeapCollection() { return descHeapSRV_; }

        // WARNING: will yield until shader is created
        inline D3D12_SHADER_BYTECODE getShader(EShaderType type, const std::string& name) { return getShaderImpl(shaders_[type], name); }

        //////////////////////////////////////////////////////////////////////////
        // Internal & External

        void createBuffer(EResourceType, uint_fast32_t, uint8_t*, EFormat, uint_fast32_t, uint_fast32_t);
        void createInputLayout(const std::string&);
        void createPipelineState(const std::string&, const PipelineStateDesc&);
        void createRootSignature(const std::string&);

        void destroyDone();
        bool destroyMain(void*, void*);
        void destroyResource(EResourceType, uint_fast32_t);

        const DX12IndexBuffer& getIndexBuffer(uint_fast32_t);
        auto getInputLayout(const std::string&) -> InputLayoutReturn;
        auto getPipelineState(const std::string&) -> PipelineStateReturn;
        auto getRootSignature(const std::string&) -> RootSignatureReturn;
        DX12Texture& getTexture(uint_fast32_t);
        const DX12VertexBuffer& getVertexBuffer(uint_fast32_t);

        //////////////////////////////////////////////////////////////////////////
        // External usage: 

        void compilePipelineStateObjects();
        auto getConstantBuffer(const std::string&) -> ConstantBufferReturn;

    private:
        D3D12_SHADER_BYTECODE getShaderImpl(RWLockMap<std::string, D3D12_SHADER_BYTECODE>&, const std::string&);
        void compileMain(const std::string& name);

    private:
        std::shared_ptr<DX12Device> device_;
        std::shared_ptr<ThreadPool> threadPool_;

        DescriptorHeapRTV descHeapRTV_;
        DescriptorHeapSRV descHeapSRV_;

        RWLockMap<std::string, DX12ConstantBuffer> constantBuffers_;
        RWLockMap<uint_fast32_t, DX12IndexBuffer> indexBuffers_;
        RWLockMap<std::string, DX12InputLayout> inputLayouts_;
        RWLockMap<std::string, DX12PipelineState> pipelineStates_;
        RWLockMap<std::string, DX12RootSignature> rootSignatures_;
        RWLockMap<uint_fast32_t, DX12Texture> textures_;
        RWLockMap<uint_fast32_t, DX12VertexBuffer> vertexBuffers_;

        // use multiples maps to allow same name in different categories
        using ShaderMap = RWLockMap<std::string, D3D12_SHADER_BYTECODE>;
        std::unordered_map<EShaderType, ShaderMap> shaders_;

        // resource destruction have to be handled by the context
        using DestroyQueueType = ThreadSafeQueue<std::pair<EResourceType, uint_fast32_t>>;
        DestroyQueueType destroyQueue_;
    };
} // namespace Takoyaki