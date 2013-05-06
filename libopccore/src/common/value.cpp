/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common::Value realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/value.h>
#include <opc/common/errors.h>

Common::ValueType Common::GetValueType(const Common::VariantType& value)
{
  switch(value.which())
  {
    case 0: return CHAR_TYPE;
    case 1: return INT_TYPE;
    case 2: return UNSIGNED_INT_TYPE;
    case 3: return FLOAT_TYPE;
    case 4: return DOUBLE_TYPE;
    case 5: return STRING_TYPE;
    case 6: return WSTRING_TYPE;
  }
  THROW_ERROR1(InvalidValueType, value.which());
}

