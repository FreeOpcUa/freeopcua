/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include <opc/ua/model.h>

namespace OpcUa
{
  namespace Model
  {
    Object::Object(NodeID objectId, Services::SharedPtr services)
      : Node(services)
    {
      Id = objectId;
      ReadParameters attrs;
      attrs.AttributesToRead.push_back(AttributeValueID(objectId, AttributeID::DISPLAY_NAME));
      attrs.AttributesToRead.push_back(AttributeValueID(objectId, AttributeID::BROWSE_NAME));
      std::vector<DataValue> values = services->Attributes()->Read(attrs);
      DisplayName = values[0].Value.As<LocalizedText>();
      BrowseName = values[1].Value.As<QualifiedName>();
    }

    Object::Object(Object&& object)
      : Node(std::move(object.OpcUaServices))
    {
      Id = std::move(object.Id);
      DisplayName = std::move(object.DisplayName);
      BrowseName = std::move(object.BrowseName);
    }

    Object::Object(const Object& object)
      : Node(object.OpcUaServices)
    {
      Id = object.Id;
      DisplayName = object.DisplayName;
      BrowseName = object.BrowseName;
    }

    Object::Object(Services::SharedPtr services)
      : Node(services)
    {
    }

    ObjectType Object::GetType() const
    {
      return ObjectType(ObjectID::Null, GetServices());
    }

    std::vector<Variable> Object::GetVariables() const
    {
      BrowseDescription desc;
      desc.Direction = BrowseDirection::Forward;
      desc.IncludeSubtypes = true;
      desc.NodeClasses =   NODE_CLASS_VARIABLE;
      desc.ReferenceTypeID = ObjectID::HierarchicalReferences;
      desc.NodeToBrowse = GetID();
      desc.ResultMask = 0;

      NodesQuery query;
      query.NodesToBrowse.push_back(desc);
      Services::SharedPtr services = GetServices();
      ViewServices::SharedPtr views = OpcUaServices->Views();
      std::vector<ReferenceDescription> refs = views->Browse(query);

      std::vector<Variable> vars;
      std::for_each(refs.begin(), refs.end(), [&services, &vars](const ReferenceDescription& ref){
        Variable var(services);
        var.Id = ref.TargetNodeID;
        var.BrowseName = ref.BrowseName;
        var.DisplayName = ref.DisplayName;
        vars.push_back(var);
      });

      return vars;
    }

    std::vector<Variable> Object::GetVariable(const QualifiedName& name) const
    {
      return std::vector<Variable>();
    }

    std::vector<Variable> Object::GetVariable(const std::vector<QualifiedName>& name) const
    {
      return std::vector<Variable>();
    }


    std::vector<Object> Object::GetObjects() const
    {
      BrowseDescription desc;
      desc.Direction = BrowseDirection::Forward;
      desc.IncludeSubtypes = true;
      desc.NodeClasses =   NODE_CLASS_OBJECT;
      desc.ReferenceTypeID = ObjectID::HierarchicalReferences;
      desc.NodeToBrowse = GetID();
      desc.ResultMask = 0;

      NodesQuery query;
      query.NodesToBrowse.push_back(desc);
      Services::SharedPtr services = GetServices();
      ViewServices::SharedPtr views = OpcUaServices->Views();
      std::vector<ReferenceDescription> refs = views->Browse(query);

      std::vector<Object> objects;
      std::for_each(refs.begin(), refs.end(), [&services, &objects](const ReferenceDescription& ref){
        Object object(services);
        object.Id = ref.TargetNodeID;
        object.BrowseName = ref.BrowseName;
        object.DisplayName = ref.DisplayName;
        objects.push_back(object);
      });

      return objects;
    }

    Object Object::GetObject(const std::string& name) const
    {
      return Object(ObjectID::Null, GetServices());
    }

    Object Object::GetObject(const std::vector<std::string>& name) const
    {
      return Object(ObjectID::Null, GetServices());
    }

    Object Object::CreateObject(const ObjectType& type, const QualifiedName& browseName)
    {
      return CreateObject(NodeID(), type, browseName);
    }

    Object Object::CreateObject(const NodeID& newNodeId, const ObjectType& nodeType, const QualifiedName& browseName)
    {
      return CreateObject(newNodeId, GetID(), nodeType.GetID(), browseName, browseName.Name);
    }

    Object Object::CreateObject(const ObjectType& type, const QualifiedName& browseName, const std::string displayName)
    {
      return CreateObject(NodeID(), GetID(), type.GetID(), browseName, displayName);
    }

