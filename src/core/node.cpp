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


#include <iostream>

#include <opc/ua/node.h>

#include <opc/common/object_id.h>
#include <opc/ua/protocol/node_management.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/variable_access_level.h>

namespace OpcUa
{

  Node::Node(Services::SharedPtr srv)
    : Node(srv, NumericNodeID(0, 0), QualifiedName("Null", 0))
  {
  }

  Node::Node(Services::SharedPtr srv, const NodeID& id)
    : Server(srv)
    , Id(id)
  {
    GetName();
  }

  Node::Node(Services::SharedPtr srv, const NodeID& id, const QualifiedName& name)
    : Server(srv)
    , Id(id)
    , BrowseName(name)
  {
  }

  Node::Node(const Node& other)
    : Server(other.Server)
    , Id(other.Id)
    , BrowseName(other.BrowseName)
  {
  }

  NodeID Node::GetId() const
  {
    return Id;
  }

  Variant Node::GetAttribute(const AttributeID attr) const
  {
    ReadParameters params;
    AttributeValueID attribute;
    attribute.Node = Id;
    attribute.Attribute = attr;
    params.AttributesToRead.push_back(attribute);
    std::vector<DataValue> vec =  Server->Attributes()-> Read(params); 
    if ( vec.size() > 0 )
    {
      DataValue dv =  vec.front(); 
      return dv.Value;
    }
    else
    {
      return Variant(); //FIXME: What does it mean when not value is found?
    }
  }

  void Node::SetAttribute(AttributeID attr, const DataValue &dval) const
  {
    WriteValue attribute;
    attribute.Node = Id;
    attribute.Attribute = attr;
    attribute.Data = dval;
    std::vector<StatusCode> codes = Server->Attributes()->Write(std::vector<WriteValue>(1, attribute));
    CheckStatusCode(codes.front());
  }

  void Node::SetValue(const Variant& val) const
  {
    DataValue dval(val);
    SetAttribute(AttributeID::Value, dval);
  }

  void Node::SetValue(const DataValue &dval) const
  {
    SetAttribute(AttributeID::Value, dval);
  }


  std::vector<Node> Node::GetChildren(const ReferenceID& refid) const
  {
    BrowseDescription description;
    description.NodeToBrowse = Id;
    description.Direction = BrowseDirection::Forward;
    description.IncludeSubtypes = true;
    description.NodeClasses = NODE_CLASS_ALL;
    description.ResultMask = REFERENCE_ALL;
    description.ReferenceTypeID =  refid;

    NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = 100;
    std::vector<Node> nodes;
    std::vector<BrowseResult> results = Server->Views()->Browse(query);
    if ( results.empty() )
    {
      return nodes;
    }
    while(!results[0].Referencies.empty())
    {
      for (auto refIt : results[0].Referencies)
      {
        Node node(Server, refIt.TargetNodeID);
        nodes.push_back(node);
      }
      results = Server->Views()->BrowseNext();
      if ( results.empty() )
      {
        return nodes;
      }
    }
    return nodes;
  }

  std::vector<Node> Node::GetChildren() const
  {
    return GetChildren(ReferenceID::HierarchicalReferences);
  }

  QualifiedName Node::GetName(bool force) const
  {
    if ( force || BrowseName == QualifiedName() ){
      Variant var = GetAttribute(AttributeID::BrowseName);
      if (var.Type() != VariantType::QUALIFIED_NAME)
      {
        throw std::runtime_error("Could not retrieve browse name.");
      }
      BrowseName = var.As<QualifiedName>();
    }
    return BrowseName;
  }

  //QualifiedName Node::GetName() const
  //{
    //if (BrowseName == QualifiedName()){
      //GetName();
    //}
    //return BrowseName;
  //} 

  std::vector<AddNodesResult> Node::AddNodes(std::vector<AddNodesItem> items) const
  {
    return Server->NodeManagement()->AddNodes(items);
  }

  std::vector<StatusCode> Node::AddReferences(std::vector<AddReferencesItem> items) const
  {
    return Server->NodeManagement()->AddReferences(items);
  }

  Node Node::GetChild(const std::string& browsename) const
  {
    return GetChild(std::vector<std::string>({browsename}));
  }

  Node Node::GetChild(const std::vector<std::string>& path) const
  {
    std::vector<QualifiedName> vec;
    uint16_t ns = GetName().NamespaceIndex;
    for (std::string str: path)
    {
      QualifiedName qname = ToQualifiedName(str, ns);
      ns = qname.NamespaceIndex;
      vec.push_back(qname);
    }
    return GetChild(vec);
  }


  Node Node::GetChild(const std::vector<QualifiedName>& path) const
  {
    std::vector<RelativePathElement> rpath;
    for (QualifiedName qname: path)
    {
      RelativePathElement el;
      el.TargetName = qname;
      rpath.push_back(el);
    }
    BrowsePath bpath;
    bpath.Path.Elements = rpath;
    bpath.StartingNode = Id;
    std::vector<BrowsePath> bpaths;
    bpaths.push_back(bpath);
    TranslateBrowsePathsParameters params;
    params.BrowsePaths = bpaths;

    std::vector<BrowsePathResult> result = Server->Views()->TranslateBrowsePathsToNodeIds(params);
    CheckStatusCode(result.front().Status);

    NodeID node =result.front().Targets.front().Node ;
    return Node(Server, node);
  }

