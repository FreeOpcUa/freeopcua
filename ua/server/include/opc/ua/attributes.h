/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_ATTRIBUTES_H
#define OPC_UA_CLIENT_ATTRIBUTES_H

#include <opc/common/interface.h>
#include <opc/ua/attribute_ids.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/types.h>

#include <vector>

namespace OpcUa
{
  namespace Remote
  {

    struct ReadParameters
    {
      NodeID Node;
      AttributeID Attribute;
    };

    struct WriteParameters
    {
      NodeID Node;
      AttributeID Attribute;
      DataValue Value;
    };

    class AttributeServices : private Common::Interface
    {
    public:
      virtual DataValue Read(const ReadParameters& filter) const = 0;
      virtual StatusCode Write(const WriteParameters& filter) = 0;
    };

  } // namespace Remote
} // namespace OpcUa

#endif // OPC_UA_CLIENT_ATTRIBUTES_H

