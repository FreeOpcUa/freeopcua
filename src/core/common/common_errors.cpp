/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors definitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/errors.h>
#include <opc/common/modules.h>

#define COMMON_ERROR(name, code, message) Common::ErrorData name(ERROR_CODE(Common::MODULE_COMMON, code), message)

COMMON_ERROR(InvalidValueType,    1, "Invalid value type '%1%'.");
COMMON_ERROR(UnsupportedFeature,  2, "Unsupported.");
COMMON_ERROR(NotImplemented,      3, "Not implemented.");
COMMON_ERROR(StdException,        4, "Standard exception '%1%'.");
COMMON_ERROR(CannotParseUri,      5, "Cannot parse uri '%1%'.");
COMMON_ERROR(FailedToStartAddons, 6, "Cannot start addons.");