    Object Object::CreateObject(const NodeID& newNodeID, const NodeID& parentNode, const NodeID& typeID, const QualifiedName& browseName, const std::string displayName)
    {
      Object object(GetServices());
      object.Id = InstantiateType(newNodeID, parentNode, typeID, NodeClass::Object, browseName, displayName);
      object.BrowseName = browseName;
      object.DisplayName = LocalizedText(displayName);
      return object;

    }

    NodeID Object::InstantiateType(const NodeID& newNodeID, const NodeID& parentNode, const NodeID& typeID, NodeClass nodeClass, const QualifiedName& browseName, const std::string displayName)
    {
      // Creating new node for object
      AddNodesItem newNodeRequest;
      newNodeRequest.BrowseName = browseName;
      newNodeRequest.RequestedNewNodeID = newNodeID;
      newNodeRequest.Class = nodeClass;
      newNodeRequest.ParentNodeId = parentNode;
      newNodeRequest.ReferenceTypeId = nodeClass == NodeClass::Object ? ObjectID::HasComponent : ObjectID::HasProperty;
      newNodeRequest.TypeDefinition = typeID;
      ObjectAttributes attrs;
      attrs.Description = LocalizedText(displayName);
      attrs.DisplayName = LocalizedText(displayName);
      newNodeRequest.Attributes = attrs;

      NodeManagementServices::SharedPtr nodes = GetServices()->NodeManagement();
      std::vector<AddNodesResult> newObjectNode = nodes->AddNodes({newNodeRequest});
      if (newObjectNode.size() != 1)
      {
        throw std::runtime_error("opcua_model| Server returned wrong number new nodes results.");
      }

      OpcUa::CheckStatusCode(newObjectNode[0].Status);

      std::map<NodeID, std::vector<ReferenceDescription>> nextRefs;
      nextRefs.insert({newObjectNode[0].AddedNodeID, BrowseObjectsAndVariables(typeID)});
      while(!nextRefs.empty())
      {
        std::map<NodeID, std::vector<ReferenceDescription>> newRefs;
        for (auto idRefs : nextRefs)
        {
          std::map<NodeID, std::vector<ReferenceDescription>> tmpRefs = CopyObjectsAndVariables(idRefs.first, idRefs.second);
          newRefs.insert(tmpRefs.begin(), tmpRefs.end());
        }
        nextRefs = std::move(newRefs);
      }
      return newObjectNode[0].AddedNodeID;
    }

    std::vector<ReferenceDescription> Object::BrowseObjectsAndVariables(const NodeID& id)
    {
      // ID of the new node.
      BrowseDescription desc;
      desc.Direction = BrowseDirection::Forward;
      desc.IncludeSubtypes = true;
      desc.NodeClasses =   NODE_CLASS_OBJECT | NODE_CLASS_VARIABLE | NODE_CLASS_METHOD;
      desc.ReferenceTypeID = ObjectID::HierarchicalReferences;
      desc.NodeToBrowse = id;
      desc.ResultMask = REFERENCE_NODE_CLASS | REFERENCE_TYPE_DEFINITION | REFERENCE_BROWSE_NAME | REFERENCE_DISPLAY_NAME;

      // browse sub objects and variables.
      NodesQuery query;
      query.NodesToBrowse.push_back(desc);
      ViewServices::SharedPtr views = GetServices()->Views();
      return views->Browse(query);
    }

    std::map<NodeID, std::vector<ReferenceDescription>> Object::CopyObjectsAndVariables(const NodeID& targetNode, const std::vector<ReferenceDescription>& refs)
    {
      std::map<NodeID, std::vector<ReferenceDescription>> nextCopyData;
      for (const ReferenceDescription& ref : refs)
      {
        std::vector<AddNodesResult> result;
        std::vector<AddNodesItem> newNodeRequest;
        switch (ref.TargetNodeClass)
        {
          case NodeClass::Object:
          {
            if (ref.TargetNodeTypeDefinition !=ObjectID::Null)
            {
              InstantiateType(NodeID(), targetNode, ref.TargetNodeTypeDefinition, NodeClass::Object, ref.BrowseName, ref.DisplayName.Text);
            }
            else
            {
              newNodeRequest = {CreateObjectCopy(targetNode, ref)};
            }
            break;
          }
          case NodeClass::Variable:
          {
            newNodeRequest = {CreateVariableCopy(targetNode, ref)};
            break;
          }
          default:
          {
            continue;
          }
        }
        if (newNodeRequest.empty())
        {
          continue;
        }
        result = GetServices()->NodeManagement()->AddNodes(newNodeRequest);
        std::vector<ReferenceDescription> newRefs = BrowseObjectsAndVariables(ref.TargetNodeID);
        nextCopyData.insert({result[0].AddedNodeID, newRefs});
      }
      return nextCopyData;
    }

