/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IUnknown default implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <com/impl/istring_impl.h>

#include <com/impl/error.h>

#include <string.h>

namespace Com
{
  namespace Impl
  {

    HRESULT String::Get(const char** result) const
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(result);
      *result =  Value.c_str();
      return COM_SUCCESS;
      CATCH_BLOCK_FINISH
    }
    
    HRESULT String::Set(const char* value)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(value);
      Value = value;
      return COM_SUCCESS;
      CATCH_BLOCK_FINISH
    }
 
    HRESULT String::QueryInterface(const char* iid, void** result)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(iid);
      CHECK_PTR_PARAM2(result);
      HRESULT res = Unknown::QueryInterface(iid, result);
      if (res == COM_SUCCESS || res != COM_NOINTERFACE)
      {
        return res;
      }
      if (strcmp(iid, IID_IString))
      {
        return COM_NOINTERFACE;
      }
      *result = static_cast<IString*>(this);
      Unknown::AddRef();
      return COM_SUCCESS;
      CATCH_BLOCK_FINISH
    }

  } // namespace Impl
} //namespace Com

