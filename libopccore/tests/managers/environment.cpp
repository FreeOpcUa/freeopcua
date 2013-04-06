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
