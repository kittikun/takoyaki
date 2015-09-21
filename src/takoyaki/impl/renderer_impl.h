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

#include <memory>

namespace Takoyaki
{
    class ConstantTableImpl;
    class InputLayoutImpl;
    class DX12DeviceContext;
    class RenderComponent;
    class RootSignatureImpl;
    class ThreadPool;

    class RendererImpl : public std::enable_shared_from_this<RendererImpl>
    {
        RendererImpl(const RendererImpl&) = delete;
        RendererImpl& operator=(const RendererImpl&) = delete;
        RendererImpl(RendererImpl&&) = delete;
        RendererImpl& operator=(RendererImpl&&) = delete;

    public:
        RendererImpl(const std::shared_ptr<DX12DeviceContext>&, const std::shared_ptr<ThreadPool>&);
        ~RendererImpl();

        //////////////////////////////////////////////////////////////////////////
        // Internal usage:
        void processComponents();

        //////////////////////////////////////////////////////////////////////////
        // External usage: 

        void addRenderComponent(std::shared_ptr<RenderComponent>&&);

        std::unique_ptr<InputLayoutImpl> createInputLayout(const std::string&);
        std::unique_ptr<RootSignatureImpl> createRootSignature(const std::string&);

        void commit();

        std::unique_ptr<ConstantTableImpl> getConstantBuffer(const std::string&);

    private:
        void commitMain();

    private:
        std::shared_ptr<DX12DeviceContext> context_;
        std::shared_ptr<ThreadPool> threadPool_;
        std::vector<std::shared_ptr<RenderComponent>> renderable_;
    };
}
// namespace Takoyaki
