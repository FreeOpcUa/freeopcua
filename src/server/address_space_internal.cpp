/// @author Alexander Rykovanov 2013-2014
/// @author Olivier Roulet-dubonnet 2014
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "address_space_internal.h"


namespace OpcUa
{
  namespace Internal
  {
    typedef std::map <IntegerID, std::shared_ptr<InternalSubscription>> SubscriptionsIDMap; // Map SubscptioinID, SubscriptionData
    
    //store subscription for one attribute
    struct AttSubscription
    {
      IntegerID SubscriptionId;
      IntegerID MonitoredItemId;
      MonitoringParameters Parameters;
    };

    AddressSpaceInMemory::AddressSpaceInMemory(bool debug)
        : Debug(debug)
        , DataChangeCallbackHandle(0)
    {
      ObjectAttributes attrs;
      attrs.Description = LocalizedText(OpcUa::Names::Root);
      attrs.DisplayName = LocalizedText(OpcUa::Names::Root);

      AddNodesItem rootNode;
      rootNode.BrowseName = QualifiedName(0, OpcUa::Names::Root);
      rootNode.Class = NodeClass::Object;
      rootNode.RequestedNewNodeID = ObjectID::RootFolder;
      rootNode.TypeDefinition = ObjectID::FolderType;
      rootNode.Attributes = attrs;
      AddNode(rootNode);
    }

    AddressSpaceInMemory::~AddressSpaceInMemory()
    {
    }

    std::vector<AddNodesResult> AddressSpaceInMemory::AddNodes(const std::vector<AddNodesItem>& items)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<AddNodesResult> results;
      for (const AddNodesItem& item: items)
      {
        results.push_back(AddNode(item));
      }
      return results;
    }

