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
#include <opc/ua/protocol/object_ids.h> // Ids of standard nodes.

namespace OpcUa
{
  namespace Model
  {

    class Reference;
    class Variable;

    class Node
    {
    public:
      Node(NodeId id, Services::SharedPtr services);

      NodeId GetId() const;


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
      NodeId Id;
      QualifiedName BrowseName;
      LocalizedText DisplayName;
      Services::SharedPtr OpcUaServices;
    };


    class ReferenceType : public Node
    {
    public:
      ReferenceType(NodeId typeId, Services::SharedPtr services);

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
      DataType(NodeId typeId, Services::SharedPtr services);

      bool IsAbstrasct() const;
      DataType Parent() const;

      std::vector<DataType> SubTypes() const;

    private:
      bool IsAbstract = false;
      NodeId ParentTypeId;
    };

    class VariableType : public Node
    {
    public:
      VariableType(NodeId typeId, Services::SharedPtr services);

      // Existing variable type.
      VariableType(Services::SharedPtr services, const NodeId& typeId);

      bool IsBase() const;
      bool IsAbstract() const;
      VariableType Parent() const;
      std::vector<VariableType> SubTypes() const;

      DataType ValueType() const;

      DataValue GetDefaultValue() const;
      void SetDefaultValue(const DataValue& value);

      std::vector<Variable> GetVariables() const;
      Variable GetVariable(const QualifiedName& name) const;
      Variable GetVariable(const RelativePath& path) const;

    private:
      bool Abstract = false;
      DataValue DefaultValue;
      NodeId ValueTypeId;
      NodeId ParentTypeId;
    };


    class Variable : public Node
    {
    public:
      Variable(NodeId variableId, Services::SharedPtr services);

      VariableType GetType() const;

       DataValue GetValue() const;
      void SetValue(const Variant& value);
      void SetValue(const DataValue& value);
      VariantType GetDataType() const;

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
      NodeId TypeId;
      VariantType DataType = VariantType::NUL;
    };


    class Object;

    class ObjectType : public Node
    {
    public:
      ObjectType(NodeId objectId, Services::SharedPtr services);

      bool IsBase() const;
      bool IsAbstract() const;

      std::vector<Variable> Variables() const;
      std::vector<Object> Objects() const;

      std::vector<ObjectType> SubTypes() const;

      ObjectType Parent() const;

    private:
      bool Abstract = false;
      NodeId ParentTypeId;
    };


    class Object : public Node
    {
    public:
      Object(NodeId objectId, Services::SharedPtr services);
      Object(Object&& object);
      Object(const Object& object);

      ObjectType GetType() const;

      /// @brief Get variables of the object.
      std::vector<Variable> GetVariables() const;
      /// @brief Get variable of the object by it name.
      Variable GetVariable(const QualifiedName& name) const;
      /// @brief Get variable by browse path.
      Variable GetVariable(const RelativePath& path) const;

      /// @brief Create a new variable.
      /// @param browseName name of new variable.
      /// @param dataType type of the value of new variable.
      /// @param type Variable type from which will be taken data type and structure.
      /// @description Will be create new node under current object.
      /// @note If id of the new variable is not specified then it will be generated.
      /// @return new variable.
      Variable CreateVariable(const QualifiedName& browseName, const Variant& value);
      Variable CreateVariable(const NodeId& newVariableId, const QualifiedName& browseName, const Variant& value);
      Variable CreateVariable(const QualifiedName& browseName, const VariableType& type);
      Variable CreateVariable(const NodeId& newVariableId, const QualifiedName& browseName, const VariableType& type);

      std::vector<Object> GetObjects() const;
      Object GetObject(const QualifiedName& name) const;
      Object GetObject(const RelativePath& name) const;

      Object CreateObject(const ObjectType& type, const QualifiedName& browseName);
      Object CreateObject(const NodeId& newNodeId, const ObjectType& nodeType, const QualifiedName& browseName);
      Object CreateObject(const ObjectType& type, const QualifiedName& browseName, const std::string& displayName);
      Object CreateObject(const NodeId& newNodeId, const ObjectType& type, const QualifiedName& browseName, const std::string& displayName);

    private:
      Object CreateObject(const NodeId& newNodeId, const NodeId& parentNode, const NodeId& typeId, const QualifiedName& browseName, const std::string& displayName);
      Variable CreateVariable(const NodeId& newNodeId, const NodeId& parentNode, const NodeId& typeId, const QualifiedName& browseName, const std::string& displayName);

      NodeId InstantiateType(const NodeId& newNodeId, const NodeId& parentNode, const NodeId& typeId, NodeClass nodeClass, const QualifiedName& browseName, const std::string& displayName);
      std::vector<ReferenceDescription> BrowseObjectsAndVariables(const NodeId& id);

      std::map<NodeId, std::vector<ReferenceDescription>> CopyObjectsAndVariables(const NodeId& targetNode, const std::vector<ReferenceDescription>& refs);
      AddNodesItem CreateVariableCopy(const NodeId& parentId, const ReferenceDescription& ref);
      AddNodesItem CreateObjectCopy(const NodeId& parentId, const ReferenceDescription& ref);

    private:
      explicit Object(Services::SharedPtr services);
    };

    class Server
    {
    public:
      Server(Services::SharedPtr services);

      Object RootObject() const;
      Object GetObject(const NodeId& id) const;
      ObjectType GetObjectType(const NodeId& typeId) const;

    public:
      Services::SharedPtr GetServices() const;

    private:
      Services::SharedPtr Connection;
    };

  } // namespace Model
} // namespace OpcUa
