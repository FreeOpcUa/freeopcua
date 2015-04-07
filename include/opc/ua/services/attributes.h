/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_Client_ATTRIBUTES_H
#define OPC_UA_Client_ATTRIBUTES_H

#include <opc/common/class_pointers.h>
#include <opc/common/interface.h>
#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/protocol.h>

#include <vector>

namespace OpcUa
{

    class AttributeServices : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(AttributeServices);

    public:
      virtual std::vector<DataValue> Read(const OpcUa::ReadParameters& filter) const = 0;
      virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter) = 0;
    };

} // namespace OpcUa

#endif // OPC_UA_Client_ATTRIBUTES_H