    std::vector<StatusCode> AddressSpaceInMemory::AddReferences(const std::vector<AddReferencesItem>& items)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<StatusCode> results;
      for (const auto& item : items)
      {
        results.push_back(AddReference(item));
      }
      return results;
    }

    std::vector<BrowsePathResult> AddressSpaceInMemory::TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<BrowsePathResult> results;
      for (BrowsePath browsepath : params.BrowsePaths )
      {
        BrowsePathResult result = TranslateBrowsePath(browsepath);
        results.push_back(result);
      }
      return results;
    }

    std::vector<BrowseResult> AddressSpaceInMemory::Browse(const OpcUa::NodesQuery& query) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      if (Debug) std::cout << "AddressSpaceInternal | Browsing." << std::endl;
      std::vector<BrowseResult> results;
      for ( BrowseDescription browseDescription: query.NodesToBrowse)
      {
        BrowseResult result;
        if(Debug)
        {
          std::cout << "AddressSpaceInternal | Browsing ";
          std::cout << " NodeID: '" << browseDescription.NodeToBrowse << "'";
          std::cout << ", ReferenceID: '" << browseDescription.ReferenceTypeID << "'";
          std::cout << ", Direction: " << browseDescription.Direction;
          std::cout << ", NodeClasses: 0x" << std::hex << (unsigned)browseDescription.NodeClasses;
          std::cout << ", ResultMask: '0x" << std::hex << (unsigned)browseDescription.ResultMask << std::endl;
        }

        NodesMap::const_iterator node_it = Nodes.find(browseDescription.NodeToBrowse);
        if ( node_it == Nodes.end() )
        {
          if (Debug) std::cout << "AddressSpaceInternal | Node '" << OpcUa::ToString(browseDescription.NodeToBrowse) << "' not found in the address space." << std::endl;
          continue;
        }

        std::copy_if(node_it->second.References.begin(), node_it->second.References.end(), std::back_inserter(result.Referencies),
            std::bind(&AddressSpaceInMemory::IsSuitableReference, this, std::cref(browseDescription), std::placeholders::_1)
        );
        results.push_back(result);
      }
      return results;
    }

    std::vector<BrowseResult> AddressSpaceInMemory::BrowseNext() const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      return std::vector<BrowseResult>();
    }

    std::vector<DataValue> AddressSpaceInMemory::Read(const ReadParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<DataValue> values;
      for (const AttributeValueID& attribute : params.AttributesToRead)
      {
        values.push_back(GetValue(attribute.Node, attribute.Attribute));
      }
      return values;
    }

    std::vector<StatusCode> AddressSpaceInMemory::Write(const std::vector<OpcUa::WriteValue>& values)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<StatusCode> statuses;
      for (WriteValue value : values)
      {
        if (value.Data.Encoding & DATA_VALUE)
        {
          statuses.push_back(SetValue(value.Node, value.Attribute, value.Data));
          continue;
        }
        statuses.push_back(StatusCode::BadNotWritable);
      }
      return statuses;
    }

    std::tuple<bool, NodeID> AddressSpaceInMemory::FindElementInNode(const NodeID& nodeid, const RelativePathElement& element) const
    {
      NodesMap::const_iterator nodeit = Nodes.find(nodeid);
      if ( nodeit != Nodes.end() )
      {
        for (auto reference : nodeit->second.References)
        {
          //if (reference.first == current) { std::cout <<   reference.second.BrowseName.NamespaceIndex << reference.second.BrowseName.Name << " to " << element.TargetName.NamespaceIndex << element.TargetName.Name <<std::endl; }
          if (reference.BrowseName == element.TargetName)
          {
            return std::make_tuple(true, reference.TargetNodeID);
          }
        }
      }
      return std::make_tuple(false, NodeID());
    }

    BrowsePathResult AddressSpaceInMemory::TranslateBrowsePath(const BrowsePath& browsepath) const
    {
      NodeID current = browsepath.StartingNode;
      BrowsePathResult result;

      for (RelativePathElement element : browsepath.Path.Elements)
      {
        auto res = FindElementInNode(current, element);
        if ( std::get<0>(res) == false )
        {
          result.Status = OpcUa::StatusCode::BadNoMatch;
          return result;
        }
        current = std::get<1>(res);
      }

      result.Status = OpcUa::StatusCode::Good;
      std::vector<BrowsePathTarget> targets;
      BrowsePathTarget target;
      target.Node = current;
      target.RemainingPathIndex = std::numeric_limits<uint32_t>::max();
      targets.push_back(target);
      result.Targets = targets;
      return result;
    }

    DataValue AddressSpaceInMemory::GetValue(const NodeID& node, AttributeID attribute) const
    {
      NodesMap::const_iterator nodeit = Nodes.find(node);
      if ( nodeit != Nodes.end() )
      {
        AttributesMap::const_iterator attrit = nodeit->second.Attributes.find(attribute);
        if ( attrit != nodeit->second.Attributes.end() )
        {
          if ( attrit->second.GetValueCallback )
          {
            if (Debug) std::cout << "AddressSpaceInternal | A callback is set for this value, calling callback" << std::endl;
            return attrit->second.GetValueCallback();
          }
          if (Debug) std::cout << "AddressSpaceInternal | No callback is set for this value returning stored value" << std::endl;
          return attrit->second.Value;
        }
      }
      DataValue value;
      value.Encoding = DATA_VALUE_STATUS_CODE;
      value.Status = StatusCode::BadNotReadable;
      return value;
    }

    uint32_t AddressSpaceInMemory::AddDataChangeCallback(const NodeID& node, AttributeID attribute, std::function<Server::DataChangeCallback> callback)
    {
      if (Debug) std::cout << "AddressSpaceInternal| Set data changes callback for node " << node
         << " and attribute " << (unsigned)attribute <<  std::endl;
      NodesMap::iterator it = Nodes.find(node);
      if ( it == Nodes.end() )
      {
        if (Debug) std::cout << "AddressSpaceInternal| Node '" << node << "' not found." << std::endl;
        throw std::runtime_error("AddressSpaceInternal | NodeID not found");
      }
      AttributesMap::iterator ait = it->second.Attributes.find(attribute);
      if ( ait == it->second.Attributes.end() )
      {
        if (Debug) std::cout << "address_space| Attribute " << (unsigned)attribute << " of node '" << node << "' not found." << std::endl;
        throw std::runtime_error("Attribute not found");
      }

      uint32_t handle = ++DataChangeCallbackHandle;
      DataChangeCallbackData data;
      data.Callback = callback;
      ait->second.DataChangeCallbacks[handle] = data;
      ClientIDToAttributeMap[handle] = NodeAttribute(node, attribute);
      return handle;
    }

    void AddressSpaceInMemory::DeleteDataChangeCallback(uint32_t serverhandle )
    {
      if (Debug) std::cout << "AddressSpaceInternal | Deleting callback with client id. " << serverhandle << std::endl;

      ClientIDToAttributeMapType::iterator it = ClientIDToAttributeMap.find(serverhandle);
      if ( it == ClientIDToAttributeMap.end() )
      {
        std::cout << "AddressSpaceInternal | Error, request to delete a callback using unknown handle: " << serverhandle << std::endl;
        return;
      }

      NodesMap::iterator nodeit = Nodes.find(it->second.Node);
      if ( nodeit != Nodes.end() )
      {
        AttributesMap::iterator ait = nodeit->second.Attributes.find(it->second.Attribute);
        if ( ait != nodeit->second.Attributes.end() )
        {
          size_t nb = ait->second.DataChangeCallbacks.erase(serverhandle);
          if (Debug) std::cout << "AddressSpaceInternal | deleted " << nb << " callbacks" << std::endl;
          ClientIDToAttributeMap.erase(serverhandle);
          return;
        }
      }
      throw std::runtime_error("AddressSpaceInternal | NodeID or attribute nor found");
    }

    StatusCode AddressSpaceInMemory::SetValueCallback(const NodeID& node, AttributeID attribute, std::function<DataValue(void)> callback)
    {
      NodesMap::iterator it = Nodes.find(node);
      if ( it != Nodes.end() )
      {
        AttributesMap::iterator ait = it->second.Attributes.find(attribute);
        if ( ait != it->second.Attributes.end() )
        {
          ait->second.GetValueCallback = callback;
          return StatusCode::Good;
        }
      }
      return StatusCode::BadAttributeIdInvalid;
    }

    StatusCode AddressSpaceInMemory::SetValue(const NodeID& node, AttributeID attribute, const DataValue& data)
    {
      NodesMap::iterator it = Nodes.find(node);
      if ( it != Nodes.end() )
      {
        AttributesMap::iterator ait = it->second.Attributes.find(attribute);
        if ( ait != it->second.Attributes.end() )
        {
          DataValue value(data);
          value.SetServerTimestamp(CurrentDateTime());
          ait->second.Value = value;
          //call registered callback
          for (auto pair : ait->second.DataChangeCallbacks)
          {
            pair.second.Callback(it->first, ait->first, ait->second.Value);
          }
          return StatusCode::Good;
        }
      }
      return StatusCode::BadAttributeIdInvalid;
    }

    bool AddressSpaceInMemory::IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const
    {
      if (Debug) std::cout << "AddressSpaceInternal | Checking reference '" << reference.ReferenceTypeID << "' to the node '" << reference.TargetNodeID << "' (" << reference.BrowseName << "_." << std::endl;

      if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
      {
        if (Debug) std::cout << "AddressSpaceInternal | Reference in different direction." << std::endl;
        return false;
      }
      if (desc.ReferenceTypeID != ObjectID::Null && !IsSuitableReferenceType(reference, desc.ReferenceTypeID, desc.IncludeSubtypes))
      {
        if (Debug) std::cout << "AddressSpaceInternal | Reference has wrong type." << std::endl;
        return false;
      }
      if (desc.NodeClasses && (desc.NodeClasses & static_cast<int32_t>(reference.TargetNodeClass)) == 0)
      {
        if (Debug) std::cout << "AddressSpaceInternal | Reference has wrong class." << std::endl;
        return false;
      }
      if (Debug) std::cout << "AddressSpaceInternal | Reference suitable." << std::endl;
      return true;
    }

    bool AddressSpaceInMemory::IsSuitableReferenceType(const ReferenceDescription& reference, const NodeID& typeID, bool includeSubtypes) const
    {
      if (!includeSubtypes)
      {
        return reference.ReferenceTypeID == typeID;
      }
      const std::vector<NodeID> suitableTypes = SelectNodesHierarchy(std::vector<NodeID>(1, typeID));
      const auto resultIt = std::find(suitableTypes.begin(), suitableTypes.end(), reference.ReferenceTypeID);\
      return resultIt != suitableTypes.end();
    }

    std::vector<NodeID> AddressSpaceInMemory::SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const
    {
      std::vector<NodeID> subNodes;
      for ( NodeID nodeid: sourceNodes )
      {
          NodesMap::const_iterator node_it = Nodes.find(nodeid);
          if ( node_it != Nodes.end() )
          {
            for (auto& ref:  node_it->second.References )
            {
              subNodes.push_back(ref.TargetNodeID);
          }
        }
      }
      if (subNodes.empty())
      {
        return sourceNodes;
      }

      const std::vector<NodeID> allChilds = SelectNodesHierarchy(subNodes);
      sourceNodes.insert(sourceNodes.end(), allChilds.begin(), allChilds.end());
      return sourceNodes;
    }

    AddNodesResult AddressSpaceInMemory::AddNode( const AddNodesItem& item )
    {
      AddNodesResult result;
      if (Debug) std::cout << "AddressSpaceInternal | address_space| Adding new node id='" << item.RequestedNewNodeID << "' name=" << item.BrowseName.Name << std::endl;

      if (!Nodes.empty() && item.RequestedNewNodeID != ObjectID::Null && Nodes.find(item.RequestedNewNodeID) != Nodes.end())
      {
        std::cerr << "AddressSpaceInternal | Error: NodeID '"<< item.RequestedNewNodeID << "' allready exist: " << std::endl;
        result.Status = StatusCode::BadNodeIdExists;
        return result;
      }

      NodesMap::iterator parent_node_it = Nodes.end();
      if (item.ParentNodeId != NodeID())
      {
        parent_node_it = Nodes.find(item.ParentNodeId);
        if ( parent_node_it == Nodes.end() )
        {
          std::cout << "AddressSpaceInternal | Error: Parent node '"<< item.ParentNodeId << "'does not exist" << std::endl;
          result.Status = StatusCode::BadParentNodeIdInvalid;
          return result;
        }
      }

      const NodeID resultID = GetNewNodeID(item.RequestedNewNodeID);
      NodeStruct nodestruct;
      //Add Common attributes
      nodestruct.Attributes[AttributeID::NodeId].Value = resultID;
      nodestruct.Attributes[AttributeID::BrowseName].Value = item.BrowseName;
      nodestruct.Attributes[AttributeID::NodeClass].Value = static_cast<int32_t>(item.Class);

      // Add requested attributes
      for (const auto& attr: item.Attributes.Attributes)
      {
        AttributeValue attval;
        attval.Value = attr.second;

        nodestruct.Attributes.insert(std::make_pair(attr.first, attval));
      }

      Nodes.insert(std::make_pair(resultID, nodestruct));

      if (parent_node_it != Nodes.end())
      {
        // Link to parent
        ReferenceDescription desc;
        desc.ReferenceTypeID = item.ReferenceTypeId;
        desc.TargetNodeID = resultID;
        desc.TargetNodeClass = item.Class;
        desc.BrowseName = item.BrowseName;
        desc.DisplayName = LocalizedText(item.BrowseName.Name);
        desc.TargetNodeTypeDefinition = item.TypeDefinition;
        desc.IsForward = true; // should this be in constructor?

        parent_node_it->second.References.push_back(desc);
      }

      if (item.TypeDefinition != ObjectID::Null)
      {
        // Link to parent
        AddReferencesItem typeRef;
        typeRef.SourceNodeID = resultID;
        typeRef.IsForward = true;
        typeRef.ReferenceTypeId = ObjectID::HasTypeDefinition;
        typeRef.TargetNodeID = item.TypeDefinition;
        typeRef.TargetNodeClass = NodeClass::DataType;
        AddReference(typeRef);
      }

      result.Status = StatusCode::Good;
      result.AddedNodeID = resultID;
      if (Debug) std::cout << "AddressSpaceInternal | node added." << std::endl;
      return result;
    }

    StatusCode AddressSpaceInMemory::AddReference(const AddReferencesItem& item)
    {
      NodesMap::iterator node_it = Nodes.find(item.SourceNodeID);
      if ( node_it == Nodes.end() )
      {
        return StatusCode::BadSourceNodeIdInvalid;
      }
      ReferenceDescription desc;
      desc.ReferenceTypeID = item.ReferenceTypeId;
      desc.IsForward = item.IsForward;
      desc.TargetNodeID = item.TargetNodeID;
      desc.TargetNodeClass = item.TargetNodeClass;
      // FIXME: these fields have to be filled from address space dynamically.
      // FIXME: note! Target node ID can be absent according standard.
      desc.BrowseName = QualifiedName(GetObjectIdName(item.TargetNodeID));
      desc.DisplayName = LocalizedText(GetObjectIdName(item.TargetNodeID));
      node_it->second.References.push_back(desc);
      return StatusCode::Good;
    }

    NodeID AddressSpaceInMemory::GetNewNodeID(const NodeID& id)
    {
      if (id == ObjectID::Null)
      {
        return OpcUa::NumericNodeID(++MaxNodeIDNum);
      }

      if (id.GetNamespaceIndex() == 0)
      {
        const uint32_t number = id.GetIntegerIdentifier();
        if (MaxNodeIDNum < number)
        {
          MaxNodeIDNum = number;
        }
      }

      return id;
    }
  }

  namespace Server
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug)
    {
      return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory(debug));
    }
  }
}
