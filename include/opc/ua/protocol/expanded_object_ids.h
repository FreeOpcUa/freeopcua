/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Well known attributes identifiers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_EXPANDED_OBJECTS_IDS
#define __OPC_UA_EXPANDED_OBJECTS_IDS

#include <stdint.h>

namespace OpcUa
{
  enum class ExpandedObjectID : uint32_t
  {
    Null           = 0,
    ObjectAttribute = 354,
    VariableAttribute = 357,
    MethodAttribute = 360,
    ObjectTypeAttribute = 363,
    VariableTypeAttribute = 366 ,
    ReferenceTypeAttribute = 369,
    DataTypeAttribute = 372,
    ViewAttribute = 375,
 
  };
}

#endif // __OPC_UA_BINARY_IDENTIFIERS
