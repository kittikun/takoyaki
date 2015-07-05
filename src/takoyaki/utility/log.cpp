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

#include "log.h"

#include <iostream>
#include <boost/date_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

namespace Takoyaki
{
	namespace Log
	{
		BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", ELogLevel);

		std::ostream& operator<<(std::ostream& strm, ELogLevel level)
		{
			static const char* strings[] =
			{
				"Core",
				"Dominion",
				"Network",
				"WARNING",
				"ERROR",
			};

			if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
				strm << strings[level];
			else
				strm << static_cast<int>(level);

			return strm;
		}

		void Initialize()
		{
			logging::add_console_log(std::cout, keywords::format = expr::format("%1%: [%2%] %3%")
				% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S")
				% severity
				% expr::message);

			logging::add_common_attributes();

#if defined(_WIN32)
			boost::shared_ptr<logging::core> core = logging::core::get();
			boost::shared_ptr<sinks::synchronous_sink< sinks::debug_output_backend>> debugSink(new sinks::synchronous_sink<sinks::debug_output_backend>());

			debugSink->set_filter(expr::is_debugger_present());
			debugSink->set_formatter(expr::format("%1%: [%2%] %3%\n")
				% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S")
				% severity
				% expr::message);

			core->add_sink(debugSink);
#endif
		}
	} // namespace Log
} // namespace Takoyaki