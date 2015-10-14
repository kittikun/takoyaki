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

#include "../public/definitions.h"

namespace Takoyaki
{
    struct FrameworkDesc;
    class DX12Context;

    class DX12Device
    {
        DX12Device(const DX12Device&) = delete;
        DX12Device& operator=(const DX12Device&) = delete;
        DX12Device(DX12Device&&) = delete;
        DX12Device& operator=(DX12Device&&) = delete;

    public:
        DX12Device() noexcept;
        ~DX12Device() = default;

        void create(const FrameworkDesc& desc, std::weak_ptr<DX12Context>);
        void present();
        void setProperty(EPropertyID, const boost::any&);
        void validate();

        inline uint_fast32_t getFrameCount() const { return bufferCount_; }
        inline uint_fast32_t getCurrentFrame() const { return currentFrame_; }
        inline const glm::vec2& getWindowSize() const { return windowSize_; }

        inline const Microsoft::WRL::ComPtr<ID3D12Device>& getDXDevice() { return D3DDevice_; }
        inline const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& getCommandQueue() { return commandQueue_; }

        inline std::unique_lock<std::mutex> getDeviceLock() { return std::unique_lock<std::mutex>(deviceMutex_); }

    private:
        void createDevice(uint_fast32_t);
        void createSwapChain();
        DXGI_MODE_ROTATION getDXGIOrientation() const;
        void waitForGpu();

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> D3DDevice_;
        Microsoft::WRL::ComPtr<IDXGIFactory4> DXGIFactory_;

        // for swapchain creation
        std::weak_ptr<DX12Context> context_;

        // main command queue
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>  commandQueue_;
        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators_;

        // cpu synchronization
        std::mutex deviceMutex_;

        // gpu synchronization
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        std::vector<uint64_t> fenceValues_;
        HANDLE fenceEvent_;

        // windows related
        IUnknown* window_;
        glm::vec2 windowSize_;
        EDisplayOrientation currentOrientation_;
        EDisplayOrientation nativeOrientation_;
        float dpi_;

        // swap chain
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_;
        uint_fast32_t bufferCount_;

        // misc
        uint_fast32_t currentFrame_;
        glm::mat4x4 matDeviceRotation_;        
    };
} // namespace Takoyaki