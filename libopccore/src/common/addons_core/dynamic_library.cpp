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

#include <opccore/common/addons_core/errors.h>

#include <dlfcn.h>

namespace Common
{

  DynamicLibrary::DynamicLibrary(const std::string& libraryPath)
    : Path(libraryPath)
    , Library(dlopen(libraryPath.c_str(), RTLD_LAZY))
  {
    if (!Library)
    {
      std::string msg;
      if (const char* err = dlerror())
      {
        msg = err;
      }
      THROW_ERROR2(UnableToLoadDynamicLibrary, Path, msg);
    }
  }

  DynamicLibrary::~DynamicLibrary()
  {
    dlclose(Library);
  }

  void* DynamicLibrary::FindSymbol(const std::string& funcName)
  {
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
