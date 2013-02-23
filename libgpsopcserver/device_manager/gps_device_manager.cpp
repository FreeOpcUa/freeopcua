/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Gps device manager realization
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "gps_device_manager.h"

#include "errors.h"
#include "gpsd.h"
#include "gps_tree_updater.h"
#include "gpsd_data_processor.h"

#include <common/addons_core/addon.h>
#include <common/addons_core/addon_ids.h>
#include <common/addons_core/addon_manager.h>
#include <managers/device_manager/manager.h>
#include <managers/property_tree/manager/manager.h>
#include <managers/property_tree/property_tree.h>

#include <iostream>

namespace
{
  class GpsDeviceManager : public DeviceManager::Manager
  {
  public:
    GpsDeviceManager(Gefest::Group::SharedPtr rootGroup)
      : RootGroup(rootGroup)
      , PropertyTreeUpdater(new Gps::PropertyTreeUpdater(RootGroup))
      , Gpsd(new Gps::Gpsd)
    {
      Gpsd->SetCallback(std::shared_ptr<Gps::GpsdDataProcessor>(new Gps::GpsdDataProcessor(PropertyTreeUpdater)));
      std::cout << "GpsDeviceManager Created" << std::endl;
    }

  public: // Device Manager interface
    virtual std::set<Common::ObjectID> GetObjectIDs() const
    {
      return PropertyTreeUpdater->GetObjectIDs();
    }

    virtual void Read(DeviceManager::ObjectDataMap& data)
    {
      PropertyTreeUpdater->GetData(data);
    }

    virtual void Write(DeviceManager::ObjectDataMap& data)
    {
      THROW_ERROR(GpsDeviceIsReadonly);
    }

  private:
    const Gefest::Group::SharedPtr RootGroup;
    Gps::PropertyTreeUpdater::SharedPtr PropertyTreeUpdater;
    const Gps::Gpsd::UniquePtr Gpsd;
  };
} // unnamed namespace

namespace Gps
{
  DeviceManager::Manager::SharedPtr CreateDeviceManager(Gefest::Group::SharedPtr rootGroup)
  {
    return DeviceManager::Manager::SharedPtr(new GpsDeviceManager(rootGroup));
  }
} // namespace Gps

