/// @author Alexander Rykovanov 2013
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
        const AttributeID id = GetAttributeID(*subNode);
        if (id == AttributeID::NODE_ID)
        {
          OpcUaNode.ID = GetNodeID(*subNode);
          continue;
        }
        const Variant value = GetAttributeValue(id, *subNode);
        OpcUaNode.Attributes.insert(std::make_pair(id, value));
      }
    }

  private:
    bool IsAttributes(const xmlNode& node) const
    {
      return IsXmlNode(node, "attributes");
    }

    bool IsAttribute(const xmlNode& node) const
    {
      return IsXmlNode(node, "attribute");
    }

    bool IsId(const xmlNode& node) const
    {
      return IsXmlNode(node, "id");
    }

    bool IsClass(const xmlNode& node) const
    {
      return IsXmlNode(node, "class");
    }

    bool IsBrowseName(const xmlNode& node) const
    {
      return IsXmlNode(node, "browse_name");
    }

    bool IsDisplayName(const xmlNode& node) const
    {
      return IsXmlNode(node, "display_name");
    }

    bool IsDescription(const xmlNode& node) const
    {
      return IsXmlNode(node, "description");
    }

    bool IsWriteMask(const xmlNode& node) const
    {
      return IsXmlNode(node, "write_mask");
    }

    bool IsUserWriteMask(const xmlNode& node) const
    {
      return IsXmlNode(node, "user_write_mask");
    }

    bool IsAbstract(const xmlNode& node) const
    {
      return IsXmlNode(node, "is_abstract");
    }

    bool IsSymmetric(const xmlNode& node) const
    {
      return IsXmlNode(node, "symmetric");
    }

    bool IsInverseName(const xmlNode& node) const
    {
      return IsXmlNode(node, "inverse_name");
    }

    bool IsContainsNoLoops(const xmlNode& node) const
    {
      return IsXmlNode(node, "contains_no_loops");
    }

    bool IsEventNotifier(const xmlNode& node) const
    {
      return IsXmlNode(node, "event_notifier");
    }

    bool IsValue(const xmlNode& node) const
    {
      return IsXmlNode(node, "value");
    }

    bool IsValueRank(const xmlNode& node) const
    {
      return IsXmlNode(node, "value_rank");
    }

    bool IsArrayDimensions(const xmlNode& node) const
    {
      return IsXmlNode(node, "array_dimensions");
    }

    bool IsAccessLevel(const xmlNode& node) const
    {
      return IsXmlNode(node, "access_level");
    }

    bool IsUserAccessLevel(const xmlNode& node) const
    {
      return IsXmlNode(node, "user_access_level");
    }

    bool IsMinimumSamplingInterval(const xmlNode& node) const
    {
      return IsXmlNode(node, "minimum_sampling_interval");
    }

    bool IsHistorizing(const xmlNode& node) const
    {
      return IsXmlNode(node, "historizing");
    }

    bool IsExecutable(const xmlNode& node) const
    {
      return IsXmlNode(node, "executable");
    }

    bool IsUserExecutable(const xmlNode& node) const
    {
      return IsXmlNode(node, "user_executable");
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
      return NumericNodeID(atoi((const char*)content.get()));
    }

    uint32_t GetInt(xmlNode& node) const
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

    Variant GetValue(xmlNode& node) const
    {
/*
      std::unique_ptr<xmlChar, LibXmlFree> content(xmlNodeGetContent(&node));
      if (!content)
      {
        std::stringstream stream;
        stream << "Empty opcua attribute. Line " << node.line << ".";
        throw std::logic_error(stream.str());
      }
      return atoi((const char*)content.get());
*/
      return Variant();
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

    OpcUa::AttributeID GetAttributeID(xmlNode& node)
    {
      if (IsId(node))
        return AttributeID::NODE_ID;
      else if (IsClass(node))
        return AttributeID::NODE_CLASS;
      else if (IsBrowseName(node))
        return AttributeID::BROWSE_NAME;
      else if (IsDisplayName(node))
        return AttributeID::DISPLAY_NAME;
      else if (IsDescription(node))
        return AttributeID::DESCRIPTION;
      else if (IsWriteMask(node))
        return AttributeID::WRITE_MASK;
      else if (IsUserWriteMask(node))
        return AttributeID::USER_WRITE_MASK;
      else if (IsAbstract(node))
        return AttributeID::IS_ABSTRACT;
      else if (IsSymmetric(node))
        return AttributeID::SYMMETRIC;
      else if (IsInverseName(node))
        return AttributeID::INVERSE_NAME;
      else if (IsContainsNoLoops(node))
        return AttributeID::CONTAINS_NO_LOOPS;
      else if (IsEventNotifier(node))
        return AttributeID::EVENT_NOTIFIER;
      else if (IsValue(node))
        return AttributeID::VALUE;
      else if (IsValueRank(node))
        return AttributeID::VALUE_RANK;
      else if (IsArrayDimensions(node))
        return AttributeID::ARRAY_DIMENSIONS;
      else if (IsAccessLevel(node))
        return AttributeID::ACCESS_LEVEL;
      else if (IsUserAccessLevel(node))
        return AttributeID::USER_ACCESS_LEVEL;
      else if (IsMinimumSamplingInterval(node))
        return AttributeID::MINIMUM_SAMPLING_INTERVAL;
      else if (IsHistorizing(node))
        return AttributeID::HISTORIZING;
      else if (IsExecutable(node))
        return AttributeID::EXECUTABLE;
      else if (IsUserExecutable(node))
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

        case AttributeID::BROWSE_NAME:
        case AttributeID::DISPLAY_NAME:
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
          return GetInt(node);

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
          break;
      }
      return GetValue(node);
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
