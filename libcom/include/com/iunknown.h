/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IUnknown interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __inunknown_h_39d0dd6b_7d2a_416d_a176_d875bba573e7
#define  __inunknown_h_39d0dd6b_7d2a_416d_a176_d875bba573e7

#include <com/com.h>

namespace Com
{
  
#define IID_IUnknown "00000000-0000-0000-C000-000000000046"

  class IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IUnknown);

  public:
    virtual unsigned AddRef() = 0;
    virtual unsigned Release() = 0;
    virtual HRESULT QueryInterface(const char* iid, void** result) = 0;

  protected:
    IUnknown()
    {
    }

    virtual ~IUnknown()
    {
    }

  private:
    IUnknown(IUnknown&);
    IUnknown& operator=(const IUnknown&);
  };

} // namespace Com

#endif //  __inunknown_h_39d0dd6b_7d2a_416d_a176_d875bba573e7

