/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Registrator of gps Device Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef addon_h_95739103_abac_49f7_b058_c581c3d99b9d
#define addon_h_95739103_abac_49f7_b058_c581c3d99b9d

namespace Gps
{
  /// @brief Function registers gps device manager in Common::AddonsManager
  /// Gps device manager build in property tree corresponding subtree with gps 
  /// data. After call gps device manager will be accessible via 
  /// ADDON_ID_DEVICE_MANAGER ID in addons manager.
  void RegisterDeviceManager();
}

#endif // addon_h_95739103_abac_49f7_b058_c581c3d99b9d

