/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "environment.h"
#include "environment/device_manager.h"
#include "../common_managers/common_managers.h"

#include <opccore/common/addons_core/addon_manager.h>


void StartEnvironment(Common::AddonsManager)
{
  RegisterDeviceManager("1", "DM1_");
  RegisterDeviceManager("2", "DM2_");
  Common::GetAddonsManager()->Start();
}
