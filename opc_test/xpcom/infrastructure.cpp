// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $


#include "infrastructure.h"

#include <boost/shared_array.hpp>
#include <common/exception.h>
#include <cppunit/Message.h>
#include <cppunit/Exception.h>
#include <mozilla/Module.h>
#include <mozilla/ModuleUtils.h>
#include <nsCOMPtr.h>
#include <nsIFactory.h>
#include <nsIModule.h>
#include <xpcom/mozopc.h>
#include <xpcom/src/impl/windows_utils.h>
#include <iostream>



#include "class_data.h"
#include "test_ns_iopcserver.h"




CPPUNIT_TEST_SUITE_REGISTRATION(InfrastructureTest);

DynamicLibrary::DynamicLibrary(const std::string& libraryFilename)
  : Library(dlopen(libraryFilename.c_str(), RTLD_LAZY))
{
  if (Library == NULL)
  {
    std::string msg = "Failed to load library '" + libraryFilename + "':";
    msg += dlerror();
    std::cout << msg << std::endl;
    THROW_COMMON_ERROR(1, msg.c_str());
  }
}

DynamicLibrary::~DynamicLibrary()
{
  dlclose(Library);
}

void* DynamicLibrary::find(const std::string& funcName)
{
  void* func = dlsym(Library, funcName.c_str());
  if (!func)
  {
    const std::string msg(dlerror());
    std::cout << msg << std::endl;
    THROW_COMMON_ERROR(1, msg.c_str());
  }
  return func;
}

void InfrastructureTest::setUp()
{
  if (OPCTest::GetClassData()->IsMozillaTestsDisabled()
#ifdef _WIN32
      && OPCTest::GetClassData()->IsWindowsTestsDisabled()     
#endif // _WIN32
     )
  {
    return;
  }
  CPPUNIT_ASSERT_NO_THROW(lib.reset(new DynamicLibrary(OPCTest::GetClassData()->GetFilename())));
}

void InfrastructureTest::tearDown()
{
  if (OPCTest::GetClassData()->IsMozillaTestsDisabled()
#ifdef _WIN32
      && OPCTest::GetClassData()->IsWindowsTestsDisabled()     
#endif // _WIN32
     )

  lib.reset(0);
}

// Testing creating XPCOM component
void InfrastructureTest::xpcom()
{
  if (OPCTest::GetClassData()->IsMozillaTestsDisabled())
  {
    return;
  }

  const mozilla::Module** modulePtr = lib->findFunction<const mozilla::Module**>("NSModule");
  CPPUNIT_ASSERT(modulePtr);
  nsCOMPtr<nsIModule> module(new mozilla::GenericModule(*modulePtr));
/* 
  typedef nsresult (*NSGetModuleProc)(nsIComponentManager* comMgr, nsIFile* file, nsIModule** module);
  const char* xpcomEntryPoint = "NSGetModule";

  NSGetModuleProc nsGetModule = lib->findFunction<NSGetModuleProc>(xpcomEntryPoint);
  CPPUNIT_ASSERT(nsGetModule != NULL);
  
  nsCOMPtr<nsIModule> module;
*/
//  nsCOMPtr<nsIComponentManager> compMgr(new nsComponentManager);
  nsCOMPtr<nsIComponentManager> compMgr;
  NS_GetComponentManager(getter_AddRefs(compMgr));
  CPPUNIT_ASSERT(compMgr);
/*
  CPPUNIT_ASSERT(NS_SUCCEEDED(nsGetModule(compMgr.get(), NULL, getter_AddRefs(module))));
  CPPUNIT_ASSERT(module.get() != NULL);
*/  
  nsCOMPtr<nsIFactory> opcFactory;
  nsCID id;
  CPPUNIT_ASSERT(id.Parse(OPCTest::GetClassData()->GetClsId().c_str()) == PR_TRUE);
  CPPUNIT_ASSERT(NS_SUCCEEDED(module->GetClassObject(compMgr.get(), id, NS_GET_IID(nsIFactory), getter_AddRefs(opcFactory))));
  CPPUNIT_ASSERT(opcFactory.get() != NULL);

  nsCOMPtr<nsIOPCServer> opcServer;
  CPPUNIT_ASSERT(NS_SUCCEEDED(opcFactory->CreateInstance(NULL, NS_GET_IID(nsISupports), getter_AddRefs(opcServer))));
  CPPUNIT_ASSERT(opcServer.get() != NULL);
  sleep(2);
  Test_nsIOPCServer(*opcServer);
}


