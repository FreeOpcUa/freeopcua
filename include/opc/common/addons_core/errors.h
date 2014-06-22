/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Errors of Addons Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/exception.h>

extern Common::ErrorData UnableToRegisterAddonWhenStarted;
extern Common::ErrorData AddonsManagerAlreadyStarted;
extern Common::ErrorData AddonsManagerAlreadyStopped;
extern Common::ErrorData AddonNotFound;
extern Common::ErrorData AddonNotInitializedYet;
extern Common::ErrorData AddonNotRegistered;
extern Common::ErrorData AddonRegisteredButShouldnt;
extern Common::ErrorData AddonIsNotStarted;
extern Common::ErrorData UnableToLoadDynamicLibrary;
extern Common::ErrorData UnableToFundSymbolInTheLibrary;
extern Common::ErrorData FailedToStartAddons;
