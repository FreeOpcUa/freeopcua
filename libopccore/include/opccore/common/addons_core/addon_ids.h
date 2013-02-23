/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Identifiers of managers
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __COMMON_ADDONS_IDS_H__
#define __COMMON_ADDONS_IDS_H__

#include <opccore/common/addons_core/addon.h>

namespace Common
{
  enum AddonID
  {
    ADDON_ID_PROPERTY_TREE,
    ADDON_ID_DEVICE_IO_MANAGER,
    ADDON_ID_ERROR_MANAGER,
    ADDON_ID_IO_MANAGER,
    
    ADDON_ID_MIN_USER = 0x8000,
  };
} // namespace Common

#endif // __COMMON_ADDONS_IDS_H__

