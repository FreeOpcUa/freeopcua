/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Registrator of gps Device Manager
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


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

