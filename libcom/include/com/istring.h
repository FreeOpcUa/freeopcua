/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IString interface.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef _istring_h_334a8f88_a5be_4b36_941e_1b8868af5ea3
#define _istring_h_334a8f88_a5be_4b36_941e_1b8868af5ea3

#include <com/iunknown.h>

namespace Com
{
#define IID_IString "334a8f88-a5be-4b36-941e-1b8868af5ea3"

  class IString : public virtual IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IString);

  public:
    virtual HRESULT Get(const char** result) const = 0;
    virtual HRESULT Set(const char* value) = 0;
  };

}

#endif // _istring_h_334a8f88_a5be_4b36_941e_1b8868af5ea3 

