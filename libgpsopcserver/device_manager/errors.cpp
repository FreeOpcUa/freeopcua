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

#include "errors.h"
#include <common/modules.h>

#define GPS_DEVICE_IO_MANAGER_ERROR_CODE(code) ERROR_CODE(Common::MODULE_GPS_DEVICE_IO_MANAGER, code)
#define GPS_DEVICE_IO_MANAGER_ERROR(name, code, message) Common::ErrorData name(GPS_DEVICE_IO_MANAGER_ERROR_CODE(code), message)

GPS_DEVICE_IO_MANAGER_ERROR(GpsInvalidItemID,    1, "Invalid item id '%1%'.");
GPS_DEVICE_IO_MANAGER_ERROR(GpsDeviceIsReadonly, 2, "Gps device is readonly.");

