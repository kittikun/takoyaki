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

#include "../test.h"

#include <memory>
#include <glm/glm.hpp>

class Test01 : public Test
{
    Test01(const Test01&) = delete;
    Test01& operator=(const Test01&) = delete;
    Test01(Test01&&) = delete;
    Test01& operator=(Test01&&) = delete;

public:
    Test01() = default;
    ~Test01() override = default;

    std::string getName() override { return "01_simple_cube"; };
    void initialize(Takoyaki::Framework*) override;
    void render(Takoyaki::Renderer*, uint_fast32_t) override;
    void update(Takoyaki::Renderer*) override;

private:
    std::unique_ptr<Takoyaki::VertexBuffer> vertexBuffer_;
    std::unique_ptr<Takoyaki::IndexBuffer> indexBuffer_;
    uint_fast32_t rsCBIndex_;
    glm::vec4 viewport_;
    glm::uvec4 scissor_;
};