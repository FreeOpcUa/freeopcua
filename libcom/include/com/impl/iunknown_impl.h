/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IUnknown default implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __iunknown__impl_h_ea680806_a894_4f12_aad0_2e06d2f3db4a
#define __iunknown__impl_h_ea680806_a894_4f12_aad0_2e06d2f3db4a

#include <com/iunknown.h>

namespace Com
{
  namespace Impl
  {

    class Unknown : public virtual IUnknown
    {
    public:
      Unknown()
        : RefCount(1)
      {
      }

      virtual HRESULT QueryInterface(const char* iid, void** result);
      virtual unsigned AddRef();
      virtual unsigned Release();

    private:
      volatile int RefCount;
    };

  } // namespace Impl
} //namespace Com

#endif // __iunknown__impl_h_ea680806_a894_4f12_aad0_2e06d2f3db4a

