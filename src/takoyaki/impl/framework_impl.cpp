// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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
#include "framework_impl.h"

#include "../utility/log.h"
#include "../dx12/DX12Device.h"

using namespace Microsoft::WRL;

namespace Takoyaki
{
    FrameworkImpl::FrameworkImpl()
    {
        Log::Initialize();
    }

    FrameworkImpl::~FrameworkImpl()
    {

    }

    void FrameworkImpl::initialize(const FrameworkDesc& desc)
    {
        LOGC_INDENT_START << "Initializing Takoyaki FrameworkImpl..";

        if (desc.type == EDeviceType::DX12) {
            device_.reset(new DX12Device());
        }

        device_->create(desc);

        LOGC_INDENT_END << "Initialization complete.";
    }

    void FrameworkImpl::setProperty(EPropertyID id, const boost::any& value)
    {
        switch (id) {
            case Takoyaki::EPropertyID::WINDOW_SIZE:
            case Takoyaki::EPropertyID::WINDOW_ORIENTATION:
            case Takoyaki::EPropertyID::WINDOW_DPI:
                device_->setProperty(id, value);
                break;

            default:
                throw new std::runtime_error("FrameworkImpl::setProperty, id");
                break;
        }
    }

    void FrameworkImpl::terminate()
    {

    }

    void FrameworkImpl::validateDevice() const
    {
        device_->validate();
    }
}
// namespace Takoyaki