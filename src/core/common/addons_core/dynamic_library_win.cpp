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

#include <windows.h>

namespace
{

void * LoadDll(const char * path)
{
  // TODO Use SetDllDirctory to search inside installation path.
  void * library = LoadLibrary(path);

  if (!library)
    {
      std::string msg;
      // TODO Get windows error string.
      THROW_ERROR2(UnableToLoadDynamicLibrary, path, msg);
    }

  return library;
}

}

namespace Common
{

DynamicLibrary::DynamicLibrary(const std::string & libraryPath)
  : Path(libraryPath)
  , Library(0)
{
}

DynamicLibrary::~DynamicLibrary()
{
  if (Library)
    {
      FreeLibrary((HMODULE)Library);
    }
}

void * DynamicLibrary::FindSymbol(const std::string & funcName)
{
  if (!Library)
    {
      Library = LoadDll(Path.c_str());
    }

  void * func = (void *)GetProcAddress((HMODULE)Library, funcName.c_str());

  if (!func)
    {
      std::string msg;
      // TODO GetWindows error string.
      THROW_ERROR3(UnableToFundSymbolInTheLibrary, funcName, Path, msg);
    }

  return func;
}

}
