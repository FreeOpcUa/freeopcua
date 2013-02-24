/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief CoCreateInterface inplementation. Creates class instance with using registry.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <com/com.h>
#include <com/class_ids.h>
#include <com/impl/error.h>

#include "iclass_factory_impl.h"
#include "sqlite_registry.h"

#include <string.h>

namespace
{
  std::string GetDatabasePath()
  {
    const char* dbPath = getenv("COMREG_DATABASE_PATH");
    if (dbPath)
    {
      return dbPath;
    }
    return "/var/lib/com/registry.sqlite";
  }
}



extern "C"
Com::HRESULT Com::CoCreateInstance(const char* clsId, Com::IUnknown** result)
{
  CATCH_BLOCK_START
  CHECK_PTR_PARAM1(clsId);
  CHECK_PTR_PARAM2(result);

  std::shared_ptr<Com::ClassRegistry> registry(Com::CreateSqliteRegistry(GetDatabasePath()));
  Com::IClassFactory::UniquePtr factory = Com::UniquePtr<Com::IClassFactory>(new Com::Impl::ClassFactory(registry));
  if (!strcmp(clsId, CLSID_ClassFactory))
  {
    *result = static_cast<Com::IUnknown*>(factory.release());
    return Com::COM_SUCCESS;
  }
  return factory->CreateInstance(clsId, result);
  CATCH_BLOCK_FINISH
}

