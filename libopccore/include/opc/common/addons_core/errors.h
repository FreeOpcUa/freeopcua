/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Errors of Addons Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __errors_h__633b7b11_4f77_424d_8f9d_b8057a779c53
#define __errors_h__633b7b11_4f77_424d_8f9d_b8057a779c53

#include <opc/common/exception.h>

#define DEFINE_ADDONS_MANAGER_ERROR(name) extern Common::ErrorData name

DEFINE_ADDONS_MANAGER_ERROR(UnableToRegisterAddonWhenStarted);
DEFINE_ADDONS_MANAGER_ERROR(AddonsManagerAlreadyStarted);
DEFINE_ADDONS_MANAGER_ERROR(AddonsManagerAlreadyStopped);
DEFINE_ADDONS_MANAGER_ERROR(AddonNotFound);
DEFINE_ADDONS_MANAGER_ERROR(AddonNotInitializedYet);
DEFINE_ADDONS_MANAGER_ERROR(AddonNotRegistered);
DEFINE_ADDONS_MANAGER_ERROR(AddonRegisteredButShouldnt);
DEFINE_ADDONS_MANAGER_ERROR(AddonIsNotStarted);
DEFINE_ADDONS_MANAGER_ERROR(UnableToLoadDynamicLibrary);
DEFINE_ADDONS_MANAGER_ERROR(UnableToFundSymbolInTheLibrary);

#endif // __errors_h__633b7b11_4f77_424d_8f9d_b8057a779c53

