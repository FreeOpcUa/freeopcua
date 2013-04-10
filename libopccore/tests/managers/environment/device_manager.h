#ifndef __device_manager_h_456bd15c_7286_44b3_ba12_fbb8490d2e16
#define __device_manager_h_456bd15c_7286_44b3_ba12_fbb8490d2e16

#include <string>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/device_manager/manager.h>

//register new device manager
// Device manager builds sbtree in property tree. All names of tree items begins with propertyTreePrefix

class IoCallback
{
public:
  virtual void OnReadData(Common::ObjectID id, DeviceManager::ObjectData& data) const = 0;
  virtual void OnWriteData(Common::ObjectID id, const DeviceManager::ObjectData& value) = 0;
  
  virtual ~IoCallback()
  {
  }
};

class TestDeviceManager : public DeviceManager::Manager
{
public:
  virtual void SetCallback(std::shared_ptr<IoCallback> callback) = 0;
  virtual std::set<Common::ObjectID> GetObjectIDs() const = 0;
};


void RegisterDeviceManager(Common::AddonsManager& manager, Common::AddonID addonID, const std::string& propertyTreePrefix);

#endif // __device_manager_h_456bd15c_7286_44b3_ba12_fbb8490d2e16
