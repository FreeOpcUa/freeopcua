/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Io manager errors declarations
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __errors_h__34bbdf66_921f_443e_90cf_2ac258dd0458
#define __errors_h__34bbdf66_921f_443e_90cf_2ac258dd0458

#include <opccore/common/exception.h>

#define DEFINE_IO_MANAGER_ERROR(name) extern Common::ErrorData name;

DEFINE_IO_MANAGER_ERROR(DeviceManagerNotRegisteredForObjectId);
DEFINE_IO_MANAGER_ERROR(ObjectNotRegisteredInRequest);
DEFINE_IO_MANAGER_ERROR(DeviceManagerDoesntProcessData);

#endif // __errors_h__34bbdf66_921f_443e_90cf_2ac258dd0458
