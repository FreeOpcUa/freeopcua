/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test COM server.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "test_com.h"

#include <com/impl/error.h>
#include <com/impl/iunknown_impl.h>
#include <string.h>

namespace
{
}

extern "C"
Com::HRESULT Com::CoCreateInstance(const char* clsId, Com::IUnknown** result)
{
  CATCH_BLOCK_START
  CHECK_PTR_PARAM1(clsId);
  CHECK_PTR_PARAM2(result);
  
  if (!strcmp(clsId, CLSID_Test))
  {
    *result = static_cast<Com::IUnknown*>(new Com::Impl::Unknown());
    return Com::COM_SUCCESS;
  }
  return Com::COM_UNKNOWN_CLASS;
  CATCH_BLOCK_FINISH
}

