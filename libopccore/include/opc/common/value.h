/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common value. Variant.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __MOZOPC_VALUE_H__
#define __MOZOPC_VALUE_H__

//#include <opc/common/addons_core/addon_ids.h>
#include <opc/common/exception.h>

#include <boost/variant.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <string>


namespace Common
{
  /// @brief Type of value holded in Value.
  enum ValueType
  {
    CHAR_TYPE,
    INT_TYPE,
    UNSIGNED_INT_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE,
    WSTRING_TYPE,
    // service types
    UNKNOWN_TYPE = ~3, // used when user don't know wich type does he need and he will ude default
    INVALID_TYPE = ~0, // user sended invalid type
  };

  /// @brief Common type that can hold values of different types.
  typedef boost::variant<char, int, unsigned int, float, double, std::string, std::wstring> VariantType;

  /// @brief Data with time.
  struct Value
  {
    boost::posix_time::ptime Time;   // time when data was updated
    VariantType Data;

  public:
    Value()
      : Data(std::string())
    {
    }

    Value(const Value& value)
    {
      *this = value;
    }
    
    template<typename T>
    explicit Value(const T& value)
      : Data(value)
    {
    }
    
    template <typename T>
    Value(const T& value, const boost::posix_time::ptime& time)
      : Time(time)
      , Data(value)
    {
    }
  };

  ValueType GetValueType(const VariantType& value);
}

#endif // __MOZOPC_VALUE_H__
