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

#pragma once

#ifdef _DEBUG

#define LOG_IDENTIFY_THREAD BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", std::this_thread::get_id());

#define LOGC BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Core) << Takoyaki::Log::GetIndent(Takoyaki::Log::Log_Core) 
#define LOGE BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Error)
#define LOGS BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Shader) << Takoyaki::Log::GetIndent(Takoyaki::Log::Log_Shader) 
#define LOGW BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Warning)

#define LOGC_INDENT_START Takoyaki::Log::StartIndent(Takoyaki::Log::Log_Core); ## LOGC
#define LOGS_INDENT_START Takoyaki::Log::StartIndent(Takoyaki::Log::Log_Shader); ## LOGS

#define LOGC_INDENT_END Takoyaki::Log::EndIndent(Takoyaki::Log::Log_Core); ## LOGC
#define LOGS_INDENT_END Takoyaki::Log::EndIndent(Takoyaki::Log::Log_Shader); ## LOGS

namespace Takoyaki
{
    namespace Log
    {
        enum ELogLevel
        {
            Log_Core,
            Log_Error,
            Log_Shader,
            Log_Warning,
            Log_Level_Count
        };

        BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(boost_log, boost::log::sources::severity_logger_mt<ELogLevel>);

        void Initialize();
        void StartIndent(ELogLevel);
        void EndIndent(ELogLevel);
        std::string GetIndent(ELogLevel);
    } // namespace Log
} // namespace Takoyaki

#else
#include <iostream>

#define LOG_IDENTIFY_THREAD
#define LOGC std::cout
#define LOGE std::cout
#define LOGS std::cout
#define LOGW std::cout
#define LOGC_INDENT_START std::cout 
#define LOGS_INDENT_START std::cout 
#define LOGC_INDENT_END std::cout
#define LOGS_INDENT_END std::cout

#endif