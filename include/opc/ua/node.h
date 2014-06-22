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

#include <opc/ua/server.h>

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
    explicit Node(Remote::Server::SharedPtr srv);
    Node(Remote::Server::SharedPtr srv, const NodeID& id);
    Node(Remote::Server::SharedPtr srv, const NodeID& id, const QualifiedName& name);
    Node(const Node& other); 

    NodeID GetId() const;

    QualifiedName GetName() const;
    void SetName(const QualifiedName& name);

    /// @brief List childrenn nodes by specified reference
    /// @return One or zero chilren nodes.
    std::vector<Node> GetChildren(const OpcUa::ReferenceID& refid) const;

    /// @brief Get ghildren by hierarchal referencies.
    /// @return One or zero chilren nodes.
    std::vector<Node> GetChildren() const;

    //The GetChildNode methods return a node defined by its path from the node. A path is defined by
    // a sequence of browse name(QualifiedName). A browse name is either defined through a qualifiedname object
    // or a string of format namespace:browsename. If a namespace is not specified it is assumed to be
    //the same as the parent
    Node GetChild(const std::vector<OpcUa::QualifiedName>& path) const;
    Node GetChild(const std::vector<std::string>& path) const;
    Node GetChild(const std::string& browsename) const;

    std::vector<Node> GetProperties() const {return GetChildren(OpcUa::ReferenceID::HasProperty);}
    std::vector<Node> GetVariables() const {return GetChildren(OpcUa::ReferenceID::HasComponent);} //Not correct should filter by variable type

    

    //TODO: How to get Referencies?

    //The Read and Write methods read or write attributes of the node
    //FIXME: add possibility to read and write several nodes at once
    Variant GetAttribute(AttributeID attr) const;
    StatusCode SetAttribute(AttributeID attr, const Variant &val);
    //std::vector<StatusCode> WriteAttrs(OpcUa::AttributeID attr, const Variant &val);

    Variant GetValue() const;
    StatusCode SetValue(const Variant& value);

    Variant DataType() const;

    //OpcUa low level methods to to modify address space model
    void AddAttribute(OpcUa::AttributeID attr, const OpcUa::Variant& val); //FIXME: deprecated
    void AddReference(const OpcUa::ReferenceDescription desc); //FIXME: deprecated
    std::vector<AddNodesResult> AddNodes(std::vector<AddNodesItem> items);
    std::vector<StatusCode> AddReferences(std::vector<AddReferencesItem> items);


    //Helper classes to modify address space model
    Node AddFolder(const NodeID& folderId, const QualifiedName& browseName);
    Node AddFolder(const std::string& nodeid, const std::string& browseName); 
    Node AddFolder(const std::string& browseName);

    Node AddObject(const NodeID& folderId, const QualifiedName& browseName);
    Node AddObject(const std::string& nodeid, const std::string& browseName); 
    Node AddObject(const std::string& browseName);

    Node AddVariable(const NodeID& variableId, const QualifiedName& browsename, const Variant& val);
    Node AddVariable(const std::string& BrowseName, const Variant& val);
    Node AddVariable(const std::string& nodeId, const std::string& browseName, const Variant& val); 

    Node AddProperty(const NodeID& propertyId, const QualifiedName& browsename, const Variant& val);
    Node AddProperty(const std::string& nodeid, const std::string& browseName, const Variant& val);
    Node AddProperty(const std::string& browseName, const Variant& val);

    std::string ToString() const;

    bool operator==(Node const& x) const { return Id == x.Id; }
    bool operator!=(Node const& x) const { return Id != x.Id; }
    //Node& operator=(const Node& x) { Id = x.Id; BrowseName = x.BrowseName; Server = x.GetServer(); return *this; }

    //FIXME: I need this to create a copy for python binding, another way?
    OpcUa::Remote::Server::SharedPtr GetServer() const {return Server;} 
    QualifiedName GetCachedName() const {return BrowseName;} 

  protected:
    OpcUa::Remote::Server::SharedPtr Server;
    NodeID Id;
    QualifiedName BrowseName;
  };


  std::ostream& operator<<(std::ostream& os, const Node& node);

  //FIXME: These 2 methods should be moved somewhere else!!!

  ObjectID VariantTypeToDataType(VariantType vt);

} // namespace OpcUa

