/******************************************************************************
 *   Copyright (C) 2014-2014 Olivier Roulet-Dubonnet          *
 *   olivier.roulet@gmail.com          *
 *                      *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as      *
 *   published by the Free Software Foundation; version 3 of the License.   *
 *                      *
 *   This library is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 *   GNU Lesser General Public License for more details.        *
 *                      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the          *
 *   Free Software Foundation, Inc.,              *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.        *
 ******************************************************************************/

#pragma once

#include <opc/ua/services/services.h>

#include <sstream>


namespace OpcUa
{

class NodeNotFoundException : public std::runtime_error
{
public:
  NodeNotFoundException() : std::runtime_error("NodeNotFoundException") { }
};

/// @brief A Node object represent an OPC-UA node.
/// It is high level object intended for developper who want to expose
/// data through OPC-UA or read data from an OPCUA server.
/// Node are usually not create directly but obtained through call
/// to GetRootNode of GetObjectsNode on server or client side

class Node
{
public:
  // Creating Root Node.
  explicit Node(Services::SharedPtr srv);
  Node(Services::SharedPtr srv, const NodeId & id);
  Node(const Node & other);
  Node() {}

  NodeId GetId() const;

  QualifiedName GetBrowseName() const;
  //void SetBrowseName(const QualifiedName& name) const;

  Node GetParent() const;

  /// @brief List child nodes by specified reference
  /// @return zero or more child nodes.
  std::vector<Node> GetChildren(const OpcUa::ReferenceId & refid) const;

  /// @brief Get ghildren by hierarchal referencies.
  /// @return One or zero chilren nodes.
  std::vector<Node> GetChildren() const;

  //The GetChildNode methods return a node defined by its path from the node. A path is defined by
  // a sequence of browse name(QualifiedName). A browse name is either defined through a qualifiedname object
  // or a string of format namespace:browsename. If a namespace is not specified it is assumed to be
  //the same as the parent
  Node GetChild(const std::vector<OpcUa::QualifiedName> & path) const;
  Node GetChild(const std::vector<std::string> & path) const;
  Node GetChild(const std::string & browsename) const ;

  //TODO: How to get References?
  std::vector<Node> GetProperties() const;
  std::vector<Node> GetVariables() const { return GetChildren(OpcUa::ReferenceId::HasComponent); } //Not correct should filter by variable type

  bool IsValid() const
  {
    try
      {
        auto value = GetAttribute(AttributeId::NodeId).Value;
        if (value.IsNul())
          {
            return false;
          }
        return !value.As<NodeId>().IsNull();
      } catch (std::exception& e) {
        return false;
      }
  }

  //The Read and Write methods read or write attributes of the node
  //FIXME: add possibility to read and write several nodes at once
  DataValue GetAttribute(const AttributeId attr) const;
  void SetAttribute(AttributeId attr, const DataValue & dval) const;
  //std::vector<StatusCode> WriteAttrs(OpcUa::AttributeId attr, const Variant &val);

  // convenience methods for attributes
  void SetAccessLevel(VariableAccessLevel value) { SetAttribute(AttributeId::AccessLevel, DataValue(static_cast<uint8_t>(value))); }
  VariableAccessLevel GetAccessLevel() const { return static_cast<VariableAccessLevel>(GetAttribute(AttributeId::AccessLevel).Value.As<uint8_t>()); }
  void SetDescription(const LocalizedText & value) { SetAttribute(AttributeId::Description, DataValue(value)); }
  LocalizedText GetDescription() const { return GetAttribute(AttributeId::Description).Value.As<LocalizedText>(); }
  void SetNodeClass(NodeClass value) { SetAttribute(AttributeId::NodeClass, DataValue(static_cast<int32_t>(value))); }
  NodeClass GetNodeClass() const { return static_cast<NodeClass>(GetAttribute(AttributeId::NodeClass).Value.As<int32_t>()); }
  void SetUserAccessLevel(VariableAccessLevel value) { SetAttribute(AttributeId::UserAccessLevel, DataValue(static_cast<uint8_t>(value))); }
  VariableAccessLevel GetUserAccessLevel() const { return static_cast<VariableAccessLevel>(GetAttribute(AttributeId::UserAccessLevel).Value.As<uint8_t>()); }
  void SetUserWriteMask(AttributeWriteMask value) { SetAttribute(AttributeId::UserWriteMask, DataValue(static_cast<uint32_t>(value))); }
  AttributeWriteMask GetUserWriteMask() const { return static_cast<AttributeWriteMask>(GetAttribute(AttributeId::UserWriteMask).Value.As<uint32_t>()); }
  void SetWriteMask(AttributeWriteMask value) { SetAttribute(AttributeId::WriteMask, DataValue(static_cast<uint32_t>(value))); }
  AttributeWriteMask GetWriteMask() const { return static_cast<AttributeWriteMask>(GetAttribute(AttributeId::WriteMask).Value.As<uint32_t>()); }

