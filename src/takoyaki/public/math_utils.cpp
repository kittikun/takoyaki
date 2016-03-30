// Copyright(c) 2015-2016-2016 Kitti Vongsay
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

#include "pch.h"
#include "math_utils.h"

#include <DirectXMath.h>

namespace Takoyaki
{
    glm::mat4 dxMatToGLM(const DirectX::XMMATRIX& mat)
    {
        // we want to use column matrix so transpose
        const auto dx = DirectX::XMMatrixTranspose(mat);

        const auto r0 = glm::vec4(DirectX::XMVectorGetX(dx.r[0]), DirectX::XMVectorGetY(dx.r[0]), DirectX::XMVectorGetZ(dx.r[0]), DirectX::XMVectorGetW(dx.r[0]));
        const auto r1 = glm::vec4(DirectX::XMVectorGetX(dx.r[1]), DirectX::XMVectorGetY(dx.r[1]), DirectX::XMVectorGetZ(dx.r[1]), DirectX::XMVectorGetW(dx.r[1]));
        const auto r2 = glm::vec4(DirectX::XMVectorGetX(dx.r[2]), DirectX::XMVectorGetY(dx.r[2]), DirectX::XMVectorGetZ(dx.r[2]), DirectX::XMVectorGetW(dx.r[2]));
        const auto r3 = glm::vec4(DirectX::XMVectorGetX(dx.r[3]), DirectX::XMVectorGetY(dx.r[3]), DirectX::XMVectorGetZ(dx.r[3]), DirectX::XMVectorGetW(dx.r[3]));

        return glm::mat4(r0, r1, r2, r3);
    }

    glm::mat4 perspectiveFovLH(float fov, float width, float height, float zNear, float zFar)
    {
        return dxMatToGLM(DirectX::XMMatrixPerspectiveFovLH(fov, width / height, zNear, zFar));
    }

    glm::mat4 lookAtLH(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
    {
        const DirectX::XMVECTORF32 dxEye = { eye.x, eye.y, eye.z, 0.0f };
        const DirectX::XMVECTORF32 dxAt = { at.x, at.y, at.z, 0.0f };
        const DirectX::XMVECTORF32 dxUp = { up.x, up.y, up.z, 0.0f };

        return dxMatToGLM(DirectX::XMMatrixLookAtLH(dxEye, dxAt, dxUp));
    }
}