// Copyright(c) 2015-2016 Kitti Vongsay
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
#include "win_utility.h"

#include "../utility/log.h"

namespace Takoyaki
{
    std::string wstrToStr(const std::wstring& wstr)
    {
        std::string res;

        if (!wstr.empty()) {
            int needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
            res.resize(needed, 0);

            WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &res[0], needed, nullptr, nullptr);
        }

        return res;
    }

    std::wstring strToWStr(const std::string& str)
    {
        std::wstring res;

        if (!str.empty()) {

            int needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
            res.resize(needed, 0);

            MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &res[0], needed);
        }

        return res;
    }

    std::wstring makeWinPath(const std::string& path)
    {
        std::wstring copy{ strToWStr(path) };

        std::replace(copy.begin(), copy.end(), '/', '\\');

        return copy;
    }

    std::string makeUnixPath(const std::wstring& path)
    {
        std::string copy{ wstrToStr(path) };

        std::replace(copy.begin(), copy.end(),'\\', '/');

        return copy;
    }

    // Following is from https://msdn.microsoft.com/en-gb/library/xcb2z8hs.aspx
    const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    void SetThreadNameWin(DWORD dwThreadID, const char* threadName)
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        } __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    void setThreadName(std::thread::native_handle_type handle, const std::string& name)
    {
        SetThreadNameWin(::GetThreadId(static_cast<HANDLE>(handle)), name.c_str());
    }
} // namespace Takoyaki