#ifdef _WIN32

inline void ThrowLastWindowsError()
{
  throw CPPUNIT_NS::Exception(CPPUNIT_NS::Message(Windows::GetLastErrorString()));    
}

inline void ThrowException(const std::string& msg)
{
  throw CPPUNIT_NS::Exception(CPPUNIT_NS::Message(msg));    
}


template <const HKEY RootKey>
class RegistryKey
{
public:
  RegistryKey()
    : Key(RootKey)
  {
  }

  RegistryKey(const std::string& keyName)
  {
    if (RegOpenKeyExA(RootKey, keyName.c_str(), 0, KEY_QUERY_VALUE, static_cast<PHKEY>(Key)) != ERROR_SUCCESS)
    {
      ThrowLastWindowsError();
    }
  }
  
  ~RegistryKey()
  {
  }

  const std::string StringValue(const std::string& subkeyName, const std::string& valueName = std::string())
  {
    DWORD type = 0;
    DWORD dataSize = 0;
    if (RegGetValueA(static_cast<HKEY>(Key), subkeyName.c_str(), (!valueName.empty()) ? (valueName.c_str()) : (NULL), RRF_RT_REG_SZ, &type, NULL, &dataSize) != ERROR_SUCCESS || type != REG_SZ) 
    {
      ThrowLastWindowsError();
    }

    const boost::shared_array<char> data(new char[dataSize]);
    if (RegGetValueA(static_cast<HKEY>(Key), subkeyName.c_str(), (!valueName.empty()) ? (valueName.c_str()) : (NULL), RRF_RT_REG_SZ, &type, data.get(), &dataSize) != ERROR_SUCCESS || dataSize == 0 || type != REG_SZ)
    {
      ThrowLastWindowsError();
    }
    return std::string(data.get(), dataSize - 1);
  }

  static bool Exists(const std::string& keyName)
  {
    HKeyHolder key;
    return RegOpenKeyExA(RootKey, keyName.c_str(), 0, KEY_QUERY_VALUE, static_cast<PHKEY>(key)) == ERROR_SUCCESS; 
  }
  
  inline static bool NotExists(const std::string& keyName)
  {
    return !Exists(keyName);
  }

private:
  class HKeyHolder
  {
  public:
    HKeyHolder(HKEY key = NULL)
      : Key(key)
    {
    }

    ~HKeyHolder()
    {
      RegCloseKey(Key);
    }

    operator HKEY()
    {
      return Key;
    }

    operator PHKEY()
    {
      return &Key;
    }

  private:
    HKEY Key;
  };

private:
  HKeyHolder Key;
};


inline std::string ServerDidntCreateKey(const std::string& key)
{
  return "COM server didn't create registry key 'HKEY_CLASSES_ROOT\\" + key + "' during registration";
}

inline std::string ServerDidntSetDefaultValue(const std::string& value, const std::string& key)
{
  return std::string("COM server didn't set default value '") + value + "' for registry key 'HKEY_CLASSES_ROOT\\CLSID\\" + key + "' during registration";
}

