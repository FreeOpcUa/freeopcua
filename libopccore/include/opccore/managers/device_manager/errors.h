/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors declarations for device manager
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __errors_h__5f590096_9eee_491e_861d_9c78b7537c17
#define __errors_h__5f590096_9eee_491e_861d_9c78b7537c17

#include <opccore/common/exception.h>

#define DEFINE_DEVICE_IO_MANAGER_ERROR(name) extern Common::ErrorData name;

DEFINE_DEVICE_IO_MANAGER_ERROR(DeviceManagerNotProcessItemWithID);

#endif // __errors_h__5f590096_9eee_491e_861d_9c78b7537c17


