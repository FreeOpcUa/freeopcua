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

#pragma once

#include <opc/ua/services/services.h>

namespace OpcUa
{
  namespace Model
  {

    class Reference;
    class Variable;

    class Node
    {
    public:
      Node(NodeID id, Services::SharedPtr services);

      NodeID GetID() const;


      QualifiedName GetBrowseName() const;
      LocalizedText GetDisplayName() const;

      std::vector<Reference> GetReferencies() const;

    public:
      Services::SharedPtr GetServices() const
      {
        return OpcUaServices;
      }

    protected:
      Node(Services::SharedPtr services);

    protected:
      NodeID Id;
      QualifiedName BrowseName;
      LocalizedText DisplayName;
      Services::SharedPtr OpcUaServices;
    };


    class ReferenceType : public Node
    {
    public:
      ReferenceType(NodeID typeId, Services::SharedPtr services);

      ReferenceType Parent() const;

      QualifiedName Name() const;
      QualifiedName InverseName() const;
    };

    class Reference
    {
    public:
      ReferenceType Type();

      Node Source() const;
      Node Target() const;
    };

    class DataType : public Node
    {
    public:
      DataType(NodeID typeID, Services::SharedPtr services);

      bool IsAbstrasct() const;
      DataType Parent() const;

      std::vector<DataType> SubTypes() const;

    private:
      bool IsAbstract = false;
      NodeID ParentTypeID;
    };

    class VariableType : public Node
    {
    public:
      VariableType(NodeID typeID, Services::SharedPtr services);

      // Existing variable type.
      VariableType(Services::SharedPtr services, const NodeID& typeId);

      VariableType Parent() const;
      std::vector<VariableType> SubTypes() const;

      bool IsAbstract() const;
      DataType ValueType() const;

      DataValue GetDefaultValue() const;
      void SetDefaultValue(const DataValue& value);

      std::vector<Variable> GetVariables() const;

      Variable GetVariable(const QualifiedName& name) const;
      Variable GetVariable(const std::vector<QualifiedName>& path) const;

    private:
      bool Abstract = false;
      DataValue DefaultValue;
      NodeID ValueTypeID;
      NodeID ParentTypeID;
    };


    class Variable : public Node
    {
    public:
      Variable(NodeID variableID, Services::SharedPtr services);

      VariableType GetType() const;

      DataValue GetValue() const;
      void SetValue(const DataValue& value);

      std::vector<Variable> Variables() const;
      Variable GetVariable(const QualifiedName& name) const;
      Variable GetVariable(const std::vector<QualifiedName>& path) const;

    private:
      friend class Object;

      Variable(Services::SharedPtr services)
        : Node(services)
      {
      }

    private:
      NodeID TypeID;
    };


    class Object;

    class ObjectType : public Node
    {
    public:
      ObjectType(NodeID objectID, Services::SharedPtr services);

      bool IsAbstract() const;

      std::vector<Variable> Variables() const;
      std::vector<Object> Objects() const;

      std::vector<ObjectType> SubTypes() const;

      ObjectType Parent() const;
    };

    class Object : public Node
    {
    public:
      Object(NodeID objectId, Services::SharedPtr services);
      Object(Object&& object);
      Object(const Object& object);

      ObjectType GetType() const;

      std::vector<Variable> GetVariables() const;
      std::vector<Variable> GetVariable(const QualifiedName& name) const;
      std::vector<Variable> GetVariable(const std::vector<QualifiedName>& name) const;

      std::vector<Object> GetObjects() const;
      Object GetObject(const std::string& name) const;
      Object GetObject(const std::vector<std::string>& name) const;

      Object CreateObject(const ObjectType& type, const QualifiedName& browseName);
      Object CreateObject(const NodeID& newNodeId, const ObjectType& nodeType, const QualifiedName& browseName);
      Object CreateObject(const ObjectType& type, const QualifiedName& browseName, const std::string displayName);
      Object CreateObject(const NodeID& newNodeID, const ObjectType& type, const QualifiedName& browseName, const std::string displayName);

      Variable CreateVariable(const VariableType& type, const QualifiedName& browseName);
      Variable CreateVariable(const VariableType& type, const QualifiedName& browseName, const std::string displayName);

    private:
      Object CreateObject(const NodeID& newNodeID, const NodeID& parentNode, const NodeID& typeID, const QualifiedName& browseName, const std::string displayName);
      Variable CreateVariable(const NodeID& parentNode, const NodeID& typeID, const QualifiedName& browseName, const std::string displayName);

      NodeID InstantiateType(const NodeID& newNodeID, const NodeID& parentNode, const NodeID& typeID, NodeClass nodeClass, const QualifiedName& browseName, const std::string displayName);
      std::vector<ReferenceDescription> BrowseObjectsAndVariables(const NodeID& id);

      std::map<NodeID, std::vector<ReferenceDescription>> CopyObjectsAndVariables(const NodeID& targetNode, const std::vector<ReferenceDescription>& refs);
      AddNodesItem CreateVariableCopy(const NodeID& parentID, const ReferenceDescription& ref);
      AddNodesItem CreateObjectCopy(const NodeID& parentID, const ReferenceDescription& ref);




    private:
      explicit Object(Services::SharedPtr services);
    };

    class Server
    {
    public:
      Server(Services::SharedPtr services);

      Object RootObject() const;
      Object GetObject(const NodeID& id) const;
      ObjectType GetObjectType(const NodeID& typeId) const;

    public:
      Services::SharedPtr GetServices() const;

    private:
      Services::SharedPtr Connection;
    };

  } // namespace Model
} // namespace OpcUa
