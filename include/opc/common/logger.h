/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/class_pointers.h>
#include <opc/spdlog/spdlog.h>
#include <opc/spdlog/fmt/ostr.h>

namespace Common
{

namespace Logger {
  DEFINE_CLASS_POINTERS(spdlog::logger)
};

}

#define LOG_TRACE(__logger__, ...) { if (__logger__ && __logger__->should_log(spdlog::level::trace)) { __logger__->trace(__VA_ARGS__); }}
#define LOG_DEBUG(__logger__, ...) { if (__logger__ && __logger__->should_log(spdlog::level::debug)) { __logger__->debug(__VA_ARGS__); }}
#define LOG_INFO(__logger__, ...) { if (__logger__ && __logger__->should_log(spdlog::level::info)) { __logger__->info(__VA_ARGS__); }}
#define LOG_WARN(__logger__, ...) { if (__logger__ && __logger__->should_log(spdlog::level::warn)) { __logger__->warn(__VA_ARGS__); }}
#define LOG_ERROR(__logger__, ...) { if (__logger__ && __logger__->should_log(spdlog::level::err)) { __logger__->error(__VA_ARGS__); }}
#define LOG_CRITICAL(__logger__, ...) { if (__logger__ && __logger__->should_log(spdlog::level::critical)) { __logger__->critical(__VA_ARGS__); }}
