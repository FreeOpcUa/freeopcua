/// @author Alexander Rykovttributeanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

// TODO Add EventNotifier Attribute to all requred nodes.

#include "xml_address_space_loader.h"
#include "xml_processor.h"

#include <opc/ua/server/address_space.h>

#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>
#include <string.h>

namespace
{
  using namespace OpcUa;

  ReferenceID GetReferenceID(const std::string& referenceName)
  {
    if (referenceName == "organizes")
      return ReferenceID::Organizes;
    if (referenceName == "references_to")
      return ReferenceID::References;
    if (referenceName == "has_child")
      return ReferenceID::HasChild;
    if (referenceName == "has_event_source")
      return ReferenceID::HasEventSource;
    if (referenceName == "has_modelling_rule")
      return ReferenceID::HasModellingRule;
    if (referenceName == "has_encoding")
      return ReferenceID::HasEncoding;
    if (referenceName == "has_description")
      return ReferenceID::HasDescription;
    if (referenceName == "has_type_definition")
      return ReferenceID::HasTypeDefinition;
    if (referenceName == "generates_event")
      return ReferenceID::GeneratesEvent;
    if (referenceName == "aggregates")
      return ReferenceID::Aggregates;
    if (referenceName == "has_subtype")
      return ReferenceID::HasSubtype;
    if (referenceName == "has_property")
      return ReferenceID::HasProperty;
    if (referenceName == "has_component")
      return ReferenceID::HasComponent;
    if (referenceName == "has_notifier")
      return ReferenceID::HasNotifier;
    if (referenceName == "has_ordered_component")
      return ReferenceID::HasOrderedComponent;
    if (referenceName == "has_model_parent")
      return ReferenceID::HasModelParent;
    if (referenceName == "from_state")
      return ReferenceID::FromState;
    if (referenceName == "to_state")
      return ReferenceID::ToState;
    if (referenceName == "has_clause")
      return ReferenceID::HasCause;
    if (referenceName == "has_effect")
      return ReferenceID::HasEffect;
    if (referenceName == "has_historical_configuration")
      return ReferenceID::HasHistoricalConfiguration;
    if (referenceName == "has_historical_event_configuration")
      return ReferenceID::HasHistoricalEventConfiguration;
    if (referenceName == "has_substate_machine")
      return ReferenceID::HasSubStateMachine;
    if (referenceName == "has_event_history")
      return ReferenceID::HasEventHistory;
    if (referenceName == "always_generates_event")
      return ReferenceID::AlwaysGeneratesEvent;
    if (referenceName == "has_true_substate")
      return ReferenceID::HasTrueSubState;
    if (referenceName == "has_false_substate")
      return ReferenceID::HasFalseSubState;
    if (referenceName == "has_condition")
      return ReferenceID::HasCondition;
    if (referenceName == "non_hierarchical_references")
      return ReferenceID::NonHierarchicalReferences;
    if (referenceName == "hierarchical_references")
      return ReferenceID::HierarchicalReferences;
    if (referenceName == "has_cause")
      return ReferenceID::HasCause;
    if (referenceName == "has_sub_state_machine")
      return ReferenceID::HasSubStateMachine;
    if (referenceName == "has_true_sub_state")
      return ReferenceID::HasTrueSubState;
    if (referenceName == "has_false_sub_state")
      return ReferenceID::HasFalseSubState;

    throw std::logic_error(std::string("Unknown reference name '") + referenceName + std::string(referenceName));
  }

  struct Reference
  {
    ReferenceID ID;
    NodeID TargetNode;
    bool IsForward;
    NodeClass TargetClass;
    QualifiedName TargetBrowseName;
    LocalizedText TargetDisplayName;
    NodeID TargetType;

    Reference()
      : ID(ReferenceID::Unknown)
      , IsForward(true)
      , TargetClass(NodeClass::All)
    {
    }
  };

  struct Attribute
  {
    AttributeID ID;
    Variant Value;
    Attribute()
      : ID(AttributeID::Unknown)
    {
    }

    Attribute(AttributeID id, Variant value)
      : ID(id)
      , Value(value)
    {
    }
  };

