/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_ADDRESSSPACE_H
#define OPC_UA_CLIENT_ADDRESSSPACE_H

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/view.h>
#include <opc/ua/protocol/node_management.h>

#include <vector>

namespace OpcUa
{
  namespace Remote
  {

    class NodeManagementServices : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(NodeManagementServices);

    public:
      virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items) = 0;
      virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items) = 0;
      //FIXME: Remove the 2 following methods, they are not as in spec
      virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value) = 0;
      virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference) = 0;
    };

  } // namespace Remote
} // namespace OpcUa

#endif // OPC_UA_CLIENT_ADDRESSSPACE_H

