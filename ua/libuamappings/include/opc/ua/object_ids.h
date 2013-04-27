/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Well known attributes identifiers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_IDENTIFIERS
#define __OPC_UA_BINARY_IDENTIFIERS

namespace OpcUa
{
  enum class ObjectID : uint32_t
  {
    RootFolder = 84,
    ObjectsFolder = 85,
    TypesFolder = 86,
    ViewsFolder = 87,
  };
}

#endif // __OPC_UA_BINARY_IDENTIFIERS
