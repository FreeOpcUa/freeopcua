/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Registrator of gps Device Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef _device_manager_h_924038eb_558e_4308_919f_7e062c12027b
#define _device_manager_h_924038eb_558e_4308_919f_7e062c12027b

#include <opccore/managers/device_manager/manager.h>
#include <opccore/managers/property_tree/group.h>


namespace TestServer
{
  /// @brief Function registers gps device manager in Common::AddonsManager
  /// Gps device manager build in property tree corresponding subtree with gps 
  /// data. After call gps device manager will be accessible via 
  /// ADDON_ID_DEVICE_MANAGER ID in addons manager.
  DeviceManager::Manager::UniquePtr CreateDeviceManager(Gefest::Group::SharedPtr rootGroup);
}

#endif //  _device_manager_h_924038eb_558e_4308_919f_7e062c12027b

