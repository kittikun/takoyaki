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

#include "app.h"

#include <array>

#include <d3d12.h>  // will be removed later once Vulkan abstraction has been added
#include <glm/gtc/matrix_transform.hpp>
#include <takoyaki.h>

#include <DirectXMath.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
};

void App::initialize(Takoyaki::Framework* framework)
{
    auto renderer = framework->getRenderer();

    // first compile shaders for this sample
    std::array<Takoyaki::ShaderDesc, 2> shDescs;

    shDescs[0].name = "SimpleVS";
    shDescs[0].path = "data/SimpleVS.hlsl";
    shDescs[0].type = Takoyaki::EShaderType::VERTEX;
    shDescs[0].entry = "main";
    shDescs[1].name = "SimplePS";
    shDescs[1].path = "data/SimplePS.hlsl";
    shDescs[1].type = Takoyaki::EShaderType::PIXEL;
    shDescs[1].entry = "main";

    for (auto& shDesc : shDescs)
        framework->compileShader(shDesc);

    // create vertex layout
    auto layout = renderer->createInputLayout("SimpleVertex");
    layout->addInput("POSITION", Takoyaki::EFormat::R32G32B32_FLOAT, 0);
    layout->addInput("COLOR", Takoyaki::EFormat::R32G32B32_FLOAT, 0);

    // create root signature that will be using the input assembler 
    // and only allow the constant buffer to be accessed from the vertex shader
    // NOTE: will be removed later once vulkan abstraction has been added
    auto rs = renderer->createRootSignature("SimpleSignature");
    D3D12_ROOT_SIGNATURE_FLAGS rsFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    rsCBIndex_ = rs->addDescriptorTable();
    rs->addDescriptorRange(rsCBIndex_, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    rs->setFlags(rsFlags);

    // one pipeline state object using the previously created data
    Takoyaki::PipelineStateDesc psDesc = {};

    psDesc.inputLayout = "SimpleVertex";
    psDesc.rootSignature = "SimpleSignature";
    psDesc.shaders[Takoyaki::EShaderType::VERTEX] = shDescs[0].name;
    psDesc.shaders[Takoyaki::EShaderType::PIXEL] = shDescs[1].name;
    psDesc.depthStencilState.depthEnable = false;
    psDesc.formatRenderTarget[0] = Takoyaki::EFormat::B8G8R8A8_UNORM;
    psDesc.numRenderTargets = 1;
    psDesc.topology = Takoyaki::ETopology::TRIANGLE;
    renderer->createPipelineState("SimpleState", psDesc);

    // compile PSO, this only needs to be called once for all your PSO
    renderer->compilePipelineStateObjects();

    // cube vertices (pos, color)
    std::array<Vertex, 8> cubeVertices =
    { {
        { glm::vec3{ -0.5f, -0.5f, -0.5f }, glm::vec3{ 0.0f, 0.0f, 0.0f } },
        { glm::vec3{ -0.5f, -0.5f,  0.5f }, glm::vec3{ 0.0f, 0.0f, 1.0f } },
        { glm::vec3{ -0.5f,  0.5f, -0.5f }, glm::vec3{ 0.0f, 1.0f, 0.0f } },
        { glm::vec3{ -0.5f,  0.5f,  0.5f }, glm::vec3{ 0.0f, 1.0f, 1.0f } },
        { glm::vec3{ 0.5f, -0.5f, -0.5f }, glm::vec3{ 1.0f, 0.0f, 0.0f } },
        { glm::vec3{ 0.5f, -0.5f,  0.5f }, glm::vec3{ 1.0f, 0.0f, 1.0f } },
        { glm::vec3{ 0.5f,  0.5f, -0.5f }, glm::vec3{ 1.0f, 1.0f, 0.0f } },
        { glm::vec3{ 0.5f,  0.5f,  0.5f }, glm::vec3{ 1.0f, 1.0f, 1.0f } },
        } };

    // cube indices as triangle list
    std::array<uint16_t, 36> cubeIndices =
    {
        0, 2, 1, // -x
        1, 2, 3,

        4, 5, 6, // +x
        5, 7, 6,

        0, 1, 5, // -y
        0, 5, 4,

        2, 6, 7, // +y
        2, 7, 3,

        0, 4, 6, // -z
        0, 6, 2,

        1, 3, 7, // +z
        1, 7, 5,
    };

    vertexBuffer_ = std::move(renderer->createVertexBuffer(reinterpret_cast<uint8_t*>(&cubeVertices[0]), sizeof(Vertex), static_cast<uint_fast32_t>(cubeVertices.size() * sizeof(Vertex))));
    indexBuffer_ = std::move(renderer->createIndexBuffer(reinterpret_cast<uint8_t*>(&cubeIndices.front()), Takoyaki::EFormat::R16_UINT, static_cast<uint_fast32_t>(cubeIndices.size() * sizeof(uint16_t))));

    auto size = framework->getWindowSize();
    viewport_ = { 0, 0, size.x, size.y };
    scissor_ = { 0, 0, static_cast<uint_fast32_t>(viewport_.x), static_cast<uint_fast32_t>(viewport_.y) };
}

void App::render(Takoyaki::Renderer* renderer)
{
    // Work sent to GPU actually happens here
    auto cmd = renderer->createCommand();

    cmd->setRootSignature("SimpleSignature");
    cmd->setRootSignatureConstantBuffer(rsCBIndex_, "ModelViewProjectionConstantBuffer");

    cmd->setViewport(viewport_);
    cmd->setScissor(scissor_);

    cmd->setDefaultRenderTarget();
    cmd->clearRenderTarget(glm::vec4{ 0.f, 0.f, 1.f, 1.f });

    cmd->drawIndexedInstanced();
}

void App::update(Takoyaki::Renderer* renderer, Takoyaki::Framework* framework)
{
    // perspective matrix
    float fov = glm::radians(70.0f);
    auto perspective = glm::perspectiveFovLH(fov, viewport_.z, viewport_.w, 1.0f, 100.0f);

    // lookat
    auto eye = glm::vec3(0.0f, 0.7f, 1.5f);
    auto at = glm::vec3(0.0f, -0.1f, 0.0f);
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    auto lookAt = glm::lookAtLH(eye, at, up);
    
    // update constant buffer
    auto mvp = renderer->getConstantBuffer("ModelViewProjectionConstantBuffer");

    // constant buffer might by empty if shader hasn't been loaded yet
    if (mvp) {
        mvp->setMatrix4x4("model", glm::mat4(1.0f));
        mvp->setMatrix4x4("projection", perspective);
        mvp->setMatrix4x4("view", lookAt);
    }
}