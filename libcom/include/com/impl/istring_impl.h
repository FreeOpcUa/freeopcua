/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IUnknown default implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __istring__impl_h_10c6b1b8_32a4_476a_be5b_1cb751b72cb0
#define __istring__impl_h_10c6b1b8_32a4_476a_be5b_1cb751b72cb0

#include "iunknown_impl.h"

#include <com/istring.h>

#include <string>

namespace Com
{
  namespace Impl
  {

    class String
      : public Unknown
      , public IString
    {
    public:
      String()
      {
      }

      virtual HRESULT Get(const char** result) const;
      virtual HRESULT Set(const char* value);
      virtual HRESULT QueryInterface(const char* iid, void** result);

    private:
      std::string Value;
    };

  } // namespace Impl
} //namespace Com


#endif // __istring__impl_h_10c6b1b8_32a4_476a_be5b_1cb751b72cb0