  std::string Node::ToString() const
  {
    std::ostringstream os;
    os << "Node(" << GetName() << ", " << Id << ")";
    return os.str();
  }

  Node Node::AddFolder(const std::string& nodeid, const std::string& browsename) const
   {
     NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
     QualifiedName qn = ToQualifiedName(browsename, GetName().NamespaceIndex);
     return AddFolder(node, qn);
   }

  Node Node::AddFolder(uint32_t namespaceIdx, const std::string& name) const
  {
    NodeID nodeid = NumericNodeID(Common::GenerateNewID(), namespaceIdx);
    QualifiedName qn = ToQualifiedName(name, namespaceIdx);
    return AddFolder(nodeid, qn);
  }

  Node Node::AddFolder(const NodeID& nodeid, const QualifiedName& browsename) const
  {

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeID = nodeid;
    item.Class = NodeClass::Object;
    item.ReferenceTypeId = ReferenceID::Organizes; 
    item.TypeDefinition = ObjectID::FolderType; 
    ObjectAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.EventNotifier = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));
    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeID, browsename);
  }

  Node Node::AddObject(const std::string& nodeid, const std::string& browsename) const
   {
     NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
     QualifiedName qn = ToQualifiedName(browsename, GetName().NamespaceIndex);
     return AddObject(node, qn);
   }

  Node Node::AddObject(uint32_t ns, const std::string& name) const
  {
    //FIXME: should default namespace be the onde from the parent of the browsename?
    NodeID nodeid = NumericNodeID(Common::GenerateNewID(), ns);
    QualifiedName qn = ToQualifiedName(name, ns);
    return AddObject(nodeid, qn);
  }

  Node Node::AddObject(const NodeID& nodeid, const QualifiedName& browsename) const
  {
    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeID = nodeid;
    item.Class = NodeClass::Object;
    item.ReferenceTypeId = ReferenceID::HasComponent; 
    item.TypeDefinition = ObjectID::BaseObjectType; 
    ObjectAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.EventNotifier = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeID, browsename);
  }

  Node Node::AddVariable(uint32_t ns, const std::string& name, const Variant& val) const
  {
    NodeID nodeid = NumericNodeID(Common::GenerateNewID(), ns);
    QualifiedName qn = ToQualifiedName(name, ns);
    return AddVariable(nodeid, qn, val);
  }

  Node Node::AddVariable(const std::string& nodeid, const std::string& browsename, const Variant& val) const
  {
    NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(browsename, GetName().NamespaceIndex);
    return AddVariable(node, qn, val);
  }

  Node Node::AddVariable(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val) const
  {
    ObjectID datatype = VariantTypeToDataType(val.Type());

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeID = nodeid;
    item.Class = NodeClass::Variable;
    item.ReferenceTypeId = ReferenceID::HasComponent; 
    item.TypeDefinition = ObjectID::BaseDataVariableType; 
    VariableAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.Value = val;
    attr.Type = datatype;
    attr.Rank  = 0;
    attr.Dimensions = val.Dimensions;
    attr.AccessLevel = VariableAccessLevel::CurrentRead;
    attr.UserAccessLevel = VariableAccessLevel::CurrentRead;
    attr.MinimumSamplingInterval = 1;
    attr.Historizing = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeID, browsename);
  }


  Node Node::AddProperty(uint32_t ns, const std::string& name, const Variant& val) const
  {
    NodeID nodeid = NumericNodeID(Common::GenerateNewID(), ns);
    const QualifiedName& qname = ToQualifiedName(name, ns);
    return AddProperty(nodeid, qname, val);
  }

  Node Node::AddProperty(const std::string& nodeid, const std::string& browsename, const Variant& val) const
  {
    NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(browsename, GetName().NamespaceIndex);
    return AddProperty(node, qn, val);
  }

  Node Node::AddProperty(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val) const
  {

    ObjectID datatype = VariantTypeToDataType(val.Type());

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeID = nodeid;
    item.Class = NodeClass::Variable;
    item.ReferenceTypeId = ReferenceID::HasProperty; 
    item.TypeDefinition = ObjectID::PropertyType; 
    VariableAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.Value = val;
    attr.Type = datatype;
    attr.Rank  = 0;
    attr.Dimensions = val.Dimensions;
    attr.AccessLevel = VariableAccessLevel::CurrentRead;
    attr.UserAccessLevel = VariableAccessLevel::CurrentRead;
    attr.MinimumSamplingInterval = 0;
    attr.Historizing = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeID, browsename);

  }

  Variant Node::GetValue() const
  {
    return GetAttribute(AttributeID::Value);
  }

  Variant Node::GetDataType() const
  {
    return GetAttribute(AttributeID::DataType);
  }

} // namespace OpcUa


std::ostream& OpcUa::operator<<(std::ostream& os, const Node& node)
{
  os << node.ToString();
  return os;
}

