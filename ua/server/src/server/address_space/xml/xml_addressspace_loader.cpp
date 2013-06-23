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

#include "xml_addressspace_loader.h"

#include <opc/ua/server/addons/address_space_registry.h>

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

  ReferenceID GetReferenceID(const char* referenceName)
  {
    if (!strcmp(referenceName, "organizes"))
      return ReferenceID::Organizes;
    if (!strcmp(referenceName, "has_child"))
      return ReferenceID::HasChild;
    if (!strcmp(referenceName, "organizes"))
      return ReferenceID::Organizes;
    if (!strcmp(referenceName, "has_event_source"))
      return ReferenceID::HasEventSource;
    if (!strcmp(referenceName, "has_modelling_rule"))
      return ReferenceID::HasModellingRule;
    if (!strcmp(referenceName, "has_encoding"))
      return ReferenceID::HasEncoding;
    if (!strcmp(referenceName, "has_description"))
      return ReferenceID::HasDescription;
    if (!strcmp(referenceName, "has_type_definition"))
      return ReferenceID::HasTypeDefinition;
    if (!strcmp(referenceName, "generates_event"))
      return ReferenceID::GeneratesEvent;
    if (!strcmp(referenceName, "aggregates"))
      return ReferenceID::Aggregates;
    if (!strcmp(referenceName, "has_subtype"))
      return ReferenceID::HasSubtype;
    if (!strcmp(referenceName, "has_property"))
      return ReferenceID::HasProperty;
    if (!strcmp(referenceName, "has_component"))
      return ReferenceID::HasComponent;
    if (!strcmp(referenceName, "has_notifier"))
      return ReferenceID::HasNotifier;
    if (!strcmp(referenceName, "has_ordered_component"))
      return ReferenceID::HasOrderedComponent;
    if (!strcmp(referenceName, "has_model_parent"))
      return ReferenceID::HasModelParent;
    if (!strcmp(referenceName, "from_state"))
      return ReferenceID::FromState;
    if (!strcmp(referenceName, "to_state"))
      return ReferenceID::ToState;
    if (!strcmp(referenceName, "has_clause"))
      return ReferenceID::HasCause;
    if (!strcmp(referenceName, "has_effect"))
      return ReferenceID::HasEffect;
    if (!strcmp(referenceName, "has_historical_configuration"))
      return ReferenceID::HasHistoricalConfiguration;
    if (!strcmp(referenceName, "has_historical_event_configuration"))
      return ReferenceID::HasHistoricalEventConfiguration;
    if (!strcmp(referenceName, "has_substate_machine"))
      return ReferenceID::HasSubStateMachine;
    if (!strcmp(referenceName, "has_event_history"))
      return ReferenceID::HasEventHistory;
    if (!strcmp(referenceName, "always_generates_event"))
      return ReferenceID::AlwaysGeneratesEvent;
    if (!strcmp(referenceName, "has_true_substate"))
      return ReferenceID::HasTrueSubState;
    if (!strcmp(referenceName, "has_false_substate"))
      return ReferenceID::HasFalseSubState;
    if (!strcmp(referenceName, "has_condition"))
      return ReferenceID::HasCondition;

    throw std::logic_error(std::string("Unknown reference name '") + referenceName + std::string(referenceName));
  }

  struct Reference
  {
    std::string Name;
    NodeID TargetNode;
  };

  struct Attribute
  {
    AttributeID ID;
    Variant Value;
    Attribute(AttributeID id, Variant value)
      : ID(id)
      , Value(value)
    {
    }
  };

  struct Node
  {
    NodeID ID;
    std::map<AttributeID, Variant> Attributes;
    std::vector<Reference> References;
  };

  class XmlProcessor : private Common::Interface
  {
  public:
    virtual void Process(const xmlNode& node) = 0;
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


  class AttributesCollector : private XmlProcessor
  {
  public:
    AttributesCollector(Node& node, bool debug)
      : OpcUaNode(node)
      , Debug(debug)
    {
    }

    virtual void Process(const xmlNode& node)
    {
      if (!IsAttributes(node))
      {
        return;
      }

      for (xmlNodePtr subNode = node.children; subNode; subNode = subNode->next)
      {
        if (!IsXmlNode(*subNode))
        {
          continue;
        }
        const AttributeID attribute = GetAttributeID(*subNode);
        if (attribute == AttributeID::NODE_ID)
        {
          OpcUaNode.ID = GetNodeID(*subNode);
          continue;
        }

        const Variant value = GetAttributeValue(attribute, *subNode);
        OpcUaNode.Attributes.insert(std::make_pair(attribute, value));
        if (attribute == AttributeID::VALUE)
        {
          OpcUaNode.Attributes.insert(std::make_pair(AttributeID::DATA_TYPE, Variant((uint32_t)value.Type)));
        }
      }
    }

  private:
    bool IsAttributes(const xmlNode& node) const
    {
      return IsXmlNode(node, "attributes");
    }

    NodeID GetNodeID(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty node id. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }
      uint32_t nsIndex = GetNamespaceIndex(node);

      const char* idText = (const char*)content.get();
      if (IsNumericNodeType(node))
      {
        return NumericNodeID(atoi(idText), nsIndex);
      }
      return StringNodeID(idText, nsIndex);
    }

    uint32_t GetUInt32(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty opcua attribute. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }
      return atoi((const char*)content.get());
    }

    bool GetBool(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty opcua attribute. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }

      const char* value = (const char*)content.get();
      if (!strcmp(value, "1") || !strcmp(value, "true"))
      {
        return true;
      }
      if (!strcmp(value, "0") || !strcmp(value, "false"))
      {
        return false;
      }

      std::stringstream stream;
      stream << "Empty boolean value of opcua attribute. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }

    NodeClass GetNodeClass(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      const xmlChar* xmlString = content.get();
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty node class. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }
      if (!xmlStrcmp(xmlString, "object"))
      {
        return NodeClass::Object;
      }
      if (!xmlStrcmp(xmlString, "variable"))
      {
        return NodeClass::Variable;
      }
      if (!xmlStrcmp(xmlString, "method"))
      {
        return NodeClass::Method;
      }
      if (!xmlStrcmp(xmlString, "object_type"))
      {
        return NodeClass::ObjectType;
      }
      if (!xmlStrcmp(xmlString, "variable_type"))
      {
        return NodeClass::VariableType;
      }
      if (!xmlStrcmp(xmlString, "reference_type"))
      {
        return NodeClass::ReferenceType;
      }
      if (!xmlStrcmp(xmlString, "data_type"))
      {
        return NodeClass::DataType;
      }
      if (!xmlStrcmp(xmlString, "data_type"))
      {
        return NodeClass::View;
      }
      std::stringstream stream;
      stream << "Unknown node class '" << (const char*)xmlString << "'. Line " << node.line << ".";
      throw std::logic_error(stream.str());
    }

    std::string GetText(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty browse name. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }
      return std::string((const char*)content.get());
    }

    LocalizedText GetLocalizedText(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty browse name. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }
      return LocalizedText((const char*)content.get());
    }

    uint32_t GetNamespaceIndex(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> attrValue(xmlGetProp(&node, (const xmlChar*)"ns"), LibXmlFree());
      const xmlChar* nsName = attrValue.get();
      if (!nsName)
        return 0;
      return atoi((const char*)nsName);
    }

    bool IsNumericNodeType(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> attrValue(xmlGetProp(&node, (const xmlChar*)"type"), LibXmlFree());
      const xmlChar* nsName = attrValue.get();
      if (!nsName)
        return 0;

      return !xmlStrcmp(nsName, "numeric");
    }

    VariantType GetVariantType(xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> attrValue(xmlGetProp(&node, (const xmlChar*)"type"), LibXmlFree());
      const xmlChar* typeName = attrValue.get();
      if (!typeName || !typeName[0] || !xmlStrcmp(typeName, "string"))
        return VariantType::STRING;
      if (!xmlStrcmp(typeName, "sbyte"))
        return VariantType::SBYTE;
      if (!xmlStrcmp(typeName, "byte"))
        return VariantType::BYTE;
      if (!xmlStrcmp(typeName, "int16"))
        return VariantType::INT16;
      if (!xmlStrcmp(typeName, "uint16"))
        return VariantType::UINT16;
      if (!xmlStrcmp(typeName, "int32"))
        return VariantType::INT32;
      if (!xmlStrcmp(typeName, "uint32"))
        return VariantType::UINT32;
      if (!xmlStrcmp(typeName, "int64"))
        return VariantType::INT64;
      if (!xmlStrcmp(typeName, "uint64"))
        return VariantType::UINT64;
      if (!xmlStrcmp(typeName, "float"))
        return VariantType::FLOAT;
      if (!xmlStrcmp(typeName, "double"))
        return VariantType::DOUBLE;

      std::stringstream stream;
      stream << std::string("Invalid value type '") << (const char*)typeName << std::string("'. Line ") << node.line << ".";
      throw std::logic_error(stream.str());
    }

    Variant GetVariantValue(OpcUa::VariantType type, xmlNode& node) const
    {
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
        return Variant();

      const char* valueText = (const char*)content.get();
      switch (type)
      {
        case VariantType::SBYTE:
          return (int8_t)strtol(valueText, nullptr, 0);
        case VariantType::BYTE:
          return (uint8_t)strtol(valueText, nullptr, 0);
        case VariantType::INT16:
          return (int16_t)strtol(valueText, nullptr, 0);
        case VariantType::UINT16:
          return (uint16_t)strtol(valueText, nullptr, 0);
        case VariantType::INT32:
          return (int32_t)strtol(valueText, nullptr, 0);
        case VariantType::UINT32:
          return (uint32_t)strtol(valueText, nullptr, 0);
        case VariantType::INT64:
          return (int64_t)strtoll(valueText, nullptr, 0);
        case VariantType::UINT64:
          return (int64_t)strtoll(valueText, nullptr, 0);
        case VariantType::FLOAT:
          return strtof(valueText, nullptr);
        case VariantType::DOUBLE:
          return strtod(valueText, nullptr);
        // TODO check for other types.
        case VariantType::NUL:
          return Variant();

        case VariantType::STRING:
        default:
          break;
      }
      return std::string(valueText);
    }

    OpcUa::AttributeID GetAttributeID(xmlNode& node) const
    {
      if (IsXmlNode(node, "id"))
        return AttributeID::NODE_ID;
      else if (IsXmlNode(node, "class"))
        return AttributeID::NODE_CLASS;
      else if (IsXmlNode(node, "browse_name"))
        return AttributeID::BROWSE_NAME;
      else if (IsXmlNode(node, "display_name"))
        return AttributeID::DISPLAY_NAME;
      else if (IsXmlNode(node, "description"))
        return AttributeID::DESCRIPTION;
      else if (IsXmlNode(node, "write_mask"))
        return AttributeID::WRITE_MASK;
      else if (IsXmlNode(node, "user_write_mask"))
        return AttributeID::USER_WRITE_MASK;
      else if (IsXmlNode(node, "is_abstract"))
        return AttributeID::IS_ABSTRACT;
      else if (IsXmlNode(node, "symmetric"))
        return AttributeID::SYMMETRIC;
      else if (IsXmlNode(node, "inverse_name"))
        return AttributeID::INVERSE_NAME;
      else if (IsXmlNode(node, "contains_no_loops"))
        return AttributeID::CONTAINS_NO_LOOPS;
      else if (IsXmlNode(node, "event_notifier"))
        return AttributeID::EVENT_NOTIFIER;
      else if (IsXmlNode(node, "value"))
        return AttributeID::VALUE;
      else if (IsXmlNode(node, "value_rank"))
        return AttributeID::VALUE_RANK;
      else if (IsXmlNode(node, "array_dimensions"))
        return AttributeID::ARRAY_DIMENSIONS;
      else if (IsXmlNode(node, "access_level"))
        return AttributeID::ACCESS_LEVEL;
      else if (IsXmlNode(node, "user_access_level"))
        return AttributeID::USER_ACCESS_LEVEL;
      else if (IsXmlNode(node, "minimum_sampling_interval"))
        return AttributeID::MINIMUM_SAMPLING_INTERVAL;
      else if (IsXmlNode(node, "historizing"))
        return AttributeID::HISTORIZING;
      else if (IsXmlNode(node, "executable"))
        return AttributeID::EXECUTABLE;
      else if (IsXmlNode(node, "user_executable"))
        return AttributeID::USER_EXECUTABLE;

      return AttributeID::UNKNOWN;
    }

    Variant GetAttributeValue(OpcUa::AttributeID id, xmlNode& node) const
    {
      switch (id)
      {
        case AttributeID::NODE_ID:
          return GetNodeID(node);

        case AttributeID::NODE_CLASS:
          return (uint32_t)GetNodeClass(node);

        case AttributeID::DISPLAY_NAME:
          return GetLocalizedText(node);

        case AttributeID::BROWSE_NAME:
        case AttributeID::DESCRIPTION:
        case AttributeID::INVERSE_NAME:
        case AttributeID::EVENT_NOTIFIER: // TODO Unknown type of attribute..
        case AttributeID::ARRAY_DIMENSIONS:
          return GetText(node);

        case AttributeID::WRITE_MASK:
        case AttributeID::USER_WRITE_MASK:
        case AttributeID::VALUE_RANK:
        case AttributeID::ACCESS_LEVEL:
        case AttributeID::USER_ACCESS_LEVEL:
        case AttributeID::MINIMUM_SAMPLING_INTERVAL:
          return GetUInt32(node);

        case AttributeID::IS_ABSTRACT:
        case AttributeID::SYMMETRIC:
        case AttributeID::CONTAINS_NO_LOOPS:
        case AttributeID::HISTORIZING:
        case AttributeID::EXECUTABLE:
        case AttributeID::USER_EXECUTABLE:
          return GetBool(node);

        case AttributeID::VALUE:
          break;

        default:
          std::cerr << "Unknown attribute '" << node.name << "' at line " << node.line <<  "." << std::endl;
          return GetText(node);
      }
      const VariantType type = GetVariantType(node);
      return GetVariantValue(type, node);
    }

  private:
    Node& OpcUaNode;
    const bool Debug;
  };


  class NodesCollector : private XmlProcessor
  {
  public:
    NodesCollector(std::map<NodeID, Node>& nodes, bool debug)
      : Nodes(nodes)
      , Debug(debug)
    {
    }

    virtual void Process(const xmlNode& node)
    {
      if (!IsNodeTag(node))
      {
        return;
      }

      Node opcuaNode;
      FillNode(node, opcuaNode);
      EnsureNodeIsValid(opcuaNode, node);
      Nodes.insert(std::make_pair(opcuaNode.ID, opcuaNode));
    }

  private:
    void EnsureNodeIsValid(const Node& opcuaNode, const xmlNode& node) const
    {
      if (opcuaNode.ID == NodeID())
      {
        std::stringstream stream;
        stream << "Node at line '" << node.line << "' has no ID.";
        throw std::logic_error(stream.str());
      }
    }

    bool IsNodeTag(const xmlNode& node) const
    {
      return IsXmlNode(node, "node");
    }

    void FillNode(const xmlNode& node, Node& opcuaNode) const
    {
      AttributesCollector attributeCollector(opcuaNode, Debug);

      for (xmlNodePtr subNode = node.children; subNode; subNode = subNode->next)
      {
        if (IsAttributesTag(*subNode))
        {
          attributeCollector.Process(*subNode);
          continue;
        }
        else if (Debug)
        {
          std::cerr << "Unknown node '" << subNode->name << "' at line " << subNode->line <<  "." << std::endl;
        }
      }
    }

  private:
    bool IsAttributesTag(const xmlNode& node) const
    {
      return IsXmlNode(node, "attributes");
    }

  private:
    std::map<NodeID, Node>& Nodes;
    const bool Debug;
  };


  class ConfigurationProcessor
  {
  public:
    ConfigurationProcessor(bool debug)
      : Debug(debug)
    {

    }

    std::map<NodeID, Node> Process(xmlDoc& doc)
    {
      xmlNodePtr rootNode = xmlDocGetRootElement(&doc);
      EnsureRootNodeValid(*rootNode);

      std::map<NodeID, Node> nodes;
      NodesCollector nodesBuilder(nodes, Debug);
      for (xmlNodePtr cur = rootNode->children; cur; cur = cur->next)
      {
        if (IsXmlNode(*cur, "node"))
        {
          nodesBuilder.Process(*cur);
        }
        else if (Debug)
        {
          std::cerr << "Unknown node '" << cur->name << "' at line " << cur->line <<  "." << std::endl;
        }
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

  std::map<NodeID, Node> ParseConfig(const char* configPath, bool debug)
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
    NodesRegistrator(OpcUa::Server::AddressSpaceRegistry& registry, bool debug)
      : Registry(registry)
      , Debug(debug)
    {
    }

    void RegisterNodes(const std::map<NodeID, Node>& nodes)
    {
      for (const auto& node : nodes)
      {
        RegisterNode(node.second);
//        AddReferences(node.first, node.second.Attributes);
      }
    }

  private:
    void RegisterNode(const Node& node)
    {
      Registry.AddAttribute(node.ID, AttributeID::NODE_ID, Variant(node.ID));
      for (const std::pair<AttributeID, Variant>& attr : node.Attributes)
      {
        Registry.AddAttribute(node.ID, attr.first, attr.second);
      }
    }


  private:
    OpcUa::Server::AddressSpaceRegistry& Registry;
    const bool Debug;
  };
} // namespace

namespace OpcUa
{
  namespace Internal
  {

    XmlAddressSpaceLoader::XmlAddressSpaceLoader(OpcUa::Server::AddressSpaceRegistry& registry, bool debug)
      : Registry(registry)
      , Debug(debug)
    {
    }

    void XmlAddressSpaceLoader::Load(const char* fileName)
    {
      std::map<NodeID, Node> nodes = ParseConfig(fileName, Debug);
      NodesRegistrator reg(Registry, Debug);
      reg.RegisterNodes(nodes);
    }

  } // namespace Internal
} // namespace OpcUa
