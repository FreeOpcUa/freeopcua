/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IClassFactory interface. 
/// Interface through which classes can be created.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __IClassFactory_a0693351_4ddf_4ab5_8ce1_6bf434cc8dd7
#define __IClassFactory_a0693351_4ddf_4ab5_8ce1_6bf434cc8dd7

#include <com/iunknown.h>

namespace Com
{
#define IID_IClassFactory "aaf6f66b-e6ba-4657-8b14-f02cf78b4676"

  class IClassFactory : public virtual IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IClassFactory);

  public:
    virtual HRESULT CreateInstance(const char* clsId, IUnknown** result) = 0;
  };

}

#endif // __IClassFactory_a0693351_4ddf_4ab5_8ce1_6bf434cc8dd7