  struct INode
  {
    NodeID ID;
    std::map<AttributeID, Variant> Attributes;
    std::vector<Reference> References;
    bool IsExternal;

    INode()
      : IsExternal(false)
    {
    }
  };

  struct XmlDocDeleter
  {
    void operator() (xmlDocPtr doc)
    {
      xmlFreeDoc(doc);
    }
  };

  struct LibXmlFree
  {
    void operator() (void* ptr)
    {
      xmlFree(ptr);
    }
  };

  int xmlStrcmp(const xmlChar* xmlStr, const char* str)
  {
    return ::xmlStrcmp(xmlStr, (const xmlChar*)str);
  }

  bool IsXmlNode(const xmlNode& node)
  {
    return node.type == XML_ELEMENT_NODE;
  }

  bool IsXmlNode(const xmlNode& node, const char* name, bool debug = false)
  {
    if (node.type != XML_ELEMENT_NODE)
    {
      return false;
    }

    if (xmlStrcmp(node.name, name))
    {
      return false;
    }
    return true;
  }

  std::string GetNodeName(xmlNode& node)
  {
    if (!node.name)
    {
      return std::string();
    }
    return (const char*)node.name;
  }

  std::string GetProperty(xmlNode& node, const char* propName)
  {
    std::unique_ptr<xmlChar, LibXmlFree> attrValue(xmlGetProp(&node, (const xmlChar*)propName), LibXmlFree());
    const xmlChar* propValue = attrValue.get();
    if (propValue)
    {
      return (const char*)propValue;
    }
    return std::string();
  }

  std::string GetNodeValue(xmlNode& node)
  {
    const std::string nodeValue = GetProperty(node, "value");
    if (!nodeValue.empty())
    {
      return nodeValue;
    }

    std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
    if (!content)
    {
      return std::string();
    }
    return (const char*)content.get();
  }

  ObjectID GetObjectIdOfType(const std::string& nodeValue)
  {
    if (nodeValue == "bool")
    {
      return ObjectID::Boolean;
    }
    if (nodeValue == "sbyte")
    {
      return ObjectID::SByte;
    }
    if (nodeValue == "byte")
    {
      return ObjectID::Byte;
    }
    if (nodeValue == "int16")
    {
      return ObjectID::Int16;
    }
    if (nodeValue == "uint16")
    {
      return ObjectID::UInt16;
    }
    if (nodeValue == "int32")
    {
      return ObjectID::Int32;
    }
    if (nodeValue == "uint32")
    {
      return ObjectID::UInt32;
    }
    if (nodeValue == "string")
    {
      return ObjectID::String;
    }
    if (nodeValue == "enum")
    {
      return ObjectID::Enumeration;
    }
    if (nodeValue == "int")
    {
      return ObjectID::Integer;
    }
    if (nodeValue == "byte_string")
    {
      return ObjectID::ByteString;
    }
    if (nodeValue == "guid")
    {
      return ObjectID::Guid;
    }
    if (nodeValue == "date_time")
    {
      return ObjectID::DateTime;
    }

    std::stringstream stream;
    stream << "Unknown data type '" << nodeValue << "'.";
    throw std::logic_error(stream.str());
  }

  inline ObjectID GetObjectIdOfType(xmlNode& node)
  {
    return GetObjectIdOfType(GetNodeValue(node));
  }

  inline VariantType ConvertToVariantType(ObjectID id)
  {
    switch (id)
    {
      case ObjectID::Null:        return VariantType::NUL;
      case ObjectID::Boolean:     return VariantType::BOOLEAN;
      case ObjectID::SByte:       return VariantType::SBYTE;
      case ObjectID::Byte:        return VariantType::BYTE;
      case ObjectID::Int16:       return VariantType::INT16;
      case ObjectID::UInt16:      return VariantType::UINT16;

      case ObjectID::Integer:
      case ObjectID::Enumeration:
      case ObjectID::Int32:       return VariantType::INT32;

      case ObjectID::UInt32:      return VariantType::UINT32;
      case ObjectID::Int64:       return VariantType::INT64;
      case ObjectID::UInt64:      return VariantType::UINT64;
      case ObjectID::Float:       return VariantType::FLOAT;
      case ObjectID::Double:      return VariantType::DOUBLE;
      case ObjectID::String:      return VariantType::STRING;
      case ObjectID::ByteString:  return VariantType::BYTE_STRING;
      case ObjectID::DateTime:    return VariantType::DATE_TIME;
      case ObjectID::Guid:        return VariantType::GUID;

      default:
        std::stringstream stream;
        stream << "Cannot convert ObjectID '" << (unsigned)id << "' to VariantType.";
        throw std::logic_error(stream.str());
    }
  }

