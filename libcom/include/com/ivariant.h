/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IVariant interface.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef _com_h_0ef80dd5_dc9b_4c44_b92f_0dbd69e6d850
#define _com_h_0ef80dd5_dc9b_4c44_b92f_0dbd69e6d850

#include <com/iunknown.h>

namespace Com
{

  typedef enum tagVARTYPE
  {
    VT_EMPTY,
    VT_INT,
    VT_INT64,
    VT_UINT,
    VT_UINT64,
    VT_FLOAT,
    VT_DOUBLE,
    VT_STRING,
  } VARTYPE;

  class IVariant : public IUnknown
  {
  public:
    virtual VARTYPE GetType() const = 0;

    virtual int GetInt() const = 0;
    virtual void SetInt(int value) = 0;

    virtual unsigned int GetUInt() const = 0;
    virtual void SetUInt(unsigned int value) = 0;

    virtual long GetInt64() const = 0;
    virtual void SetInt(long value) = 0;

    virtual unsigned long GetUInt64() const = 0;
    virtual void SetUInt64(unsigned long value) = 0;

    virtual float GetFloat() const = 0;
    virtual void SetFloat(float value) = 0;

    virtual double GetDouble() const = 0;
    virtual void SetFloat(double value) = 0;

    virtual const char* GetString() const = 0;
    virtual void SetString(const char* value) = 0;
  };

} // namespace Com

#endif // _com_h_0ef80dd5_dc9b_4c44_b92f_0dbd69e6d850

