/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Well known attributes identifiers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_VARIABLE_ACCESS_LEVEL_H_
#define OPC_UA_VARIABLE_ACCESS_LEVEL_H_

#include <stdint.h>

namespace OpcUa
{
  enum class VariableAccessLevel : uint8_t
  {
    CurrentRead  = 1,
    CurrentWrite = 2,
    HistoryRead  = 4,
    HistoryWrite = 8,
    SemanticChange = 16,
  };
}

#endif /* OPC_UA_VARIABLE_ACCESS_LEVEL_H_ */
