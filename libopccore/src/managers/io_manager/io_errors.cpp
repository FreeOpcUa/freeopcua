/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Io manager errors dfinitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/managers/io_manager/errors.h>
#include <opc/common/modules.h>

#define IO_MANAGER_ERROR_CODE(code) ERROR_CODE(Common::MODULE_IO_MANAGER, code)
#define IO_MANAGER_ERROR(name, code, message) Common::ErrorData name(IO_MANAGER_ERROR_CODE(code), message)

IO_MANAGER_ERROR(DeviceManagerNotRegisteredForObjectId,     1, "Unable to find DeviceManager for object '%1%'.");
IO_MANAGER_ERROR(ObjectNotRegisteredInRequest,              2, "Object with id '%1%' is not registered in the request.");
IO_MANAGER_ERROR(DeviceManagerDoesntProcessData,            3, "Unable to register Device Manager that doesn't process data.");

