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
#include "dx12_command_builder.h"

#include "dx12_context.h"
#include "dx12_device.h"
#include "dxcommon.h"
#include "../impl/command_impl.h"
#include "../utility/log.h"

namespace Takoyaki
{
    DX12CommandBuilder::DX12CommandBuilder(DX12Context* context, DX12Device* device)
        : context_{ context }
        , device_{ device }
    {
    }

    bool DX12CommandBuilder::buildCommand(const CommandDesc& desc, TaskCommand* cmd)
    {
        // at this stage shouldn't need to lock to access resources anymore
        auto frame = device_->getCurrentFrame();
        DX12Texture* rt = nullptr;

        // set a render target
        if (desc.renderTarget == UINT_FAST32_MAX) {
            // default one
            rt = device_->getRenderTarget(frame);
        } else {
            // user specified
            auto& textures = context_->getTextures();
            auto found = textures.find(desc.renderTarget);

            if (found == textures.end()) {
                auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find rendertarget \"%1%\"" } % desc.renderTarget;

                throw std::runtime_error{ boost::str(fmt) };
            }

            rt = &found->second;
        }

        D3D12_RESOURCE_STATES rtState = rt->getInitialState();

        // prepare the render target to be used
        D3D12_RESOURCE_BARRIER beforeBarrier = TransitionBarrier(rt->getResource(), rt->getInitialState(), D3D12_RESOURCE_STATE_RENDER_TARGET);

        cmd->commands->ResourceBarrier(1, &beforeBarrier);
        cmd->commands->OMSetRenderTargets(1, &rt->getRenderTargetView(), false, nullptr);
        rtState = D3D12_RESOURCE_STATE_RENDER_TARGET;

        for (auto descCmd : desc.commands) {
            switch (descCmd.first) {
                case ECommandType::CLEAR_COLOR:
                {
                    auto color = boost::any_cast<glm::vec4>(descCmd.second);
                    cmd->commands->ClearRenderTargetView(device_->getRenderTarget(frame)->getRenderTargetView(), glm::value_ptr(color), 0, nullptr);
                }
                break;

                //case ECommandType::COPY_RENDERTARGET:
                //{
                //    auto& textures = context_->getTextures();
                //    auto handle = boost::any_cast<uint_fast32_t>(descCmd.second);
                //    auto found = textures.find(handle);

                //    if (found == textures.end()) {
                //        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find destination texture \"%1%\" for copy operation" } % handle;

                //        throw std::runtime_error{ boost::str(fmt) };
                //    }

                //    D3D12_RESOURCE_BARRIER sourceBefore = TransitionBarrier(rt->getResource(), rtState, D3D12_RESOURCE_STATE_COPY_SOURCE);

                //    cmd->commands->ResourceBarrier(1, &sourceBefore);
                //    rtState = D3D12_RESOURCE_STATE_COPY_SOURCE;

                //    cmd->commands->CopyResource(found->second.getResource(), rt->getResource());
                //}
                //break;

                case ECommandType::COPY_REGION_TEXTURE2D:
                {
                    // dstTex, dstSubresource, dstOffset, srcTex, srcAreaMin, srcAreaMax
                    auto params = boost::any_cast<CopyTexRegionParams>(descCmd.second);
                    auto& textures = context_->getTextures();

                    auto dstFound = textures.find(params.dstHandle);

                    if (dstFound == textures.end()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find destination texture \"%1%\" for copy operation" } % params.dstHandle;

                        throw std::runtime_error{ boost::str(fmt) };
                    }

                    auto srcFound = textures.find(params.srcHandle);

                    if (srcFound == textures.end()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find source texture \"%1%\" for copy operation" } % params.srcHandle;

                        throw std::runtime_error{ boost::str(fmt) };
                    }

                    D3D12_TEXTURE_COPY_LOCATION dstLoc, srcLoc;

                    dstLoc.pResource = dstFound->second.getResource();
                    dstLoc.SubresourceIndex = params.dstSubresource;
                    srcLoc.pResource = srcFound->second.getResource();
                    srcLoc.SubresourceIndex = params.srcSubresource;

                    // check if empty
                    if (glm::all(glm::equal(params.srcAreaMin, glm::ivec3())) && glm::all(glm::equal(params.srcAreaMax, glm::ivec3()))) {
                        cmd->commands->CopyTextureRegion(&dstLoc, params.dstOffset.x, params.dstOffset.y, params.dstOffset.z, &srcLoc, nullptr);
                    } else {
                        D3D12_BOX srcBox;

                        // srcRect xy are top - left, zw are size
                        srcBox.left = params.srcAreaMin.x;
                        srcBox.bottom = params.srcAreaMin.y;
                        srcBox.back = params.srcAreaMin.z;

                        srcBox.right = params.srcAreaMax.x;
                        srcBox.top = params.srcAreaMax.y;
                        srcBox.front = params.srcAreaMax.z;

                        cmd->commands->CopyTextureRegion(&dstLoc, params.dstOffset.x, params.dstOffset.y, params.dstOffset.z, &srcLoc, &srcBox);
                    }
                }
                break;

