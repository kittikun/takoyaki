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

#include <memory>
#include <takoyaki.h>

int main(int, char**)
{
    std::unique_ptr<Takoyaki::Framework> framework;
    Takoyaki::FrameworkDesc desc = {};

    desc.bufferCount = 3;
    //desc.nativeOrientation = DisplayOrientationsToTakoyaki(disp->NativeOrientation);
    //desc.currentOrientation = DisplayOrientationsToTakoyaki(disp->CurrentOrientation);
    desc.numWorkerThreads = 4; // std::thread::hardware_concurrency();
    desc.type = Takoyaki::EDeviceType::DX12;
    //desc.windowHandle = reinterpret_cast<void*>(window);
    //desc.windowSize.x = window->Bounds.Width;
    //desc.windowSize.y = window->Bounds.Height;
    //desc.windowDpi = disp->LogicalDpi;

    framework->initialize(desc);

    return 0;
}