  inline ObjectID ConvertToObjectID(VariantType type)
  {
    switch (type)
    {
      case VariantType::NUL:         return ObjectID::Null;
      case VariantType::BOOLEAN:     return ObjectID::Boolean;
      case VariantType::SBYTE:       return ObjectID::SByte;
      case VariantType::BYTE:        return ObjectID::Byte;
      case VariantType::INT16:       return ObjectID::Int16;
      case VariantType::UINT16:      return ObjectID::UInt16;
      case VariantType::INT32:       return ObjectID::Int32;
      case VariantType::UINT32:      return ObjectID::UInt32;
      case VariantType::INT64:       return ObjectID::Int64;
      case VariantType::UINT64:      return ObjectID::UInt64;
      case VariantType::FLOAT:       return ObjectID::Float;
      case VariantType::DOUBLE:      return ObjectID::Double;
      case VariantType::STRING:      return ObjectID::String;
      case VariantType::BYTE_STRING: return ObjectID::ByteString;
      case VariantType::DATE_TIME:   return ObjectID::DateTime;
      case VariantType::GUID:        return ObjectID::Guid;

      default:
        std::stringstream stream;
        stream << "Cannot convert VariantType '"<< (unsigned)type << "' to ObjectID.";
        throw std::logic_error(stream.str());
    }
  }

  inline VariantType GetVariantType(xmlNode& node)
  {
    const ObjectID typeId = GetObjectIdOfType(GetProperty(node, "type"));
    return ConvertToVariantType(typeId);
  }

  NodeClass GetNodeClass(xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    if (nodeValue == "object")
    {
      return NodeClass::Object;
    }
    if (nodeValue == "variable")
    {
      return NodeClass::Variable;
    }
    if (nodeValue == "method")
    {
      return NodeClass::Method;
    }
    if (nodeValue == "object_type")
    {
      return NodeClass::ObjectType;
    }
    if (nodeValue == "variable_type")
    {
      return NodeClass::VariableType;
    }
    if (nodeValue == "reference_type")
    {
      return NodeClass::ReferenceType;
    }
    if (nodeValue == "data_type")
    {
      return NodeClass::DataType;
    }
    if (nodeValue == "data_type")
    {
      return NodeClass::View;
    }
    std::stringstream stream;
    stream << "Unknown node class '" << nodeValue << "'. Line " << node.line << ".";
    throw std::logic_error(stream.str());
  }

  bool IsNumericNodeType(xmlNode& node)
  {
    const std::string propValue = GetProperty(node, "type");
    return propValue == "numeric";
  }

  uint32_t GetNamespaceIndex(xmlNode& node)
  {
    const std::string propValue = GetProperty(node, "ns");
    return atoi(propValue.c_str());
  }