// Regestering COM server in registry
void InfrastructureTest::comRegisterServer()
{
  if (OPCTest::GetClassData()->IsWindowsTestsDisabled())
  {
    return;
  }

  const OPCTest::CoClassData* cls = OPCTest::GetClassData();
  typedef void (*DllRegisterServerProc)();
  DllRegisterServerProc DllRegisterServerFunc = lib->findFunction<DllRegisterServerProc>("DllRegisterServer");
  CPPUNIT_ASSERT_MESSAGE("COM Server doesn't export function DllRegisterServer", DllRegisterServerFunc != NULL);

  // Regestering COM server
  DllRegisterServerFunc();

  // CoClass Guid entry exists HKCR\CLSID\{guid}
  CPPUNIT_ASSERT_MESSAGE(ServerDidntCreateKey(cls->RegistryKeys.ClsIdFullPathKey), RegistryKey<HKEY_CLASSES_ROOT>::Exists(cls->RegistryKeys.ClsIdFullPathKey));
  // Class name value exists HKCR\CLSID\{guid} : className
  CPPUNIT_ASSERT_MESSAGE(ServerDidntSetDefaultValue(cls->GetName(), cls->RegistryKeys.ClsIdKeyName), RegistryKey<HKEY_CLASSES_ROOT>(cls->RegistryKeys.ClsId).StringValue(cls->RegistryKeys.ClsIdKeyName) == cls->GetName());
  // ProgId Entry Exists
  CPPUNIT_ASSERT_MESSAGE(ServerDidntCreateKey(cls->GetProgId()), RegistryKey<HKEY_CLASSES_ROOT>::Exists(cls->GetProgId()));
  // class name entry at progid key
  CPPUNIT_ASSERT_MESSAGE(ServerDidntSetDefaultValue(cls->GetName(), cls->GetProgId()), RegistryKey<HKEY_CLASSES_ROOT>().StringValue(cls->GetProgId()) == cls->GetName());
  // ClsID Key subkey of ProgId
  CPPUNIT_ASSERT_MESSAGE(ServerDidntCreateKey(cls->RegistryKeys.ProgId_ClsIdKey), RegistryKey<HKEY_CLASSES_ROOT>::Exists(cls->RegistryKeys.ProgId_ClsIdKey));
  // ClassGuid Value == GetCoClassGuid();
  CPPUNIT_ASSERT_MESSAGE(ServerDidntSetDefaultValue(cls->RegistryKeys.ClsIdKeyName, cls->RegistryKeys.ProgId_ClsIdKey), RegistryKey<HKEY_CLASSES_ROOT>(cls->GetProgId()).StringValue(cls->RegistryKeys.ClsId) == cls->RegistryKeys.ClsIdKeyName);
}

inline std::string ServerDidntDeletedRegistryKey(const std::string& keyPath)
{
  return "COM Server didn't deleted registry key and it subtree 'HKEY_CLASSES_ROOT\\" + keyPath + "' during unregistration";
}

// Unregestering COM server
void InfrastructureTest::comUnregisterServer()
{
  if (OPCTest::GetClassData()->IsWindowsTestsDisabled())
  {
    return;
  }

  const OPCTest::CoClassData* cls = OPCTest::GetClassData();
  typedef void (*DllUnregisterServerProc)();
  DllUnregisterServerProc DllUnregisterServerFunc = lib->findFunction<DllUnregisterServerProc>("DllUnregisterServer");
  CPPUNIT_ASSERT(DllUnregisterServerFunc != NULL);
  DllUnregisterServerFunc();
  CPPUNIT_ASSERT_MESSAGE(ServerDidntDeletedRegistryKey(cls->RegistryKeys.ClsIdFullPathKey), RegistryKey<HKEY_CLASSES_ROOT>::NotExists(cls->RegistryKeys.ClsIdFullPathKey));
  CPPUNIT_ASSERT_MESSAGE(ServerDidntDeletedRegistryKey(cls->GetProgId()), RegistryKey<HKEY_CLASSES_ROOT>::NotExists(cls->GetProgId()));
}

#endif // _WIN32
