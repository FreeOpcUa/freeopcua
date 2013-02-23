/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors definitions
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opccore/common/errors.h>
#include <opccore/common/modules.h>

#define COMMON_MODULE_ERROR_CODE(code) ERROR_CODE(Common::MODULE_COMMON, code)
#define COMMON_ERROR(name, code, message) Common::ErrorData name(COMMON_MODULE_ERROR_CODE(code), message)

COMMON_ERROR(InvalidValueType,   1, "Invalid value type '%1%'.");
COMMON_ERROR(UnsupportedFeature, 2, "Unsupported.");
COMMON_ERROR(NotImplemented,     3, "Not implemented.");
COMMON_ERROR(StdException,       4, "Standard exception '%1%'.");