  NodeID GetNodeID(xmlNode& node)
  {
    std::string nodeValue = GetNodeValue(node);
    if (nodeValue.empty())
    {
      std::stringstream stream;
      stream << "Empty node id. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }
    uint32_t nsIndex = GetNamespaceIndex(node);
    if (IsNumericNodeType(node))
    {
      return NumericNodeID(atoi(nodeValue.c_str()), nsIndex);
    }
    return StringNodeID(nodeValue, nsIndex);
  }

  bool GetBool(std::string str)
  {
    if (!str.empty() && (str == "false" || str == "0"))
    {
      return false;
    }
    return !str.empty();
  }

  std::string GetText(xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    if (nodeValue.empty())
    {
      std::stringstream stream;
      stream << "Empty browse name. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }
    return nodeValue;
  }


  QualifiedName GetQualifiedName(xmlNode& node)
  {
    return QualifiedName(GetNamespaceIndex(node), GetText(node));
  }

  LocalizedText GetLocalizedText(xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    if (nodeValue.empty())
    {
      std::stringstream stream;
      stream << "Empty browse name. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }
    return LocalizedText(nodeValue);
  }

  uint32_t GetUInt32(xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    if (nodeValue.empty())
    {
      std::stringstream stream;
      stream << "Empty opcua attribute value. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }
    return atoi(nodeValue.c_str());
  }

  int32_t GetInt32(xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    if (nodeValue.empty())
    {
      std::stringstream stream;
      stream << "Empty opcua attribute value. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }
    return atoi(nodeValue.c_str());
  }


  bool GetBool(xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    return GetBool(nodeValue);
  }

  Variant GetVariantValue(OpcUa::VariantType type, xmlNode& node)
  {
    const std::string nodeValue = GetNodeValue(node);
    switch (type)
    {
      case VariantType::SBYTE:
        return Variant((int8_t)strtol(nodeValue.c_str(), nullptr, 0));
      case VariantType::BYTE:
        return Variant((uint8_t)strtol(nodeValue.c_str(), nullptr, 0));
      case VariantType::INT16:
        return Variant((int16_t)strtol(nodeValue.c_str(), nullptr, 0));
      case VariantType::UINT16:
        return Variant((uint16_t)strtol(nodeValue.c_str(), nullptr, 0));
      case VariantType::INT32:
        return Variant((int32_t)strtol(nodeValue.c_str(), nullptr, 0));
      case VariantType::UINT32:
        return Variant((uint32_t)strtol(nodeValue.c_str(), nullptr, 0));
      case VariantType::INT64:
        return Variant((int64_t)strtoll(nodeValue.c_str(), nullptr, 0));
      case VariantType::UINT64:
        return Variant((int64_t)strtoll(nodeValue.c_str(), nullptr, 0));
      case VariantType::FLOAT:
        return Variant(strtof(nodeValue.c_str(), nullptr));
      case VariantType::DOUBLE:
        return Variant(strtod(nodeValue.c_str(), nullptr));
      // TODO check for other types.
      case VariantType::NUL:
        return Variant();

      case VariantType::STRING:
      default:
        break;
    }
    return Variant(nodeValue);
  }

  OpcUa::AttributeID GetAttributeID(xmlNode& node)
  {
    if (IsXmlNode(node, "id"))
      return AttributeID::NodeId;
    else if (IsXmlNode(node, "class"))
      return AttributeID::NodeClass;
    else if (IsXmlNode(node, "browse_name"))
      return AttributeID::BrowseName;
    else if (IsXmlNode(node, "display_name"))
      return AttributeID::DisplayName;
    else if (IsXmlNode(node, "description"))
      return AttributeID::Description;
    else if (IsXmlNode(node, "write_mask"))
      return AttributeID::WriteMask;
    else if (IsXmlNode(node, "user_write_mask"))
      return AttributeID::UserWriteMask;
    else if (IsXmlNode(node, "is_abstract"))
      return AttributeID::IsAbstract;
    else if (IsXmlNode(node, "symmetric"))
      return AttributeID::Symmetric;
    else if (IsXmlNode(node, "inverse_name"))
      return AttributeID::InverseName;
    else if (IsXmlNode(node, "contains_no_loops"))
      return AttributeID::ContainsNoLoops;
    else if (IsXmlNode(node, "event_notifier"))
      return AttributeID::EventNotifier;
    else if (IsXmlNode(node, "value"))
      return AttributeID::Value;
    else if (IsXmlNode(node, "value_rank"))
      return AttributeID::ValueRank;
    else if (IsXmlNode(node, "data_type"))
      return AttributeID::DataType;
    else if (IsXmlNode(node, "array_dimensions"))
      return AttributeID::ArrayDimensions;
    else if (IsXmlNode(node, "access_level"))
      return AttributeID::AccessLevel;
    else if (IsXmlNode(node, "user_access_level"))
      return AttributeID::UserAccessLevel;
    else if (IsXmlNode(node, "minimum_sampling_interval"))
      return AttributeID::MinimumSamplingInterval;
    else if (IsXmlNode(node, "historizing"))
      return AttributeID::Historizing;
    else if (IsXmlNode(node, "executable"))
      return AttributeID::Executable;
    else if (IsXmlNode(node, "user_executable"))
      return AttributeID::UserExecutable;

    return AttributeID::Unknown;
  }

  Variant GetAttributeValue(OpcUa::AttributeID id, xmlNode& node)
  {
    switch (id)
    {
      case AttributeID::NodeId:
        return Variant(GetNodeID(node));

      case AttributeID::NodeClass:
        return Variant((int32_t)GetNodeClass(node));

      case AttributeID::DisplayName:
        return Variant(GetLocalizedText(node));

      case AttributeID::BrowseName:
        return Variant(GetQualifiedName(node));

      case AttributeID::Description:
      case AttributeID::InverseName:
        return Variant(GetText(node));

      case AttributeID::EventNotifier:
        return Variant(std::vector<uint8_t>{0});

      case AttributeID::ValueRank:
        return Variant(GetInt32(node));

      case AttributeID::WriteMask:
      case AttributeID::UserWriteMask:
      case AttributeID::ArrayDimensions:
      case AttributeID::AccessLevel:
      case AttributeID::UserAccessLevel:
      case AttributeID::MinimumSamplingInterval:
        return Variant(GetUInt32(node));

      case AttributeID::IsAbstract:
      case AttributeID::Symmetric:
      case AttributeID::ContainsNoLoops:
      case AttributeID::Historizing:
      case AttributeID::Executable:
      case AttributeID::UserExecutable:
        return Variant(GetBool(node));

      case AttributeID::Value:
        break;

      case AttributeID::DataType:
        return Variant(GetObjectIdOfType(node));

      default:
        return Variant(GetText(node));
    }
    const VariantType type = GetVariantType(node);
    return GetVariantValue(type, node);
  }

  class AttributesCollector : private Internal::XmlProcessor
  {
  public:
    AttributesCollector(INode& node, bool debug)
      : OpcUaNode(node)
      , Debug(debug)
    {
    }

    virtual void Process(xmlNode& node)
    {
      for (xmlNodePtr subNode = node.children; subNode; subNode = subNode->next)
      {
        if (!IsXmlNode(*subNode))
        {
          continue;
        }
        const AttributeID attribute = GetAttributeID(*subNode);
        if (attribute == AttributeID::NodeId)
        {
          OpcUaNode.ID = GetNodeID(*subNode);
          continue;
        }

        const Variant value = GetAttributeValue(attribute, *subNode);
        AddAttribute(attribute, value);
      }

      // If tag 'data_type' is absent in the xml then need to add data type which will be based on type of value.
      if (!HasAttribute(AttributeID::DataType) && HasAttribute(AttributeID::Value))
      {
        AddAttribute(AttributeID::DataType, GetDataType(AttributeID::Value));
      }
    }

  private:
    bool IsAttributes(const xmlNode& node) const
    {
      return IsXmlNode(node, "attributes");
    }

    template <typename T>
    void AddAttribute(AttributeID attr, const T& value)
    {
      OpcUaNode.Attributes.insert(std::make_pair(attr, Variant(value)));
    }

    bool HasAttribute(AttributeID attr) const
    {
      return OpcUaNode.Attributes.find(AttributeID::DataType) != OpcUaNode.Attributes.end();
    }

    ObjectID GetDataType(AttributeID attr) const
    {
      auto attrPos = OpcUaNode.Attributes.find(attr);
      if (attrPos == OpcUaNode.Attributes.end())
      {
        return ObjectID::Null;
      }
      return ConvertToObjectID(attrPos->second.Type());
    }

  private:
    INode& OpcUaNode;
    const bool Debug;
  };

  class ReferencesCollector : private Internal::XmlProcessor
  {
  public:
    ReferencesCollector(INode& node, bool debug)
      : OpcUaNode(node)
      , Debug(debug)
    {
    }

    virtual void Process(xmlNode& node)
    {
      for (xmlNodePtr refNode = node.children; refNode; refNode = refNode->next)
      {
        if (!IsXmlNode(*refNode))
        {
          continue;
        }

        try
        {
          AddReferenceToNode(*refNode);
        }
        catch (const std::exception& exc)
        {
          std::cerr << exc.what() << std::endl;
        }
      }
    }

  private:
    void AddReferenceToNode(xmlNode& refNode)
    {
      const std::string refName = GetNodeName(refNode);
      const NodeID targetNode = GetNodeID(refNode);
      Reference reference;
      reference.ID = GetReferenceID(refName);

      for (xmlNodePtr subNode = refNode.children; subNode; subNode = subNode->next)
      {
        if (!IsXmlNode(*subNode))
        {
          continue;
        }

        const std::string& nodeName = GetNodeName(*subNode);
        if (nodeName == "id")
        {
          reference.TargetNode = GetNodeID(*subNode);
        }
        else if (nodeName == "class")
        {
          reference.TargetClass = GetNodeClass(*subNode);
        }
        else if (nodeName == "browse_name")
        {
          reference.TargetBrowseName = GetQualifiedName(*subNode);
        }
        else if (nodeName == "display_name")
        {
          reference.TargetDisplayName = GetLocalizedText(*subNode);
        }
        else if (nodeName == "is_forward")
        {
          reference.IsForward = GetBool(GetNodeValue(*subNode));
        }
        else if (nodeName == "type_definition")
        {
          reference.TargetType = GetNodeID(*subNode);
        }
      }

      EnsureValid(reference, refNode.line);
      OpcUaNode.References.push_back(reference);
    }

  private:
    void EnsureValid(const Reference& ref, int lineNum) const
    {
      std::stringstream stream;
      if (ref.ID == ReferenceID::Unknown)
      {
        stream << "Unknown reference type. line" << lineNum << ".";
        throw std::logic_error(stream.str());
      }
      if (ref.TargetNode == NodeID())
      {
        stream << "Empty target node ID. line" << lineNum << ".";
        throw std::logic_error(stream.str());
      }
    }

  private:
    INode& OpcUaNode;
    const bool Debug;
  };

  class NodesCollector : private Internal::XmlProcessor
  {
  public:
    NodesCollector(std::map<NodeID, INode>& nodes, bool debug)
      : Nodes(nodes)
      , Debug(debug)
    {
    }

    virtual void Process(xmlNode& node)
    {
      if (!IsXmlNode(node))
      {
        return;
      }

      INode opcuaNode;
      if (IsXmlNode(node, "node"))
      {
        opcuaNode.IsExternal = false;
      }
      else if (IsXmlNode(node, "external"))
      {
        opcuaNode.IsExternal = true;
      }
      else
      {
        if (Debug)
        {
          std::cerr << "Unknown node '" << node.name << "' at line " << node.line <<  "." << std::endl;
        }
        return;
      }

      FillNode(node, opcuaNode);
      EnsureNodeIsValid(opcuaNode, node);
      Nodes.insert(std::make_pair(opcuaNode.ID, opcuaNode));
    }

  private:
    void EnsureNodeIsValid(const INode& opcuaNode, const xmlNode& node) const
    {
      if (opcuaNode.ID == NodeID())
      {
        std::stringstream stream;
        stream << "INode at line '" << node.line << "' has no ID.";
        throw std::logic_error(stream.str());
      }
    }

    void FillNode(const xmlNode& node, INode& opcuaNode) const
    {
      AttributesCollector attributeCollector(opcuaNode, Debug);
      ReferencesCollector referencCollector(opcuaNode, Debug);

      for (xmlNodePtr subNode = node.children; subNode; subNode = subNode->next)
      {
        if (IsXmlNode(*subNode, "attributes"))
        {
          attributeCollector.Process(*subNode);
          continue;
        }
        else if (IsXmlNode(*subNode, "references"))
        {
          referencCollector.Process(*subNode);
        }
        else if (Debug)
        {
          std::cerr << "Unknown node '" << subNode->name << "' at line " << subNode->line <<  "." << std::endl;
        }
      }
    }

  private:
    std::map<NodeID, INode>& Nodes;
    const bool Debug;
  };


  class ConfigurationProcessor
  {
  public:
    ConfigurationProcessor(bool debug)
      : Debug(debug)
    {

    }

    std::map<NodeID, INode> Process(xmlDoc& doc)
    {
      xmlNodePtr rootNode = xmlDocGetRootElement(&doc);
      EnsureRootNodeValid(*rootNode);

      std::map<NodeID, INode> nodes;
      NodesCollector nodesBuilder(nodes, Debug);
      for (xmlNodePtr cur = rootNode->children; cur; cur = cur->next)
      {
        nodesBuilder.Process(*cur);
      }

      return nodes;
    }

  private:
    void EnsureRootNodeValid(xmlNode& rootNode)
    {
      if (rootNode.type != XML_ELEMENT_NODE)
      {
        throw std::logic_error("Root element is not a node.'");
      }
      if (xmlStrcmp(rootNode.name, "address_space"))
      {
        throw std::logic_error(std::string("Invalid root element '") + (const char*)rootNode.name + std::string("'."));
      }
      std::unique_ptr<xmlChar, LibXmlFree> versionBuf(xmlGetProp(&rootNode, (const xmlChar*)"version"), LibXmlFree());
      const xmlChar* version = versionBuf.get();
      if (!version)
      {
        throw std::logic_error("Address space element has no 'version' attribute.");
      }
      if (xmlStrcmp(version, "1"))
      {
        throw std::logic_error(std::string("Unknown version '") + (const char*)version + std::string("'of address space."));
      }
    }

  private:
    const bool Debug;
  };

  std::map<NodeID, INode> ParseConfig(const char* configPath, bool debug)
  {
    std::unique_ptr<xmlDoc, XmlDocDeleter> doc(xmlParseFile(configPath), XmlDocDeleter());
    if (!doc)
    {
      throw std::logic_error(std::string("Cannot load file '") + std::string(configPath) + std::string("'"));
    }
    ConfigurationProcessor xmlConfiguration(debug);
    return xmlConfiguration.Process(*doc);
  }


  class NodesRegistrator
  {
  public:
    NodesRegistrator(OpcUa::NodeManagementServices& registry, bool debug)
      : Registry(registry)
      , Debug(debug)
    {
    }

    void RegisterNodes(const std::map<NodeID, INode>& nodes)
    {
      for (const auto& node : nodes)
      {
        if (!node.second.IsExternal)
        {
          RegisterNode(node.second);
        }
        AddReferences(node.second);
      }
    }

  private:
    void RegisterNode(const INode& node)
    {
      //Registry.AddAttribute(node.ID, AttributeID::NodeId, Variant(node.ID));
      for (const std::pair<AttributeID, Variant>& attr : node.Attributes)
      {
        //Registry.AddAttribute(node.ID, attr.first, attr.second);
      }
    }

    void AddReferences(const INode& node)
    {
      for (const Reference& ref : node.References)
      {
        ReferenceDescription desc;
        desc.BrowseName = ref.TargetBrowseName;
        desc.DisplayName = ref.TargetDisplayName;
        desc.IsForward = ref.IsForward;
        desc.ReferenceTypeID = ref.ID;
        desc.TargetNodeClass = ref.TargetClass;
        desc.TargetNodeID = ref.TargetNode;
        desc.TargetNodeTypeDefinition = ref.TargetType;
        //Registry.AddReference(node.ID, desc);
      }
    }

  private:
    OpcUa::NodeManagementServices& Registry;
    const bool Debug;
  };
} // namespace

namespace OpcUa
{
  namespace Internal
  {

    XmlAddressSpaceLoader::XmlAddressSpaceLoader(OpcUa::NodeManagementServices& registry, bool debug)
      : Registry(registry)
      , Debug(debug)
    {
    }

    void XmlAddressSpaceLoader::Load(const char* fileName)
    {
      std::map<NodeID, INode> nodes = ParseConfig(fileName, Debug);
      NodesRegistrator reg(Registry, Debug);
      reg.RegisterNodes(nodes);
    }

  } // namespace Internal
} // namespace OpcUa
