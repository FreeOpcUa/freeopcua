/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IUnknown default implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <com/impl/iunknown_impl.h>

#include <com/impl/error.h>

#include <cstddef>
#include <string.h>

namespace Com
{
  namespace Impl
  {
    unsigned Unknown::AddRef()
    {
      return ++RefCount;
    }

    unsigned Unknown::Release()
    {
      int count = --RefCount;
      if (count == 0)
      {
        delete this;
      }
      return count;
    }

    HRESULT Unknown::QueryInterface(const char* iid, void** result)
    {
      CATCH_BLOCK_START
      CHECK_PTR_PARAM1(iid);
      CHECK_PTR_PARAM2(result);
      void* tmp = NULL;
      if (!strcmp(iid, IID_IUnknown))
      {
        tmp = this;
      }
      else
      {
        return COM_NOINTERFACE; 
      }

      if (tmp != NULL)
      {
        AddRef();
        *result = tmp;
      }
      return COM_SUCCESS;
      CATCH_BLOCK_FINISH
    }

  } // namespace Impl
} // namespace Com