    Variable Object::CreateVariable(const VariableType& type, const QualifiedName& browseName)
    {
      return CreateVariable(type, browseName, browseName.Name);
    }

    Variable Object::CreateVariable(const VariableType& type, const QualifiedName& browseName, const std::string displayName)
    {
      return CreateVariable(GetID(), type.GetID(), browseName, displayName);
    }

    Variable Object::CreateVariable(const NodeID& parentNode, const NodeID& typeID, const QualifiedName& browseName, const std::string displayName)
    {
      Variable variable(GetServices());
      return variable;
    }

    AddNodesItem Object::CreateVariableCopy(const NodeID& parentID, const ReferenceDescription& ref)
    {
      const NodeID& nodeID = ref.TargetNodeID;

      ReadParameters readParams;
      readParams.AttributesToRead.push_back({nodeID, AttributeID::DISPLAY_NAME});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::DESCRIPTION});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::VALUE});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::DATA_TYPE});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::VALUE_RANK});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::ARRAY_DIMENSIONS});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::ACCESS_LEVEL});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::USER_ACCESS_LEVEL});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::MINIMUM_SAMPLING_INTERVAL});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::HISTORIZING});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::WRITE_MASK});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::USER_WRITE_MASK});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::BROWSE_NAME});
      std::vector<DataValue> values = GetServices()->Attributes()->Read(readParams);

      VariableAttributes attrs;
      attrs.DisplayName = values[0].Value.As<LocalizedText>();
      attrs.Description = values[1].Value.As<LocalizedText>();
      attrs.Value = values[2].Value;
      attrs.Type = values[3].Value.As<NodeID>();
      attrs.Rank = values[4].Value.As<int32_t>();
      attrs.Dimensions = values[5].Value.As<std::vector<uint32_t>>();
      attrs.AccessLevel = static_cast<VariableAccessLevel>(values[6].Value.As<uint8_t>());
      attrs.UserAccessLevel = static_cast<VariableAccessLevel>(values[7].Value.As<uint8_t>());
      attrs.MinimumSamplingInterval = values[8].Value.As<Duration>();
      attrs.Historizing = values[9].Value.As<bool>();
      attrs.WriteMask = values[10].Value.As<uint32_t>();
      attrs.UserWriteMask = values[11].Value.As<uint32_t>();

      AddNodesItem newNode;
      newNode.BrowseName = values[12].Value.As<QualifiedName>();
      newNode.Class = NodeClass::Variable;
      newNode.ParentNodeId = parentID;
      newNode.ReferenceTypeId = ref.ReferenceTypeID;
      newNode.TypeDefinition = ref.TargetNodeTypeDefinition;
      newNode.Attributes = attrs;
      return newNode;
    }

    AddNodesItem Object::CreateObjectCopy(const NodeID& parentID, const ReferenceDescription& ref)
    {
      const NodeID& nodeID = ref.TargetNodeID;

      ReadParameters readParams;
      readParams.AttributesToRead.push_back({nodeID, AttributeID::DISPLAY_NAME});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::DESCRIPTION});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::WRITE_MASK});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::USER_WRITE_MASK});
      readParams.AttributesToRead.push_back({nodeID, AttributeID::BROWSE_NAME});
      std::vector<DataValue> values = GetServices()->Attributes()->Read(readParams);

      ObjectAttributes attrs;
      attrs.DisplayName = values[0].Value.As<LocalizedText>();
      attrs.Description = values[1].Value.As<LocalizedText>();
      attrs.WriteMask = values[2].Value.As<uint32_t>();
      attrs.UserWriteMask = values[3].Value.As<uint32_t>();

      AddNodesItem newNode;
      newNode.BrowseName = values[4].Value.As<QualifiedName>();
      newNode.Class = NodeClass::Object;
      newNode.ParentNodeId = parentID;
      newNode.ReferenceTypeId = ref.ReferenceTypeID;
      newNode.TypeDefinition = ref.TargetNodeTypeDefinition;
      newNode.Attributes = attrs;
      return newNode;
    }

  } // namespace Model
} // namespace OpcUa
