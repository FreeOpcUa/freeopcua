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



#include <opc/ua/node.h>

#include <opc/common/object_id.h>
#include <opc/ua/protocol/node_management.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/variable_access_level.h>

namespace OpcUa
{

  Node::Node(Remote::Server::SharedPtr srv)
    : Node(srv, ObjectID::RootFolder)
  {
    BrowseName = GetName();
  }

  Node::Node(Remote::Server::SharedPtr srv, const NodeID& id)
    : Server(srv)
    , Id(id)
    , BrowseName(GetName())
  {
  }

  Node::Node(Remote::Server::SharedPtr srv, const NodeID& id, const QualifiedName& name)
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

  Variant Node::GetAttribute(const OpcUa::AttributeID attr) const
  {
    ReadParameters params;
    AttributeValueID attribute;
    attribute.Node = Id;
    attribute.Attribute = attr;
    params.AttributesToRead.push_back(attribute);
    std::vector<DataValue> vec =  Server->Attributes()-> Read(params); 
    if ( vec.size() > 0 )
    {
      DataValue dv =  Server->Attributes()->Read(params).front(); 
      return dv.Value;
    }
    else
    {
      return DataValue(); //FIXME: What does it mean when not value is found?
    }
  }

  StatusCode Node::SetAttribute(const OpcUa::AttributeID attr, const Variant &value)
  {
    OpcUa::WriteValue attribute;
    attribute.Node = Id;
    attribute.Attribute = attr;
    attribute.Data = value;
    std::vector<StatusCode> codes = Server->Attributes()->Write(std::vector<OpcUa::WriteValue>(1, attribute));
    return codes.front();
  }

  StatusCode Node::SetValue(const Variant& value)
  {
    return SetAttribute(OpcUa::AttributeID::VALUE, value);
  }

  std::vector<Node> Node::GetChildren(const OpcUa::ReferenceID& refid) const
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = Id;
    description.Direction = OpcUa::BrowseDirection::Forward;
    description.IncludeSubtypes = true;
    description.NodeClasses = OpcUa::NODE_CLASS_ALL;
    description.ResultMask = OpcUa::REFERENCE_ALL;
    description.ReferenceTypeID =  refid;

