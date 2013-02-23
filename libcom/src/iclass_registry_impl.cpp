/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IClassRegistry default implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "iclass_registry_impl.h"

#include <com/impl/error.h>
#include <com/impl/istring_impl.h>

#include <string.h>

namespace Com
{
  namespace Impl
  {

    HRESULT ClassRegistry::Register(const char* clsId, const char* libraryPath)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(clsId);
      CHECK_PTR_PARAM2(libraryPath);
      Registry->RegisterClass(clsId, libraryPath);
      return COM_SUCCESS;
      CATCH_BLOCK_FINISH
    }

    HRESULT ClassRegistry::Unregister(const char* clsId)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(clsId);
      Registry->UnregisterClass(clsId);
      return COM_SUCCESS;
      CATCH_BLOCK_FINISH
    }

    HRESULT ClassRegistry::GetLibraryPath(const char* clsId, IString** resultPath) const
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(clsId);
      CHECK_PTR_PARAM2(resultPath);
      std::string path = Registry->GetLibraryPath(clsId);
      Com::IString::UniquePtr result(new Com::Impl::String());
      Com::HRESULT res = result->Set(path.c_str());
      if (res == COM_SUCCESS)
      {
        *resultPath = result.release();
      }
      return res;
      CATCH_BLOCK_FINISH
    }
   
    HRESULT ClassRegistry::QueryInterface(const char* iid, void** result)
    {
      HRESULT res = Unknown::QueryInterface(iid, result);
      if (res == COM_SUCCESS || res != COM_NOINTERFACE)
      {
        return res;
      }
      if (strcmp(iid, IID_IClassRegistry))
      {
        return COM_NOINTERFACE;
      }
      *result = static_cast<IClassRegistry*>(this);
      Unknown::AddRef();
      return COM_SUCCESS;
    }
  } // namespace Impl
} // namespace Com

