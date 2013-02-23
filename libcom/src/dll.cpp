/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Dynamic library wrapper.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "dll.h"

#include <com/impl/error.h>
#include <dlfcn.h>
#include <map>


namespace
{
  class LinuxLibraryManager : public Com::Impl::DynamicLibraryManager
  {
  public:
    void* FindSymbol(const std::string& funcName, const std::string& library);

  private:
    void* Load(const std::string& libraryName);

  private:
    std::map<std::string, void*> Libraries;
  };


  void* LinuxLibraryManager::Load(const std::string& libraryFilename)
  {
    std::map<std::string, void*>::const_iterator libraryIt = Libraries.find(libraryFilename);
    if (libraryIt != Libraries.end())
    {
      return libraryIt->second;
    }

    void* library = dlopen(libraryFilename.c_str(), RTLD_LAZY);
    if (!library)
    {
      std::string msg = "Failed to load library '" + libraryFilename + "':";
      if (const char* err =dlerror())
      {
        msg += err;
      }
      throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, msg);
    }
    Libraries.insert(std::make_pair(libraryFilename, library));
    return library;
  }

  void* LinuxLibraryManager::FindSymbol(const std::string& funcName, const std::string& libraryFilename)
  {
    void* library = Load(libraryFilename);
    void* func = dlsym(library, funcName.c_str());
    if (!func)
    {
      std::string msg = "Failed to symbol '" + funcName + "' in the library '" + libraryFilename+ "':\n";
      if (const char* err =dlerror())
      {
        msg += err;
      }
      throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, msg);
    }
    return func;
  }
} //namespace

std::unique_ptr<Com::Impl::DynamicLibraryManager> Com::Impl::CreateDynamicLibraryManager()
{
  return std::unique_ptr<Com::Impl::DynamicLibraryManager>(new LinuxLibraryManager);
}

