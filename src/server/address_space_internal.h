/// @author Alexander Rykovanov 2013-2014
/// @author Olivier Roulet-dubonnet 2014
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include "address_space_addon.h"

#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/view.h>
#include <opc/ua/services/attributes.h>
#include <opc/ua/services/node_management.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <ctime>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <deque>
#include <set>
#include <thread>



namespace OpcUa
{
  namespace Internal
  {

    class InternalSubscription;

    struct NodeAttribute
    {
      NodeID Node;
      AttributeID Attribute;
      NodeAttribute(NodeID node, AttributeID attribute) : Node(node), Attribute(attribute) {}
      NodeAttribute() {} //seems compiler wants this one
    };

    typedef std::map<uint32_t, NodeAttribute> ClientIDToAttributeMapType;
    
    struct DataChangeCallbackData
    {
      std::function<void(const IntegerID&, const DataValue&)> DataChangeCallback;
      IntegerID ClientHandle;
    };

    typedef std::map<uint32_t, DataChangeCallbackData> DataChangeCallbackMap;

    //Store an attribute value together with a link to all its suscriptions
    struct AttributeValue
    {
      DataValue Value;
      DataChangeCallbackMap DataChangeCallbacks;
      std::function<DataValue(void)> GetValueCallback;
    };

    typedef std::map<AttributeID, AttributeValue> AttributesMap;
    
    //Store all data related to a Node
    struct NodeStruct
    {
      AttributesMap Attributes;
      std::vector<ReferenceDescription> References;
    };

    typedef std::map<NodeID, NodeStruct> NodesMap;

    //In memory storage of server opc-ua data model
    class AddressSpaceInMemory : public Server::AddressSpace
    {
      public:
        AddressSpaceInMemory(bool debug);

       ~AddressSpaceInMemory();

        //Services implementation
        virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items);
        virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items);
        virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const;
        virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const;
        virtual std::vector<ReferenceDescription> BrowseNext() const;
        virtual std::vector<DataValue> Read(const ReadParameters& params) const;
        virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values);

        //Server side methods
        uint32_t AddDataChangeCallback(const NodeID& node, AttributeID attribute, const IntegerID& clienthandle, std::function<void(IntegerID, DataValue)> callback);
        void DeleteDataChangeCallback(uint32_t serverhandle);
        StatusCode SetValueCallback(const NodeID& node, AttributeID attribute, std::function<DataValue(void)> callback);

      private:
        std::tuple<bool, NodeID> FindElementInNode(const NodeID& nodeid, const RelativePathElement& element) const;
        BrowsePathResult TranslateBrowsePath(const BrowsePath& browsepath) const;
        DataValue GetValue(const NodeID& node, AttributeID attribute) const;
        StatusCode SetValue(const NodeID& node, AttributeID attribute, const Variant& data); //FIXME should be removed
        StatusCode SetValue(const NodeID& node, AttributeID attribute, const DataValue& data); //FIXME: should be used instead of variant
        bool IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const;
        bool IsSuitableReferenceType(const ReferenceDescription& reference, const NodeID& typeID, bool includeSubtypes) const;
        std::vector<NodeID> SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const;
        AddNodesResult AddNode( const AddNodesItem& item );
        StatusCode AddReference(const AddReferencesItem& item);
        NodeID GetNewNodeID(const NodeID& id);

      private:
        bool Debug = false;
        mutable boost::shared_mutex DbMutex;
        NodesMap Nodes;
        ClientIDToAttributeMapType ClientIDToAttributeMap; //Use to find callback using callback subcsriptionid
        uint64_t MaxNodeIDNum = 0;
    };
  }

  namespace Server
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug);
  }
}

