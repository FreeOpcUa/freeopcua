/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Registrator of gps Device Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef gps_device_manager_h_fa733825_cde7_418b_97dd_b4147d2fc42
#define gps_device_manager_h_fa733825_cde7_418b_97dd_b4147d2fc42

#include <managers/device_manager/manager.h>
#include <managers/property_tree/group.h>
#include <memory>

namespace Gps
{
  /// @brief Function registers gps device manager in Common::AddonsManager
  /// Gps device manager build in property tree corresponding subtree with gps 
  /// data. After call gps device manager will be accessible via 
  /// ADDON_ID_DEVICE_MANAGER ID in addons manager.
  DeviceManager::Manager::SharedPtr CreateDeviceManager(Gefest::Group::SharedPtr rootGroup);
}

#endif // gps_device_manager_h_5fa733825_cde7_418b_97dd_b4147d2fc42

