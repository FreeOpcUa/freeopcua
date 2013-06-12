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

#include <map>
#include <stdexcept>
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
    ReferenceID ID;
    NodeID TargetNode;
  };

  struct Attribute
  {
    AttributeID ID;
    Variant Value;
  };

  struct Node
  {
    NodeID ID;
    std::vector<Attribute> Attributes;
    std::vector<Reference> References;
  };

  std::map<NodeID, Node> ParseConfig(const char* configPath)
  {
    xmlDocPtr doc = xmlParseFile(configPath);
    std::map<NodeID, Node> nodes;
    return nodes;
  }
 }

namespace OpcUa
{
  namespace Internal
  {

    XmlAddressSpaceLoader::XmlAddressSpaceLoader(OpcUa::Server::AddressSpaceRegistry& registry)
      : Registry(registry)
    {
    }

    void XmlAddressSpaceLoader::Load(const char* fileName)
    {
      std::map<NodeID, Node> nodes = ParseConfig(fileName);
    }

  } // namespace Internal
} // namespace OpcUa