                case ECommandType::DRAW_INDEXED:
                {
                    auto params = boost::any_cast<CommandDesc::DrawIndexedParams>(descCmd.second);

                    cmd->commands->DrawIndexedInstanced(std::get<0>(params), 1, std::get<1>(params), std::get<2>(params), 0);
                }
                break;

                case ECommandType::SET_INDEX_BUFFER:
                {
                    auto handle = boost::any_cast<uint_fast32_t>(descCmd.second);
                    auto& indexBuffers = context_->getIndexBuffers();
                    auto found = indexBuffers.find(handle);

                    if (found == indexBuffers.end()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find index buffer \"%1%\"" } % handle;

                        throw std::runtime_error{ boost::str(fmt) };
                    }

                    cmd->commands->IASetIndexBuffer(&found->second.getView());
                }
                break;

                case ECommandType::SET_PRIMITIVE_TOPOLOGY:
                {
                    auto topology = boost::any_cast<ETopology>(descCmd.second);

                    cmd->commands->IASetPrimitiveTopology(TopologyToDX(topology));
                }
                break;

                case ECommandType::SET_ROOT_SIGNATURE:
                {
                    auto name = boost::any_cast<std::string>(descCmd.second);
                    auto& rootSignatures = context_->getRootSignatures();
                    auto found = rootSignatures.find(name);

                    if (found == rootSignatures.end()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find root signature \"%1%\"" } % name;

                        throw std::runtime_error{ boost::str(fmt) };
                    }

                    cmd->commands->SetGraphicsRootSignature(found->second.getRootSignature());
                }
                break;

                case ECommandType::SET_ROOT_SIGNATURE_CONSTANT_BUFFER:
                {
                    auto pair = boost::any_cast<CommandDesc::RSCBParams>(descCmd.second);
                    auto& constantBuffers = context_->getConstantBuffers();
                    auto found = constantBuffers.find(pair.second);

                    if (found == constantBuffers.end()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find constant buffer \"%1%\"" } % pair.second;

                        LOGW << boost::str(fmt);
                        return false;
                    }

                    if (!found->second.isReady()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, constant buffer not ready \"%1%\"" } % pair.second;

                        LOGW << boost::str(fmt);
                        return false;
                    }

                    ID3D12DescriptorHeap* temp[] = { found->second.getHeap(frame)->descriptor.Get() };

                    cmd->commands->SetDescriptorHeaps(1, temp);
                    cmd->commands->SetGraphicsRootDescriptorTable(pair.first, found->second.getGPUView(frame));
                }
                break;

                case ECommandType::SET_SCISSOR:
                {
                    auto scissor = boost::any_cast<glm::uvec4>(descCmd.second);

                    D3D12_RECT rect = { static_cast<LONG>(scissor.x), static_cast<LONG>(scissor.y), static_cast<LONG>(scissor.z), static_cast<LONG>(scissor.w) };

                    cmd->commands->RSSetScissorRects(1, &rect);
                }
                break;

                case ECommandType::SET_VERTEX_BUFFER:
                {
                    auto handle = boost::any_cast<uint_fast32_t>(descCmd.second);
                    auto& vertexBuffers = context_->getVertexBuffers();
                    auto found = vertexBuffers.find(handle);

                    if (found == vertexBuffers.end()) {
                        auto fmt = boost::format{ "DX12DeviceContext::buildCommand, cannot find vertex buffer \"%1%\"" } % handle;

                        throw std::runtime_error{ boost::str(fmt) };
                    }

                    cmd->commands->IASetVertexBuffers(0, 1, &found->second.getView());
                }
                break;

                case ECommandType::SET_VIEWPORT:
                {
                    auto vp = boost::any_cast<glm::vec4>(descCmd.second);

                    D3D12_VIEWPORT viewport = { vp.x, vp.y, vp.z, vp.w, 0.f, 1.f };

                    cmd->commands->RSSetViewports(1, &viewport);
                }
                break;
            }
        }

        // transition back the render target to its initial state
        D3D12_RESOURCE_BARRIER afterBarrier = TransitionBarrier(rt->getResource(), rtState, rt->getInitialState());

        cmd->commands->ResourceBarrier(1, &afterBarrier);

        cmd->priority = desc.priority;
        DXCheckThrow(cmd->commands->Close());

        return true;
    }
}