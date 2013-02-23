/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include <opccore/common/addons_core/errors.h>
#include <opccore/common/modules.h>

#define ADDONS_MANAGER_MODULE_ERROR_CODE(code) ERROR_CODE(Common::MODULE_ADDONS_MANAGER, code)
#define ADDONS_MANAGER_ERROR(name, code, message) Common::ErrorData name(ADDONS_MANAGER_MODULE_ERROR_CODE(code), message)

ADDONS_MANAGER_ERROR(UnableToRegisterAddonWhenStarted, 1, "Unable to register addon '%1%' with dependencies when addons manager started.");
ADDONS_MANAGER_ERROR(AddonsManagerAlreadyStarted,      2, "Addons manager already started.");
ADDONS_MANAGER_ERROR(AddonsManagerAlreadyStopped,      3, "Addons Manager already stopped.");
ADDONS_MANAGER_ERROR(AddonNotFound,                    4, "Addon with id '%1%' not found.");
ADDONS_MANAGER_ERROR(AddonNotInitializedYet,           5, "Addon with id '%1%' is not initialized yet");
ADDONS_MANAGER_ERROR(AddonNotRegistered,               6, "Addon with id '%1%' is not registered, but should.");
ADDONS_MANAGER_ERROR(AddonRegisteredButShouldnt,       7, "Addon with id '%1%' registered, but shouldn't.");
ADDONS_MANAGER_ERROR(AddonIsNotStarted,                8, "Addons with id '%1%' is not started.");

