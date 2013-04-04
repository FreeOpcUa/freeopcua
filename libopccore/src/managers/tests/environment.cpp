#include "environment.h"
#include "environment/device_manager.h"
#include "../common_managers/common_managers.h"

#include <opccore/common/addons_core/addon_manager.h>

void StartEnvironment(Common::AddonsManager)
{
  RegisterDeviceManager(static_cast<Common::AddonID>(1), "DM1_");
  RegisterDeviceManager(static_cast<Common::AddonID>(2), "DM2_");
  Common::GetAddonsManager()->Start();
}
