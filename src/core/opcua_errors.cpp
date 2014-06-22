/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors definitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/errors.h>
#include <opc/common/modules.h>

#define OPCUA_CORE_MODULE_ERROR_CODE(code) ERROR_CODE(Common::MODULE_OPCUA_CORE, code)
#define OPCUA_CORE_ERROR(name, code, message) Common::ErrorData name(OPCUA_CORE_MODULE_ERROR_CODE(code), message)

OPCUA_CORE_ERROR(CannotCreateChannelOnInvalidSocket,   1, "Cannot create socket on invalid socket.");

