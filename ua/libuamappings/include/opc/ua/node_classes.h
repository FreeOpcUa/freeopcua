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

namespace OpcUa
{

  enum class NodeClass : uint32_t
  {
    OBJECT = 1,
    VARIABLE = 2,
    METHOD = 4,
    OBJECT_TYPE = 8,
    VARIABLE_TYPE = 16,
    REFERENCE_TYPE = 32,
    DATA_TYPE = 64,
    VIEW = 128
  };

}

#endif //  OPC_UA_NODE_CLASSES_H

