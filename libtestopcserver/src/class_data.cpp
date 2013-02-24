// Author: Alexander Rykovanov 2009
//
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "addon.h"
/*
extern "C" const char* GetCoClassName()
{
  static const char* ClassName = "test_opc_server";
  return ClassName;
}

extern "C" const char* GetCoClassContractId()
{
  static const char* ClassContractId = "@treww.org/opc/test;1";
  return ClassContractId;
}

extern "C" const char* GetCoClassProgId()
{
  static const char* ClassProgId = "test.mozopc.1";
  return ClassProgId;
}

extern "C" const char* GetCoClassGuid()
{
  static const char* ClassGuid = "ed73f3ef-abeb-4677-9b3b-49e83c122ff3";
  return ClassGuid;
}
*/
extern "C" void RegisterServerManagers()
{
//  while(true);
  TestServer::RegisterDeviceManager();
}
