/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Node classes values.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_NODE_CLASSES_H
#define OPC_UA_NODE_CLASSES_H

#include <stdint.h>

namespace OpcUa
{

  enum class NodeClass : int32_t
  {
    All = 0, // uses in browse description to aquire all classes.
    Object = 1,
    Variable = 2,
    Method = 4,
    ObjectType = 8,
    VariableType = 16,
    ReferenceType = 32,
    DataType = 64,
    View = 128
  };

}

#endif //  OPC_UA_NODE_CLASSES_H