  //Helper method to get/set VALUE attribute of a node (Not all nodes support VALUE attribute)
  Variant GetValue() const;
  DataValue GetDataValue() const;
  void SetValue(const Variant & val) const;
  void SetValue(const DataValue & dval) const;

  Variant GetDataType() const;

  // CallMethod
  std::vector<Variant> CallMethod(NodeId methodId, std::vector<Variant> inputArguments) const;
  std::vector<std::vector<Variant>> CallMethods(std::vector<NodeId> methodIds, std::vector<std::vector<Variant>> inputArguments) const;

  //OpcUa low level methods to to modify address space model
  std::vector<AddNodesResult> AddNodes(std::vector<AddNodesItem> items) const;
  std::vector<StatusCode> AddReferences(std::vector<AddReferencesItem> items) const;


  //Helper classes to modify address space model
  Node AddFolder(const NodeId & folderId, const QualifiedName & browseName) const;
  Node AddFolder(const std::string & nodeid, const std::string & browseName) const;
  Node AddFolder(uint32_t namespaceidx, const std::string & browseName) const;

  Node AddObject(const NodeId & folderId, const QualifiedName & browseName) const;
  Node AddObject(const std::string & nodeid, const std::string & browseName) const;
  Node AddObject(uint32_t namespaceidx, const std::string & browseName) const;

  Node AddVariable(const NodeId & variableId, const QualifiedName & browsename, const Variant & val) const;
  Node AddVariable(uint32_t namespaceidx, const std::string & BrowseName, const Variant & val) const;
  Node AddVariable(const std::string & nodeId, const std::string & browseName, const Variant & val) const;

  Node AddProperty(const NodeId & propertyId, const QualifiedName & browsename, const Variant & val) const;
  Node AddProperty(const std::string & nodeid, const std::string & browseName, const Variant & val) const;
  Node AddProperty(uint32_t namespaceidx, const std::string & browseName, const Variant & val) const;

  Node AddMethod(const NodeId & variableId, const QualifiedName & browsename, std::function<std::vector<OpcUa::Variant> (NodeId context, std::vector<OpcUa::Variant> arguments)> method) const;
  Node AddMethod(uint32_t namespaceidx, const std::string & BrowseName, std::function<std::vector<OpcUa::Variant> (NodeId context, std::vector<OpcUa::Variant> arguments)> method) const;
  Node AddMethod(const std::string & nodeId, const std::string & browseName, std::function<std::vector<OpcUa::Variant> (NodeId context, std::vector<OpcUa::Variant> arguments)> method) const;

  std::string ToString() const;

  bool operator==(Node const & x) const { return Id == x.Id; }
  bool operator!=(Node const & x) const { return Id != x.Id; }

  //FIXME: I need this to create a copy for python binding, another way?
  OpcUa::Services::SharedPtr GetServices() const {return Server;}

protected:
  // base function for GetChildren(), put found children into given nodes
  // parameter
  void _GetChildren(const OpcUa::ReferenceId & refid, std::vector<Node> & nodes) const;

protected:
  OpcUa::Services::SharedPtr Server;
  NodeId Id;
};


std::ostream & operator<<(std::ostream & os, const Node & node);

//FIXME: The following methods should be moved somewhere else!!!

ObjectId VariantTypeToDataType(VariantType vt);


} // namespace OpcUa

