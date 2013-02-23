// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <common/addons_core/addon_manager.h>
#include <managers/common_managers/common_managers.h>
#include <servers/gps/device_manager/addon.h>

extern "C" const char* GetCoClassName()
{
  static const char* ClassName = "gps_opc_server";
  return ClassName;
}

extern "C" const char* GetCoClassContractId()
{
  static const char* ClassContractId = "@treww.org/opc/gps;1";
  return ClassContractId;
}

extern "C" const char* GetCoClassProgId()
{
  static const char* ClassProgId = "gps.mozopc.1";
  return ClassProgId;
}

extern "C" const char* GetCoClassGuid()
{
  static const char* ClassGuid = "42c6bddc-4431-4dcf-8d99-eac5c180c72f";
  return ClassGuid;
}

extern "C" void RegisterServerManagers()
{
  Gps::RegisterDeviceManager();
}
