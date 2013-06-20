/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Well known attributes identifiers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_ATTRIBUTE_IDENTIFIERS
#define __OPC_UA_BINARY_ATTRIBUTE_IDENTIFIERS

#include <stdint.h>

namespace OpcUa
{
  enum class AttributeID : uint32_t
  {
    NODE_ID    = 1,
    NODE_CLASS = 2,
    BROWSE_NAME = 3,
    DISPLAY_NAME = 4,
    DESCRIPTION = 5,
    WRITE_MASK = 6,
    USER_WRITE_MASK = 7,
    IS_ABSTRACT = 8,
    SYMMETRIC = 9,
    INVERSE_NAME = 10,
    CONTAINS_NO_LOOPS = 11,
    EVENT_NOTIFIER = 12,
    VALUE = 13,
    DATA_TYPE = 14,
    VALUE_RANK = 15,
    ARRAY_DIMENSIONS = 16,
    ACCESS_LEVEL = 17,
    USER_ACCESS_LEVEL = 18,
    MINIMUM_SAMPLING_INTERVAL = 19,
    HISTORIZING = 20,
    EXECUTABLE = 21,
    USER_EXECUTABLE = 22,

    UNKNOWN = ~uint32_t(),
  };
}

#endif // __OPC_UA_BINARY_ATTRIBUTE_IDENTIFIERS
