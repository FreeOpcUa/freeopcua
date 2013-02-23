/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Registrator of test Device Manager
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __addon_h_d175728c_f6fb_4e0a_acd7_77abb8781135
#define __addon_h_d175728c_f6fb_4e0a_acd7_77abb8781135

namespace TestServer
{
  /// @brief Function registers gps device manager in Common::AddonsManager
  /// Gps device manager build in property tree corresponding subtree with gps 
  /// data. After call gps device manager will be accessible via 
  /// ADDON_ID_DEVICE_MANAGER ID in addons manager.
  void RegisterDeviceManager();
}

#endif // __addon_h_d175728c_f6fb_4e0a_acd7_77abb8781135

