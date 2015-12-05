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

namespace Takoyaki
{
    struct CommandDesc;
    class DX12Context;
    class DX12Device;
    struct TaskCommand;

    class DX12CommandBuilder
    {
        DX12CommandBuilder(const DX12CommandBuilder&) = delete;
        DX12CommandBuilder& operator=(const DX12CommandBuilder&) = delete;
        DX12CommandBuilder(DX12CommandBuilder&&) = delete;
        DX12CommandBuilder& operator=(DX12CommandBuilder&&) = delete;

    public:
        DX12CommandBuilder(DX12Context*, DX12Device*);
        ~DX12CommandBuilder() = default;

        bool buildCommand(const CommandDesc&, TaskCommand*);

    private:
        DX12Context* context_;

        // context will own a shared_ptr so we don't need to worry about it here
        DX12Device* device_;
    };
}
