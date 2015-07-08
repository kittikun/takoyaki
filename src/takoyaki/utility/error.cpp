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

#include "error.h"

#include <array>
#include <string>
#include <unordered_map>

namespace Takoyaki
{
    namespace Utility
    {
        std::string GetWDAError()
        {
            //Get the error message, if any.
            DWORD errorMessageID = GetLastError();

            if (errorMessageID == 0)
                return "No error message has been recorded";

            LPSTR messageBuffer = nullptr;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

            std::string message(messageBuffer, size);

            //Free the buffer.
            LocalFree(messageBuffer);

            return message;
        }

        std::string GetDXGIError(HRESULT code)
        {
            std::unordered_map<HRESULT, std::string> map = {
                { DXGI_ERROR_DEVICE_HUNG, "DXGI_ERROR_DEVICE_HUNG" },
                { DXGI_ERROR_DEVICE_REMOVED, "DXGI_ERROR_DEVICE_REMOVED" },
                { DXGI_ERROR_DEVICE_RESET, "DXGI_ERROR_DEVICE_RESET" },
                { DXGI_ERROR_DRIVER_INTERNAL_ERROR, "DXGI_ERROR_DRIVER_INTERNAL_ERROR" },
                { DXGI_ERROR_FRAME_STATISTICS_DISJOINT, "DXGI_ERROR_FRAME_STATISTICS_DISJOINT" },
                { DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE, "DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE" },
                { DXGI_ERROR_INVALID_CALL, "DXGI_ERROR_INVALID_CALL" },
                { DXGI_ERROR_MORE_DATA, "DXGI_ERROR_MORE_DATA" },
                { DXGI_ERROR_NONEXCLUSIVE, "DXGI_ERROR_NONEXCLUSIVE" },
                { DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE" },
                { DXGI_ERROR_NOT_FOUND, "DXGI_ERROR_NOT_FOUND" },
                { DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED, "DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED" },
                { DXGI_ERROR_REMOTE_OUTOFMEMORY, "DXGI_ERROR_REMOTE_OUTOFMEMORY" },
                { DXGI_ERROR_WAS_STILL_DRAWING, "DXGI_ERROR_WAS_STILL_DRAWING" },
                { DXGI_ERROR_UNSUPPORTED, "DXGI_ERROR_UNSUPPORTED" },
                { DXGI_ERROR_ACCESS_LOST, "DXGI_ERROR_ACCESS_LOST" },
                { DXGI_ERROR_WAIT_TIMEOUT, "DXGI_ERROR_WAIT_TIMEOUT" },
                { DXGI_ERROR_SESSION_DISCONNECTED, "DXGI_ERROR_SESSION_DISCONNECTED" },
                { DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE, "DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE" },
                { DXGI_ERROR_CANNOT_PROTECT_CONTENT, "DXGI_ERROR_CANNOT_PROTECT_CONTENT" },
                { DXGI_ERROR_ACCESS_DENIED, "DXGI_ERROR_ACCESS_DENIED" },
                { DXGI_ERROR_NAME_ALREADY_EXISTS, "DXGI_ERROR_NAME_ALREADY_EXISTS" },
                { DXGI_ERROR_SDK_COMPONENT_MISSING, "DXGI_ERROR_SDK_COMPONENT_MISSING" },
                { S_OK ,"S_OK" }
            };

            return map[code];
        }

        std::string GetDXError(HRESULT code)
        {
            std::unordered_map<HRESULT, std::string> map = {
                { D3D11_ERROR_FILE_NOT_FOUND, "D3D11_ERROR_FILE_NOT_FOUND" },
                { D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS" },
                { D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS" },
                { D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD" },
                { DXGI_ERROR_INVALID_CALL, "DXGI_ERROR_INVALID_CALL" },
                { DXGI_ERROR_WAS_STILL_DRAWING, "DXGI_ERROR_WAS_STILL_DRAWING" },
                { E_FAIL, "E_FAIL" },
                { E_INVALIDARG, "E_INVALIDARG" },
                { E_OUTOFMEMORY, "E_OUTOFMEMORY" },
                { E_NOTIMPL, "E_NOTIMPL" },
                { S_FALSE, "S_FALSE" },
                { S_OK, "S_OK" }
            };

            return map[code];
        }

    } // namespace Utility
} // namespace Takoyaki