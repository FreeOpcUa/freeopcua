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

  ReferenceId GetReferenceId(const std::string& referenceName)
  {
    if (referenceName == "organizes")
      return ReferenceId::Organizes;
    if (referenceName == "references_to")
      return ReferenceId::References;
    if (referenceName == "has_child")
      return ReferenceId::HasChild;
    if (referenceName == "has_event_source")
      return ReferenceId::HasEventSource;
    if (referenceName == "has_modelling_rule")
      return ReferenceId::HasModellingRule;
    if (referenceName == "has_encoding")
      return ReferenceId::HasEncoding;
    if (referenceName == "has_description")
      return ReferenceId::HasDescription;
    if (referenceName == "has_type_definition")
      return ReferenceId::HasTypeDefinition;
    if (referenceName == "generates_event")
      return ReferenceId::GeneratesEvent;
    if (referenceName == "aggregates")
      return ReferenceId::Aggregates;
    if (referenceName == "has_subtype")
      return ReferenceId::HasSubtype;
    if (referenceName == "has_property")
      return ReferenceId::HasProperty;
    if (referenceName == "has_component")
      return ReferenceId::HasComponent;
    if (referenceName == "has_notifier")
      return ReferenceId::HasNotifier;
    if (referenceName == "has_ordered_component")
      return ReferenceId::HasOrderedComponent;
    if (referenceName == "has_model_parent")
      return ReferenceId::HasModelParent;
    if (referenceName == "from_state")
      return ReferenceId::FromState;
    if (referenceName == "to_state")
      return ReferenceId::ToState;
    if (referenceName == "has_clause")
      return ReferenceId::HasCause;
    if (referenceName == "has_effect")
      return ReferenceId::HasEffect;
    if (referenceName == "has_historical_configuration")
      return ReferenceId::HasHistoricalConfiguration;
    if (referenceName == "has_historical_event_configuration")
      return ReferenceId::HasHistoricalEventConfiguration;
    if (referenceName == "has_substate_machine")
      return ReferenceId::HasSubStateMachine;
    if (referenceName == "has_event_history")
      return ReferenceId::HasEventHistory;
    if (referenceName == "always_generates_event")
      return ReferenceId::AlwaysGeneratesEvent;
    if (referenceName == "has_true_substate")
      return ReferenceId::HasTrueSubState;
    if (referenceName == "has_false_substate")
      return ReferenceId::HasFalseSubState;
    if (referenceName == "has_condition")
      return ReferenceId::HasCondition;
    if (referenceName == "non_hierarchical_references")
      return ReferenceId::NonHierarchicalReferences;
    if (referenceName == "hierarchical_references")
      return ReferenceId::HierarchicalReferences;
    if (referenceName == "has_cause")
      return ReferenceId::HasCause;
    if (referenceName == "has_sub_state_machine")
      return ReferenceId::HasSubStateMachine;
    if (referenceName == "has_true_sub_state")
      return ReferenceId::HasTrueSubState;
    if (referenceName == "has_false_sub_state")
      return ReferenceId::HasFalseSubState;

    throw std::logic_error(std::string("Unknown reference name '") + referenceName + std::string(referenceName));
  }

  struct Reference
  {
    ReferenceId Id;
    NodeId TargetNode;
    bool IsForward;
    NodeClass TargetClass;
    QualifiedName TargetBrowseName;
    LocalizedText TargetDisplayName;
    NodeId TargetType;

    Reference()
      : Id(ReferenceId::Unknown)
      , IsForward(true)
      , TargetClass(NodeClass::Unspecified)
    {
    }
  };

  struct Attribute
  {
    AttributeId Id;
    Variant Value;
    Attribute()
      : Id(AttributeId::Unknown)
    {
    }

    Attribute(AttributeId id, Variant value)
      : Id(id)
      , Value(value)
    {
    }
  };

  struct INode
  {
    NodeId Id;
    std::map<AttributeId, Variant> Attributes;
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

  ObjectId GetObjectIdOfType(const std::string& nodeValue)
  {
    if (nodeValue == "bool")
    {
      return ObjectId::Boolean;
    }
    if (nodeValue == "sbyte")
    {
      return ObjectId::SByte;
    }
    if (nodeValue == "byte")
    {
      return ObjectId::Byte;
    }
    if (nodeValue == "int16")
    {
      return ObjectId::Int16;
    }
    if (nodeValue == "uint16")
    {
      return ObjectId::UInt16;
    }
    if (nodeValue == "int32")
    {
      return ObjectId::Int32;
    }
    if (nodeValue == "uint32")
    {
      return ObjectId::UInt32;
    }
    if (nodeValue == "string")
    {
      return ObjectId::String;
    }
    if (nodeValue == "enum")
    {
      return ObjectId::Enumeration;
    }
    if (nodeValue == "int")
    {
      return ObjectId::Integer;
    }
    if (nodeValue == "byte_string")
    {
      return ObjectId::ByteString;
    }
    if (nodeValue == "guid")
    {
      return ObjectId::Guid;
    }
    if (nodeValue == "date_time")
    {
      return ObjectId::DateTime;
    }

    std::stringstream stream;
    stream << "Unknown data type '" << nodeValue << "'.";
    throw std::logic_error(stream.str());
  }

  inline ObjectId GetObjectIdOfType(xmlNode& node)
  {
    return GetObjectIdOfType(GetNodeValue(node));
  }

  inline VariantType ConvertToVariantType(ObjectId id)
  {
    switch (id)
    {
      case ObjectId::Null:        return VariantType::NUL;
      case ObjectId::Boolean:     return VariantType::BOOLEAN;
      case ObjectId::SByte:       return VariantType::SBYTE;
      case ObjectId::Byte:        return VariantType::BYTE;
      case ObjectId::Int16:       return VariantType::INT16;
      case ObjectId::UInt16:      return VariantType::UINT16;

      case ObjectId::Integer:
      case ObjectId::Enumeration:
      case ObjectId::Int32:       return VariantType::INT32;

      case ObjectId::UInt32:      return VariantType::UINT32;
      case ObjectId::Int64:       return VariantType::INT64;
      case ObjectId::UInt64:      return VariantType::UINT64;
      case ObjectId::Float:       return VariantType::FLOAT;
      case ObjectId::Double:      return VariantType::DOUBLE;
      case ObjectId::String:      return VariantType::STRING;
      case ObjectId::ByteString:  return VariantType::BYTE_STRING;
      case ObjectId::DateTime:    return VariantType::DATE_TIME;
      case ObjectId::Guid:        return VariantType::GUId;

      default:
        std::stringstream stream;
        stream << "Cannot convert ObjectId '" << (unsigned)id << "' to VariantType.";
        throw std::logic_error(stream.str());
    }
  }

  inline ObjectId ConvertToObjectId(VariantType type)
  {
    switch (type)
    {
      case VariantType::NUL:         return ObjectId::Null;
      case VariantType::BOOLEAN:     return ObjectId::Boolean;
      case VariantType::SBYTE:       return ObjectId::SByte;
      case VariantType::BYTE:        return ObjectId::Byte;
      case VariantType::INT16:       return ObjectId::Int16;
      case VariantType::UINT16:      return ObjectId::UInt16;
      case VariantType::INT32:       return ObjectId::Int32;
      case VariantType::UINT32:      return ObjectId::UInt32;
      case VariantType::INT64:       return ObjectId::Int64;
      case VariantType::UINT64:      return ObjectId::UInt64;
      case VariantType::FLOAT:       return ObjectId::Float;
      case VariantType::DOUBLE:      return ObjectId::Double;
      case VariantType::STRING:      return ObjectId::String;
      case VariantType::BYTE_STRING: return ObjectId::ByteString;
      case VariantType::DATE_TIME:   return ObjectId::DateTime;
      case VariantType::GUId:        return ObjectId::Guid;

      default:
        std::stringstream stream;
        stream << "Cannot convert VariantType '"<< (unsigned)type << "' to ObjectId.";
        throw std::logic_error(stream.str());
    }
  }

  inline VariantType GetVariantType(xmlNode& node)
  {
    const ObjectId typeId = GetObjectIdOfType(GetProperty(node, "type"));
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

  NodeId GetNodeId(xmlNode& node)
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
      return NumericNodeId(atoi(nodeValue.c_str()), nsIndex);
    }
    return StringNodeId(nodeValue, nsIndex);
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

  OpcUa::AttributeId GetAttributeId(xmlNode& node)
  {
    if (IsXmlNode(node, "id"))
      return AttributeId::NodeId;
    else if (IsXmlNode(node, "class"))
      return AttributeId::NodeClass;
    else if (IsXmlNode(node, "browse_name"))
      return AttributeId::BrowseName;
    else if (IsXmlNode(node, "display_name"))
      return AttributeId::DisplayName;
    else if (IsXmlNode(node, "description"))
      return AttributeId::Description;
    else if (IsXmlNode(node, "write_mask"))
      return AttributeId::WriteMask;
    else if (IsXmlNode(node, "user_write_mask"))
      return AttributeId::UserWriteMask;
    else if (IsXmlNode(node, "is_abstract"))
      return AttributeId::IsAbstract;
    else if (IsXmlNode(node, "symmetric"))
      return AttributeId::Symmetric;
    else if (IsXmlNode(node, "inverse_name"))
      return AttributeId::InverseName;
    else if (IsXmlNode(node, "contains_no_loops"))
      return AttributeId::ContainsNoLoops;
    else if (IsXmlNode(node, "event_notifier"))
      return AttributeId::EventNotifier;
    else if (IsXmlNode(node, "value"))
      return AttributeId::Value;
    else if (IsXmlNode(node, "value_rank"))
      return AttributeId::ValueRank;
    else if (IsXmlNode(node, "data_type"))
      return AttributeId::DataType;
    else if (IsXmlNode(node, "array_dimensions"))
      return AttributeId::ArrayDimensions;
    else if (IsXmlNode(node, "access_level"))
      return AttributeId::AccessLevel;
    else if (IsXmlNode(node, "user_access_level"))
      return AttributeId::UserAccessLevel;
    else if (IsXmlNode(node, "minimum_sampling_interval"))
      return AttributeId::MinimumSamplingInterval;
    else if (IsXmlNode(node, "historizing"))
      return AttributeId::Historizing;
    else if (IsXmlNode(node, "executable"))
      return AttributeId::Executable;
    else if (IsXmlNode(node, "user_executable"))
      return AttributeId::UserExecutable;

    return AttributeId::Unknown;
  }

  Variant GetAttributeValue(OpcUa::AttributeId id, xmlNode& node)
  {
    switch (id)
    {
      case AttributeId::NodeId:
        return Variant(GetNodeId(node));

      case AttributeId::NodeClass:
        return Variant((int32_t)GetNodeClass(node));

      case AttributeId::DisplayName:
        return Variant(GetLocalizedText(node));

      case AttributeId::BrowseName:
        return Variant(GetQualifiedName(node));

      case AttributeId::Description:
      case AttributeId::InverseName:
        return Variant(GetText(node));

      case AttributeId::EventNotifier:
        return Variant(std::vector<uint8_t>{0});

      case AttributeId::ValueRank:
        return Variant(GetInt32(node));

      case AttributeId::WriteMask:
      case AttributeId::UserWriteMask:
      case AttributeId::ArrayDimensions:
      case AttributeId::AccessLevel:
      case AttributeId::UserAccessLevel:
      case AttributeId::MinimumSamplingInterval:
        return Variant(GetUInt32(node));

      case AttributeId::IsAbstract:
      case AttributeId::Symmetric:
      case AttributeId::ContainsNoLoops:
      case AttributeId::Historizing:
      case AttributeId::Executable:
      case AttributeId::UserExecutable:
        return Variant(GetBool(node));

      case AttributeId::Value:
        break;

      case AttributeId::DataType:
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
        const AttributeId attribute = GetAttributeId(*subNode);
        if (attribute == AttributeId::NodeId)
        {
          OpcUaNode.Id = GetNodeId(*subNode);
          continue;
        }

        const Variant value = GetAttributeValue(attribute, *subNode);
        AddAttribute(attribute, value);
      }

      // If tag 'data_type' is absent in the xml then need to add data type which will be based on type of value.
      if (!HasAttribute(AttributeId::DataType) && HasAttribute(AttributeId::Value))
      {
        AddAttribute(AttributeId::DataType, GetDataType(AttributeId::Value));
      }
    }

  private:
    bool IsAttributes(const xmlNode& node) const
    {
      return IsXmlNode(node, "attributes");
    }

    template <typename T>
    void AddAttribute(AttributeId attr, const T& value)
    {
      OpcUaNode.Attributes.insert(std::make_pair(attr, Variant(value)));
    }

    bool HasAttribute(AttributeId attr) const
    {
      return OpcUaNode.Attributes.find(AttributeId::DataType) != OpcUaNode.Attributes.end();
    }

    ObjectId GetDataType(AttributeId attr) const
    {
      auto attrPos = OpcUaNode.Attributes.find(attr);
      if (attrPos == OpcUaNode.Attributes.end())
      {
        return ObjectId::Null;
      }
      return ConvertToObjectId(attrPos->second.Type());
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
      const NodeId targetNode = GetNodeId(refNode);
      Reference reference;
      reference.Id = GetReferenceId(refName);

      for (xmlNodePtr subNode = refNode.children; subNode; subNode = subNode->next)
      {
        if (!IsXmlNode(*subNode))
        {
          continue;
        }

        const std::string& nodeName = GetNodeName(*subNode);
        if (nodeName == "id")
        {
          reference.TargetNode = GetNodeId(*subNode);
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
          reference.TargetType = GetNodeId(*subNode);
        }
      }

      EnsureValid(reference, refNode.line);
      OpcUaNode.References.push_back(reference);
    }

  private:
    void EnsureValid(const Reference& ref, int lineNum) const
    {
      std::stringstream stream;
      if (ref.Id == ReferenceId::Unknown)
      {
        stream << "Unknown reference type. line" << lineNum << ".";
        throw std::logic_error(stream.str());
      }
      if (ref.TargetNode == NodeId())
      {
        stream << "Empty target node Id. line" << lineNum << ".";
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
    NodesCollector(std::map<NodeId, INode>& nodes, bool debug)
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
      Nodes.insert(std::make_pair(opcuaNode.Id, opcuaNode));
    }

  private:
    void EnsureNodeIsValid(const INode& opcuaNode, const xmlNode& node) const
    {
      if (opcuaNode.Id == NodeId())
      {
        std::stringstream stream;
        stream << "INode at line '" << node.line << "' has no Id.";
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
    std::map<NodeId, INode>& Nodes;
    const bool Debug;
  };


  class ConfigurationProcessor
  {
  public:
    ConfigurationProcessor(bool debug)
      : Debug(debug)
    {

    }

    std::map<NodeId, INode> Process(xmlDoc& doc)
    {
      xmlNodePtr rootNode = xmlDocGetRootElement(&doc);
      EnsureRootNodeValid(*rootNode);

      std::map<NodeId, INode> nodes;
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

  std::map<NodeId, INode> ParseConfig(const char* configPath, bool debug)
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

    void RegisterNodes(const std::map<NodeId, INode>& nodes)
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
      //Registry.AddAttribute(node.Id, AttributeId::NodeId, Variant(node.Id));
      for (const std::pair<AttributeId, Variant>& attr : node.Attributes)
      {
        //Registry.AddAttribute(node.Id, attr.first, attr.second);
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
        desc.ReferenceTypeId = ref.Id;
        desc.TargetNodeClass = ref.TargetClass;
        desc.TargetNodeId = ref.TargetNode;
        desc.TargetNodeTypeDefinition = ref.TargetType;
        //Registry.AddReference(node.Id, desc);
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
      std::map<NodeId, INode> nodes = ParseConfig(fileName, Debug);
      NodesRegistrator reg(Registry, Debug);
      reg.RegisterNodes(nodes);
    }

  } // namespace Internal
} // namespace OpcUa
