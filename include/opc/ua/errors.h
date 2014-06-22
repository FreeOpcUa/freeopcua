/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors declarations
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/exception.h>

#define DEFINE_COMMON_ERROR(name) extern Common::ErrorData name;

DEFINE_COMMON_ERROR(CannotCreateChannelOnInvalidSocket);

