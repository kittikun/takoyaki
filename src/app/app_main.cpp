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

#include <memory>

#include <constant_table.h>
#include <d3d12.h>
#include <framework.h>
#include <glm/glm.hpp>
#include <input_layout.h>
#include <render_component.h>
#include <root_signature.h>

class Test final : public Takoyaki::RenderComponent
{
    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;
    Test(Test&&) = delete;
    Test& operator=(Test&&) = delete;
public:
    Test::Test() = default;

    void render(Takoyaki::Renderer& renderer)
    {
        auto viewProj = renderer.getConstantBuffer("CBModelViewProjection");      
        Test test;

        // cbuffer might by empty is shader hasn't been loaded yet
        if (viewProj) {
            viewProj->setMatrix4x4("matViewProjection", glm::mat4x4());
        }

        int i = 0;
    }
};

void appMain(std::weak_ptr<Takoyaki::Framework> frmwk)
{
    auto renderer = frmwk.lock()->getRenderer();

    // create root signature
    auto rs = renderer->createRootSignature("SimpleSignature");
    D3D12_ROOT_SIGNATURE_FLAGS rsFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    auto index = rs->addDescriptorTable();
    rs->addDescriptorRange(index, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    rs->setFlags(rsFlags);

    renderer->commit();

    // create vertex layout
    auto layout = renderer->createInputLayout("SimpleVertex");
    layout->addInput("POSITION", Takoyaki::EFormat::R32G32B32_FLOAT, 0);
    layout->addInput("COLOR", Takoyaki::EFormat::R32G32B32_FLOAT, 0);

    renderer->addRenderComponent(std::make_unique<Test>());

}