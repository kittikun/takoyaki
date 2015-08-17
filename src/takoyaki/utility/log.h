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

#pragma once

#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>

#define LOGC BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Core) << Takoyaki::Log::GetIndent(Takoyaki::Log::Log_Core) 
#define LOGW BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Warning)
#define LOGE BOOST_LOG_SEV(Takoyaki::Log::boost_log::get(), Takoyaki::Log::Log_Error)

#define LOGC_INDENT_START Takoyaki::Log::StartIndent(Takoyaki::Log::Log_Core); ## LOGC

#define LOGC_INDENT_END Takoyaki::Log::EndIndent(Takoyaki::Log::Log_Core); ## LOGC

namespace Takoyaki
{
	namespace Log
	{
		enum ELogLevel
		{
			Log_Core,
			Log_Warning,
			Log_Error,
            Log_Level_Count
		};

		BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(boost_log, boost::log::sources::severity_logger_mt<ELogLevel>);

		void Initialize();
        void StartIndent(ELogLevel);
        void EndIndent(ELogLevel);
        std::string GetIndent(ELogLevel);
	} // namespace Log
} // namespace Takoyaki
