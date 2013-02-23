/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IClassFactory default implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "iclass_factory_impl.h"
#include "iclass_registry_impl.h"
#include "dll.h"

#include <com/class_ids.h>
#include <com/impl/error.h>

#include <functional>
#include <map>
#include <string.h>


namespace
{
  std::unique_ptr<Com::Impl::DynamicLibraryManager> LibraryManager;
  Com::Impl::DynamicLibraryManager& GetLibraryManager()
  {
    if (!LibraryManager.get())
    {
      LibraryManager = Com::Impl::CreateDynamicLibraryManager();
    }
    return *LibraryManager;
  }
  

  void CreateInstance(Com::CoCreateInstanceFunc createInstanceImpl, const char* clsId, Com::IUnknown** unk)
  {
    Com::HRESULT res = createInstanceImpl(clsId, unk);
    if (res != Com::COM_SUCCESS)
    {
      throw Com::Impl::Error(res, std::string("Unable to create class '") + std::string(clsId) + std::string("'."));
    }
  }

  typedef std::map<std::string, Com::CoCreateInstanceFunc> ClassDataMap;
  ClassDataMap ClassData;

  void CreateInstance(std::string libraryPath, const char* clsId, Com::IUnknown**result)
  {
    const ClassDataMap::const_iterator dataIt = ClassData.find(libraryPath);
    if (dataIt != ClassData.end())
    {
      CreateInstance(dataIt->second, clsId, result);
      return;
    }
    
    Com::Impl::DynamicLibraryManager& libraryManager = GetLibraryManager();
    Com::CoCreateInstanceFunc createInstance = libraryManager.Find<Com::CoCreateInstanceFunc>("CoCreateInstance", libraryPath); 
    ClassData.insert(std::make_pair(libraryPath, createInstance));
    CreateInstance(createInstance, clsId, result);
  }
 
}


namespace Com
{
  namespace Impl
  {

    HRESULT ClassFactory::CreateInstance(const char* clsId, IUnknown** result)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(clsId);
      CHECK_PTR_PARAM2(result);
      *result = NULL;
      if (!strcmp(clsId, CLSID_ClassRegistry))
      {
         *result = static_cast<Com::IUnknown*>(new Com::Impl::ClassRegistry(Registry));
         return Com::COM_SUCCESS;
      }

      ::CreateInstance(Registry->GetLibraryPath(clsId), clsId, result);
      return *result ? Com::COM_SUCCESS : Com::COM_UNKNOWN_CLASS;
      CATCH_BLOCK_FINISH
    }

    HRESULT ClassFactory::QueryInterface(const char* iid, void** result)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(iid);
      CHECK_PTR_PARAM2(result);

      HRESULT res = Unknown::QueryInterface(iid, result);
      if (res == COM_SUCCESS || res != COM_NOINTERFACE)
      {
        return res;
      }
      if (strcmp(iid, IID_IClassFactory))
      {
        return COM_NOINTERFACE;
      }
      *result = static_cast<void*>(this);
      Unknown::AddRef();
      return COM_SUCCESS;

      CATCH_BLOCK_FINISH
    }

  }
}

