/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief OpcClient errors dfinitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/opc_client/errors.h>

#include <opccore/common/modules.h>

#define OPCCLIENT_ERROR_CODE(code) ERROR_CODE(Common::MODULE_IO_MANAGER, code)
#define OPCCLIENT_ERROR(name, code, message) Common::ErrorData name(OPCCLIENT_ERROR_CODE(code), message)

OPCCLIENT_ERROR(GroupAlreadyExist,     1, "Group '%1%' already exist.");
OPCCLIENT_ERROR(GroupNotFound,         2, "Group '%1%' not found.");
OPCCLIENT_ERROR(InvalidServerID,       3, "Invalid server object ID '%1%'.");
OPCCLIENT_ERROR(ItemPathNotFound,      4, "Unable to find item by path '%1%'.");
OPCCLIENT_ERROR(InvalidAccessPath,     5, "Invalid access path '%1%'.");
OPCCLIENT_ERROR(BlobsUnsupported,      6, "Using of blobs is unsupported.");
OPCCLIENT_ERROR(NothingToRead,         7, "Nothing to read.");
OPCCLIENT_ERROR(NothingToWrite,        8, "Nothing to write.");

