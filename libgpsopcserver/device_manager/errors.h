/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors declarations
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __errors_h__29cb083f_cca6_4bcc_b861_c8dde214fd9c
#define __errors_h__29cb083f_cca6_4bcc_b861_c8dde214fd9c

#include <common/exception.h>

#define DEFINE_GPS_DEVICE_IO_MANAGER_ERROR(name) extern Common::ErrorData name;

DEFINE_GPS_DEVICE_IO_MANAGER_ERROR(GpsInvalidItemID);
DEFINE_GPS_DEVICE_IO_MANAGER_ERROR(GpsDeviceIsReadonly);


#endif // __errors_h__29cb083f_cca6_4bcc_b861_c8dde214fd9c

