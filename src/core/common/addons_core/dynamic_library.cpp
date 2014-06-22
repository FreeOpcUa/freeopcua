/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic library class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "dynamic_library.h"

#include <opc/common/addons_core/errors.h>

#include <dlfcn.h>

namespace
{

  void* LoadLibrary(const char* path)
  {
    void* library = dlopen(path, RTLD_LAZY);
    if (!library)
    {
      std::string msg;
      if (const char* err = dlerror())
      {
        msg = err;
      }
      THROW_ERROR2(UnableToLoadDynamicLibrary, path, msg);
    }
    return library;
  }

}

namespace Common
{

  DynamicLibrary::DynamicLibrary(const std::string& libraryPath)
    : Path(libraryPath)
    , Library(0)
  {
  }

  DynamicLibrary::~DynamicLibrary()
  {
    if (Library)
    {
      //dlclose(Library);
    }
  }

  void* DynamicLibrary::FindSymbol(const std::string& funcName)
  {
    if (!Library)
    {
      Library = LoadLibrary(Path.c_str());
    }

    void* func = dlsym(Library, funcName.c_str());
    if (!func)
    {
      std::string msg;
      if (const char* err =dlerror())
      {
        msg = err;
      }
      THROW_ERROR3(UnableToFundSymbolInTheLibrary, funcName, Path, msg);
    }
    return func;
  }

}
