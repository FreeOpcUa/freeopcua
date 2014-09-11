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
      : Node(objectId, services)
    {

    }

    ObjectType Object::GetType() const
    {
      return ObjectType(ObjectID::Null, GetServices());
    }

    std::vector<Variable> Object::GetVariables() const
    {
      return std::vector<Variable>();
    }

    std::vector<Variable> Object::GetVariable(const QualifiedName& name) const
    {
      return std::vector<Variable>();
    }

    std::vector<Variable> Object::GetVariable(const std::vector<QualifiedName>& name) const
    {
      return std::vector<Variable>();
    }


    std::vector<Object> Object::Objects() const
    {
      return std::vector<Object>();
    }

    Object Object::GetObject(const std::string& name) const
    {
      return Object(ObjectID::Null, GetServices());
    }

    Object Object::GetObject(const std::vector<std::string>& name) const
    {
      return Object(ObjectID::Null, GetServices());
    }

    Object Object::CreateObject(const ObjectType& type)
    {
      // Reading attributes required for new object
      AttributeServices::SharedPtr attributes = GetServices()->Attributes();
      OpcUa::ReadParameters params;
      params.AttributesToRead.push_back(AttributeValueID(type.GetID(), AttributeID::NODE_ID));
      params.AttributesToRead.push_back(AttributeValueID(type.GetID(), AttributeID::BROWSE_NAME));
      params.AttributesToRead.push_back(AttributeValueID(type.GetID(), AttributeID::DISPLAY_NAME));
      params.AttributesToRead.push_back(AttributeValueID(type.GetID(), AttributeID::DESCRIPTION));

      std::vector<DataValue> result = attributes->Read(params);
      if (result.size() != params.AttributesToRead.size())
      {
        throw std::runtime_error("opcua_model| Server returned wrong number of attributes.");
      }

      // Creating new node for object
      AddNodesItem newNodeRequest;
      newNodeRequest.BrowseName = result[0].Value.As<QualifiedName>();
      newNodeRequest.Class = NodeClass::Object;
      newNodeRequest.ParentNodeId = GetID();
      newNodeRequest.ReferenceTypeId = ObjectID::HasComponent;
      newNodeRequest.TypeDefinition = type.GetID();
      ObjectAttributes attrs;
      attrs.Description = result[3].Value.As<LocalizedText>();
      attrs.DisplayName = result[3].Value.As<LocalizedText>();
      newNodeRequest.Attributes = attrs;

      NodeManagementServices::SharedPtr nodes = GetServices()->NodeManagement();
      std::vector<AddNodesResult> newObjectNode = nodes->AddNodes({newNodeRequest});
      OpcUa::CheckStatusCode(newObjectNode[0].Status);

/*
      const DataValue& value = result[0];
      OpcUa::CheckStatusCode(value.Status);
      if (value.Value.Type != VariantType::NODE_ID || value.Value.Value.Node.size() != 1 || value.Value.Value.Node[0] != type.GetID())
      {
        throw std::runtime_error("opcua_model| Server returned attribute with node class.");
      }

      BrowseDescription desc;
      desc.Direction = BrowseDirection::Forward;
      desc.IncludeSubtypes = false;
      desc.NodeClasses =   NODE_CLASS_OBJECT | NODE_CLASS_VARIABLE | NODE_CLASS_METHOD;
      desc.ReferenceTypeID = ObjectID::HierarchicalReferences;
      desc.NodeToBrowse = GetID();
      desc.ResultMask = REFERENCE_TYPE | REFERENCE_IS_FORWARD | REFERENCE_NODE_CLASS | REFERENCE_BROWSE_NAME | REFERENCE_DISPLAY_NAME | REFERENCE_TYPE_DEFINITION;

      NodesQuery query;
      query.NodesToBrowse.push_back(desc);
      ViewServices::SharedPtr views = GetServices()->Views();
      std::vector<ReferenceDescription> refs = views->Browse(query);
*/

      return Object(newObjectNode[0].AddedNodeID, GetServices());
    }


  } // namespace Model
} // namespace OpcUa
