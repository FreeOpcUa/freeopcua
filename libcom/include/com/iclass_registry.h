/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IClassRegistry interface. 
/// Interface through which classes can be registered and class creation will be performed via one point.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __IClassRegistry_980dbac5_197b_4e81_9d27_ce0a562fb3c7
#define __IClassRegistry_980dbac5_197b_4e81_9d27_ce0a562fb3c7

#include <com/istring.h>

namespace Com
{
#define IID_IClassRegistry "980dbac5-197b-4e81-9d27-ce0a562fb3c7"

  class IClassRegistry : public virtual IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IClassRegistry);

  public:
    virtual HRESULT Register(const char* clsId, const char* libraryPath) = 0;
    virtual HRESULT Unregister(const char* clsId) = 0;
    virtual HRESULT GetLibraryPath(const char* clsId, IString** resultPath) const = 0;
  };

}

#endif // __IClassRegistry_980dbac5_197b_4e81_9d27_ce0a562fb3c7



