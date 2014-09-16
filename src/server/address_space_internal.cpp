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

    using namespace OpcUa::Remote;

    AddressSpaceInMemory::AddressSpaceInMemory(bool debug)
        : Debug(debug)
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

      std::vector<ReferenceDescription> AddressSpaceInMemory::Browse(const OpcUa::NodesQuery& query) const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        if (Debug) std::cout << "Browsing." << std::endl;
        std::vector<ReferenceDescription> result;
        for ( BrowseDescription browseDescription: query.NodesToBrowse)
        {
          if(Debug)
          {
            std::cout << "Browsing ";
            std::cout << " NodeID: '" << browseDescription.NodeToBrowse << "'";
            std::cout << ", ReferenceID: '" << browseDescription.ReferenceTypeID << "'";
            std::cout << ", Direction: " << browseDescription.Direction;
            std::cout << ", NodeClasses: 0x" << std::hex << (unsigned)browseDescription.NodeClasses;
            std::cout << ", ResultMask: '0x" << std::hex << (unsigned)browseDescription.ResultMask << std::endl;
          }

          NodesMap::const_iterator node_it = Nodes.find(browseDescription.NodeToBrowse);
          if ( node_it == Nodes.end() )
          {
            if (Debug) std::cout << "Node not found in the address space." << std::endl;
            continue;
          }

          if(Debug)
          {
            std::cout << "Node found in the address space." << std::endl;
            std::cout << "Finding reference." << std::endl;
          }

          std::copy_if(node_it->second.References.begin(), node_it->second.References.end(), std::back_inserter(result),
              std::bind(&AddressSpaceInMemory::IsSuitableReference, this, std::cref(browseDescription), std::placeholders::_1)
          );
        }
        return result;
      }

      std::vector<ReferenceDescription> AddressSpaceInMemory::BrowseNext() const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        return std::vector<ReferenceDescription>();
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
            statuses.push_back(SetValue(value.Node, value.Attribute, value.Data.Value));
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
              if (Debug) std::cout << "A callback is set for this value, calling callback" << std::endl;
              return attrit->second.GetValueCallback();
            }
            if (Debug) std::cout << "No callback is set for this value returning stored value" << std::endl;
            return attrit->second.Value;
          }
        }
        DataValue value;
        value.Encoding = DATA_VALUE_STATUS_CODE;
        value.Status = StatusCode::BadNotReadable;
        return value;
      }

      uint32_t AddressSpaceInMemory::AddDataChangeCallback(const NodeID& node, AttributeID attribute, IntegerID clienthandle, std::function<void(IntegerID, DataValue)> callback )
      {
        NodesMap::iterator it = Nodes.find(node);
        if ( it != Nodes.end() )
        {
          AttributesMap::iterator ait = it->second.Attributes.find(attribute);
          if ( ait != it->second.Attributes.end() )
          {
            static uint32_t handle;
            ++handle;
            DataChangeCallbackData data;
            data.DataChangeCallback = callback;
            data.ClientHandle = clienthandle;
            ait->second.DataChangeCallbacks[handle] = data;
            return handle;
          }
        }
        //return 0; //SHould I return 0 or raise exception?
        throw std::runtime_error("NodeID or attribute not found");
      }

      void AddressSpaceInMemory::DeleteDataChangeCallback(const NodeID& node, AttributeID attribute, IntegerID handle )
      {
        NodesMap::iterator it = Nodes.find(node);
        if ( it != Nodes.end() )
        {
          AttributesMap::iterator ait = it->second.Attributes.find(attribute);
          if ( ait != it->second.Attributes.end() )
          {
            ait->second.DataChangeCallbacks.erase(handle); 
            return;
          }
        }
        throw std::runtime_error("NodeID or attribute nor found");
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

      StatusCode AddressSpaceInMemory::SetValue(const NodeID& node, AttributeID attribute, const Variant& data)
      {
        NodesMap::iterator it = Nodes.find(node);
        if ( it != Nodes.end() )
        {
          AttributesMap::iterator ait = it->second.Attributes.find(attribute);
          if ( ait != it->second.Attributes.end() )
          {
            DataValue value(data);
            value.ServerTimestamp = CurrentDateTime();
            value.SourceTimestamp = CurrentDateTime(); //FIXME: should allow client to set this one
            ait->second.Value = value;
            //call registered callback
            for (auto pair : ait->second.DataChangeCallbacks)
            {
              pair.second.DataChangeCallback(pair.second.ClientHandle, ait->second.Value);
            }
            return StatusCode::Good;
          }
        }
        return StatusCode::BadAttributeIdInvalid;
      }

      bool AddressSpaceInMemory::IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const
      {
        if (Debug)
          std::cout << "Checking reference '" << reference.ReferenceTypeID << "' to the node '" << reference.TargetNodeID << "' (" << reference.BrowseName << "_." << std::endl;

        if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
        {
          if (Debug) std::cout << "Reference in different direction." << std::endl;
          return false;
        }
        if (desc.ReferenceTypeID != ObjectID::Null && !IsSuitableReferenceType(reference, desc.ReferenceTypeID, desc.IncludeSubtypes))
        {
          if (Debug) std::cout << "Reference has wrong type." << std::endl;
          return false;
        }
        if (desc.NodeClasses && (desc.NodeClasses & static_cast<int32_t>(reference.TargetNodeClass)) == 0)
        {
          if (Debug) std::cout << "Reference has wrong class." << std::endl;
          return false;
        }
        if (Debug) std::cout << "Reference suitable." << std::endl;
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
        if (Debug) std::cout << "address_space| Adding new node id='" << item.RequestedNewNodeID << "' name=" << item.BrowseName.Name << std::endl;

        if (!Nodes.empty() && Nodes.find(item.RequestedNewNodeID) != Nodes.end())
        {
          std::cout << "Error: NodeID '"<< item.RequestedNewNodeID << "' allready exist: " << std::endl;
          result.Status = StatusCode::BadNodeIdExists;
          return result;
        }

        NodesMap::iterator parent_node_it = Nodes.end();
        if (item.ParentNodeId != NodeID())
        {
          parent_node_it = Nodes.find(item.ParentNodeId);
          if ( parent_node_it == Nodes.end() )
          {
            std::cout << "Error: Parent node '"<< item.ParentNodeId << "'does not exist" << std::endl;
            result.Status = StatusCode::BadParentNodeIdInvalid;
            return result;
          }
        }

        NodeStruct nodestruct;
        //Add Common attributes
        AttributeValue attv;
        attv.Value = item.RequestedNewNodeID;
        nodestruct.Attributes[AttributeID::NODE_ID] = attv;
        attv.Value = item.BrowseName;
        nodestruct.Attributes[AttributeID::BROWSE_NAME] = attv;
        attv.Value = static_cast<int32_t>(item.Class);
        nodestruct.Attributes[AttributeID::NODE_CLASS] = attv;

        // Add requested attributes
        for (const auto& attr: item.Attributes.Attributes)
        {
          AttributeValue attval;
          attval.Value = attr.second;

          nodestruct.Attributes.insert(std::make_pair(attr.first, attval));
        }
        Nodes.insert(std::make_pair(item.RequestedNewNodeID, nodestruct));


        if (parent_node_it != Nodes.end())
        {
          // Link to parent
          ReferenceDescription desc;
          desc.ReferenceTypeID = item.ReferenceTypeId;
          desc.TargetNodeID = item.RequestedNewNodeID;
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
          typeRef.SourceNodeID = item.RequestedNewNodeID;
          typeRef.IsForward = true;
          typeRef.ReferenceTypeId = ObjectID::HasTypeDefinition;
          typeRef.TargetNodeID = item.TypeDefinition;
          typeRef.TargetNodeClass = NodeClass::DataType;
          AddReference(typeRef);
        }

        result.Status = StatusCode::Good;
        result.AddedNodeID = item.RequestedNewNodeID;
        if (Debug) std::cout << "address_space| node added." << std::endl;
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

  }

  namespace UaServer
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug)
    {
      return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory(debug));
    }
  }


}