    OpcUa::NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = 100;
    std::vector<Node> nodes;
    std::vector<OpcUa::ReferenceDescription> refs = Server->Views()->Browse(query);
    while(!refs.empty())
    {
      for (auto refIt : refs)
      {
        Node node(Server, refIt.TargetNodeID);
        nodes.push_back(node);
      }
      refs = Server->Views()->BrowseNext();
    }
    return nodes;
  }

  std::vector<Node> Node::GetChildren() const
  {
    return GetChildren(ReferenceID::HierarchicalReferences);
  }

  QualifiedName Node::GetName() const
  {
    Variant var = GetAttribute(OpcUa::AttributeID::BROWSE_NAME);
    if (var.Type == OpcUa::VariantType::QUALIFIED_NAME)
    {
      return var.Value.Name.front();
    }

    return QualifiedName(); // TODO Exception!
  }

  void Node::AddAttribute(OpcUa::AttributeID attr, const OpcUa::Variant& val)
  {
    return Server->NodeManagement()->AddAttribute(Id, attr, val);
  }

  void Node::AddReference(const OpcUa::ReferenceDescription desc)
  {
    return Server->NodeManagement()->AddReference(Id, desc);
  }

  std::vector<AddNodesResult> Node::AddNodes(std::vector<AddNodesItem> items)
  {
    return Server->NodeManagement()->AddNodes(items);
  }

  std::vector<StatusCode> Node::AddReferences(std::vector<AddReferencesItem> items)
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
    uint16_t ns = BrowseName.NamespaceIndex;
    for (std::string str: path)
    {
      QualifiedName qname = OpcUa::ToQualifiedName(str, ns);
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

    if ( result.front().Status == OpcUa::StatusCode::Good )
    {
      NodeID node =result.front().Targets.front().Node ;
      return Node(Server, node);
    }
    else
    {
      throw NodeNotFoundException();
    }
  }

  std::string Node::ToString() const
  {
    std::ostringstream os;
    os << "Node(" << BrowseName << ", " << Id << ")";
    return os.str();
  }

  Node Node::AddFolder(const std::string& nodeid, const std::string& browsename)
   {
     NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
     QualifiedName qn = ToQualifiedName(browsename, this->BrowseName.NamespaceIndex);
     return AddFolder(node, qn);
   }

  Node Node::AddFolder(const std::string& name)
  {
    NodeID nodeid = OpcUa::NumericNodeID(Common::GenerateNewID(), this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(name, BrowseName.NamespaceIndex);
    return AddFolder(nodeid, qn);
  }

  Node Node::AddFolder(const NodeID& nodeid, const QualifiedName& browsename)
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
    if ( res.Status != StatusCode::Good )
    {
      throw std::runtime_error("Error while adding node"); //FIXME: Should return exception with better explanation
    }
    AddReferencesItem refitem;
    refitem.SourceNodeID = res.AddedNodeID;
    refitem.TargetNodeID = ObjectID::FolderType;
    refitem.TargetNodeClass = NodeClass::ObjectType;
    refitem.IsForward = true;
    refitem.ReferenceTypeId = ReferenceID::HasTypeDefinition;
    //refitem.TargetServerUri =  //Necessary in case the ref is on another server, not supported
    Server->NodeManagement()->AddReferences(std::vector<AddReferencesItem>({refitem}));

    return Node(Server, res.AddedNodeID, browsename);
  }

  Node Node::AddObject(const std::string& nodeid, const std::string& browsename)
   {
     NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
     QualifiedName qn = ToQualifiedName(browsename, this->BrowseName.NamespaceIndex);
     return AddObject(node, qn);
   }

  Node Node::AddObject(const std::string& name)
  {
    //FIXME: should default namespace be the onde from the parent of the browsename?
    NodeID nodeid = NumericNodeID(Common::GenerateNewID(), this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(name, BrowseName.NamespaceIndex);
    return AddObject(nodeid, qn);
  }

  Node Node::AddObject(const NodeID& nodeid, const QualifiedName& browsename)
  {
    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeID = nodeid;
    item.Class = NodeClass::Object;
    item.ReferenceTypeId = ReferenceID::Organizes; // FIXME check
    item.TypeDefinition = ObjectID::Null; //FIXME: check
    ObjectAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.EventNotifier = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    if ( res.Status != StatusCode::Good )
    {
      throw std::runtime_error("Error while adding node"); //FIXME: Should return exception with better explanation
    }

    AddReferencesItem refitem;
    refitem.SourceNodeID = res.AddedNodeID;
    refitem.TargetNodeID = ObjectID::BaseObjectType;
    refitem.TargetNodeClass = NodeClass::ObjectType;
    refitem.IsForward = true;
    refitem.ReferenceTypeId = ReferenceID::HasTypeDefinition;
    //refitem.TargetServerUri =  //Necessary in case the ref is on another server, not supported
    Server->NodeManagement()->AddReferences(std::vector<AddReferencesItem>({refitem}));

    return Node(Server, res.AddedNodeID, browsename);
  }

  Node Node::AddVariable(const std::string& name, const Variant& val)
  {
    NodeID nodeid = OpcUa::NumericNodeID(Common::GenerateNewID(), this->Id.GetNamespaceIndex());
    QualifiedName qn = OpcUa::ToQualifiedName(name, BrowseName.NamespaceIndex);
    return AddVariable(nodeid, qn, val);
  }

  Node Node::AddVariable(const std::string& nodeid, const std::string& browsename, const Variant& val)
  {
    NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(browsename, this->BrowseName.NamespaceIndex);
    return AddVariable(node, qn, val);
  }

  Node Node::AddVariable(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val)
  {
    ObjectID datatype = VariantTypeToDataType(val.Type);

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeID = nodeid;
    item.Class = NodeClass::Variable;
    item.ReferenceTypeId = ReferenceID::HasComponent; 
    item.TypeDefinition = ObjectID::BaseVariableType; 
    VariableAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.Value = val;
    attr.Type = datatype;
    attr.Rank  = 0;
    attr.Dimensions = val.Dimensions;
    attr.AccessLevel = 0;
    attr.UserAccessLevel = 0;
    attr.MinimumSamplingInterval = 1;
    attr.Historizing = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    if ( res.Status != StatusCode::Good )
    {
      throw std::runtime_error("Error while adding node"); //FIXME: Should return exception with better explanation
    }
    AddReferencesItem refitem;
    refitem.SourceNodeID = res.AddedNodeID;
    refitem.TargetNodeID = ObjectID::BaseDataVariableType;
    refitem.TargetNodeClass = NodeClass::DataType;
    refitem.IsForward = true;
    refitem.ReferenceTypeId = ReferenceID::HasTypeDefinition;
    //refitem.TargetServerUri =  //Necessary in case the ref is on another server, not supported
    Server->NodeManagement()->AddReferences(std::vector<AddReferencesItem>({refitem}));

    return Node(Server, res.AddedNodeID, browsename);
  }


  Node Node::AddProperty(const std::string& name, const Variant& val)
  {
    NodeID nodeid = NumericNodeID(Common::GenerateNewID(), this->Id.GetNamespaceIndex());
    const QualifiedName& qname = ToQualifiedName(name, BrowseName.NamespaceIndex);
    return AddProperty(nodeid, qname, val);
  }

  Node Node::AddProperty(const std::string& nodeid, const std::string& browsename, const Variant& val)
  {
    NodeID node = ToNodeID(nodeid, this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(browsename, this->BrowseName.NamespaceIndex);
    return AddProperty(node, qn, val);
  }

  Node Node::AddProperty(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val)
  {

    ObjectID datatype = VariantTypeToDataType(val.Type);

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
    attr.AccessLevel = 0;
    attr.UserAccessLevel = 0;
    attr.MinimumSamplingInterval = 0;
    attr.Historizing = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    if ( res.Status != StatusCode::Good )
    {
      throw std::runtime_error("Error while adding node"); //FIXME: Should return exception with better explanation
    }
    AddReferencesItem refitem;
    refitem.SourceNodeID = res.AddedNodeID;
    refitem.TargetNodeID = ObjectID::PropertyType;
    refitem.TargetNodeClass = NodeClass::DataType;
    refitem.IsForward = true;
    refitem.ReferenceTypeId = ReferenceID::HasTypeDefinition;
    //refitem.TargetServerUri =  //Necessary in case the ref is on another server, not supported
    Server->NodeManagement()->AddReferences(std::vector<AddReferencesItem>({refitem}));

    return Node(Server, res.AddedNodeID, browsename);

  }

  Variant Node::GetValue() const
  {
    return GetAttribute(AttributeID::VALUE);
  }
  Variant Node::DataType() const
  {
    return GetAttribute(AttributeID::DATA_TYPE);
  }

} // namespace OpcUa


//FIXME: This should be somewhere else, maybe wariant.h. And maybe there is another way or this is wrong
OpcUa::ObjectID OpcUa::VariantTypeToDataType(OpcUa::VariantType vt)
{
  switch (vt)
  {
    case VariantType::BOOLEAN:          return ObjectID::Boolean;
    case VariantType::SBYTE:            return ObjectID::SByte;
    case VariantType::BYTE:             return ObjectID::Byte;
    case VariantType::INT16:            return ObjectID::Int16;
    case VariantType::UINT16:           return ObjectID::UInt16;
    case VariantType::INT32:            return ObjectID::Int32;
    case VariantType::UINT32:           return ObjectID::UInt32;
    case VariantType::INT64:            return ObjectID::Int64;
    case VariantType::UINT64:           return ObjectID::UInt64;
    case VariantType::FLOAT:            return ObjectID::Float;
    case VariantType::DOUBLE:           return ObjectID::Double;
    case VariantType::STRING:           return ObjectID::String;
    case VariantType::DATE_TIME:        return ObjectID::DateTime;
    case VariantType::GUID:             return ObjectID::Guid;
    case VariantType::BYTE_STRING:      return ObjectID::ByteString;
    case VariantType::XML_ELEMENT:      return ObjectID::XmlElement;
    case VariantType::NODE_ID:          return ObjectID::NodeID;
    case VariantType::EXPANDED_NODE_ID: return ObjectID::ExpandedNodeID;
    case VariantType::STATUS_CODE:      return ObjectID::StatusCode;
    case VariantType::QUALIFIED_NAME:   return ObjectID::QualifiedName;
    case VariantType::LOCALIZED_TEXT:   return ObjectID::LocalizedText;
    case VariantType::DIAGNOSTIC_INFO:  return ObjectID::DiagnosticInfo;
    case VariantType::DATA_VALUE:       return ObjectID::DataValue;
    case VariantType::NUL:              return ObjectID::Null;
    case VariantType::EXTENSION_OBJECT:
    case VariantType::VARIANT:
    default:
      throw std::logic_error("Unknown variant type.");
  }
}

std::ostream& OpcUa::operator<<(std::ostream& os, const Node& node)
{
  os << node.ToString();
  return os;
}

