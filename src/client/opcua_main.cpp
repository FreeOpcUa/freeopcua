/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Remote Server implementaion.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#include "opcua_options.h"

#include <opc/ua/client/addon.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/config_file.h>
#include <opc/common/uri_facade.h>
#include <opc/ua/node.h>
#include <opc/ua/protocol/node_classes.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/variant_visitor.h>
#include <opc/ua/services/services.h>

#include <iostream>
#include <stdexcept>

namespace
{

  using namespace OpcUa;

  struct Tabs
  {
    Tabs(unsigned num = 0)
      : Num(num)
    {
    }
    unsigned Num;
  };

  std::ostream& operator <<(std::ostream& os, const Tabs& tabs)
  {
    for (unsigned i = 0; i < tabs.Num; ++i)
    {
      os << " ";
    }
    return os;
  }


  std::string GetName(OpcUa::MessageSecurityMode mode)
  {
    switch (mode)
    {
      case OpcUa::MSM_NONE:
        return "none";
      case OpcUa::MSM_SIGN:
        return "sign";
      case OpcUa::MSM_SIGN_AND_ENCRYPT:
        return "sign and encrypt";

      default:
        return "unknown";
    }
  }

  std::string GetName(OpcUa::ApplicationType type)
  {
    switch (type)
    {
      case OpcUa::ApplicationType::SERVER:
        return "server";
      case OpcUa::ApplicationType::CLIENT:
        return "client";
      case OpcUa::ApplicationType::CLIENT_AND_SERVER:
        return "client and server";
      case OpcUa::ApplicationType::DISCOVERY_SERVER:
        return "discovery server";
      default:
        return "unknown";
    }
  }

  std::string GetName(OpcUa::UserIdentifyTokenType type)
  {
    switch (type)
    {
      case OpcUa::UserIdentifyTokenType::ANONYMOUS:
        return "anonymous";
      case OpcUa::UserIdentifyTokenType::USERNAME:
        return "username";
      case OpcUa::UserIdentifyTokenType::CERTIFICATE:
        return "x509v3 certificate";
      case OpcUa::UserIdentifyTokenType::ISSUED_TOKEN:
        return "WS_Security token";
      default:
        return "unknown";
    }
  };

  std::string GetNodeClassName(unsigned cls)
  {
    std::string result;
    NodeClass nodeClass = static_cast<NodeClass>(cls);
    if (nodeClass == NodeClass::Object)
    {
      result += "object";
    }

    if (nodeClass == NodeClass::Variable)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "variable";
    }

    if (nodeClass == NodeClass::Method)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "method";
    }

    if (nodeClass == NodeClass::ObjectType)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "object type";
    }

    if (nodeClass == NodeClass::VariableType)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "variable type";
    }

    if (nodeClass == NodeClass::ReferenceType)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "reference type";
    }

    if (nodeClass == NodeClass::DataType)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "data type";
    }

    if (nodeClass == NodeClass::View)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "view";
    }

    return result;
  }




  void Print(const OpcUa::NodeID& nodeID, const Tabs& tabs)
  {
    OpcUa::NodeIDEncoding encoding = static_cast<OpcUa::NodeIDEncoding>(nodeID.Encoding & OpcUa::NodeIDEncoding::EV_VALUE_MASK);

    const Tabs dataTabs(tabs.Num + 2);
    switch (encoding)
    {
      case OpcUa::NodeIDEncoding::EV_TWO_BYTE:
      {
        std::cout << tabs << "Two byte:" << std::endl;
        std::cout << dataTabs << "Identifier:" << (unsigned)nodeID.TwoByteData.Identifier << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_FOUR_BYTE:
      {
        std::cout << tabs << "Four byte:" << std::endl;
        std::cout << dataTabs << "NamespaceIndex:" << (unsigned)nodeID.FourByteData.NamespaceIndex << std::endl;
        std::cout << dataTabs << "Identifier" << (unsigned)nodeID.FourByteData.Identifier << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_NUMERIC:
      {
        std::cout << tabs << "Numeric:" << std::endl;
        std::cout << dataTabs << "NamespaceIndex" << (unsigned)nodeID.NumericData.NamespaceIndex << std::endl;
        std::cout << dataTabs << "Identifier" << (unsigned)nodeID.NumericData.Identifier << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_STRING:
      {
        std::cout << tabs << "String: " << std::endl;
        std::cout << dataTabs << "NamespaceIndex: " << (unsigned)nodeID.StringData.NamespaceIndex << std::endl;
        std::cout << dataTabs << "Identifier: " <<  nodeID.StringData.Identifier << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_BYTE_STRING:
      {
        std::cout << tabs << "Binary: " << std::endl;
        std::cout << dataTabs << "NamespaceIndex: " << (unsigned)nodeID.BinaryData.NamespaceIndex << std::endl;
        std::cout << dataTabs << "Identifier: ";
        for (auto val : nodeID.BinaryData.Identifier) {std::cout << (unsigned)val; }
        std::cout << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_GUID:
      {
        std::cout << tabs << "Guid: " << std::endl;
        std::cout << dataTabs << "Namespace Index: " << (unsigned)nodeID.GuidData.NamespaceIndex << std::endl;
        const OpcUa::Guid& guid = nodeID.GuidData.Identifier;
        std::cout << dataTabs << "Identifier: " << std::hex << guid.Data1 << "-" << guid.Data2 << "-" << guid.Data3;
        for (auto val : guid.Data4) {std::cout << (unsigned)val; }
        break;
      }
      default:
      {
        std::cout << tabs << "unknown id type:" << (unsigned)encoding << std::endl;
        break;
      }
    }

    if (nodeID.Encoding & OpcUa::NodeIDEncoding::EV_NAMESPACE_URI_FLAG)
    {
      std::cout << tabs << "Namespace URI: " << nodeID.NamespaceURI << std::endl;
    }

    if (nodeID.Encoding & OpcUa::NodeIDEncoding::EV_SERVER_INDEX_FLAG)
    {
      std::cout << tabs << "Server index: " << nodeID.ServerIndex << std::endl;
    }
  }

  void Print(const OpcUa::UserTokenPolicy& policy, const Tabs& tabs)
  {
    std::cout << tabs << "ID: " << policy.PolicyID << std::endl;
    std::cout << tabs << "TokenType: " << GetName(policy.TokenType) << std::endl;
    std::cout << tabs << "IssuedTokenType: " << policy.IssuedTokenType  << std::endl;
    std::cout << tabs << "IssuerEndpointURL: " << policy.IssuerEndpointURL << std::endl;
    std::cout << tabs << "SecurityPolicyURI: " << policy.SecurityPolicyURI << std::endl;
  };


  void Print(const OpcUa::ApplicationDescription& desc, const Tabs& tab)
  {
    std::cout << tab << "Name: " << desc.Name.Text << " (" << desc.Name.Locale << ")" << std::endl;
    std::cout << tab << "Type: " << GetName(desc.Type) << std::endl;
    std::cout << tab << "URI: " << desc.URI << std::endl;
    std::cout << tab << "ProductURI: " << desc.ProductURI << std::endl;
    std::cout << tab << "GatewayServerURI: " << desc.GatewayServerURI << std::endl;
    std::cout << tab << "DiscoveryProfileURI: " << desc.DiscoveryProfileURI << std::endl;
    if (!desc.DiscoveryURLs.empty())
    {
      std::cout << tab << "DiscoveryProfileURLs: ";
      for (auto it = desc.DiscoveryURLs.begin(); it != desc.DiscoveryURLs.end(); ++it)
      {
        std::cout << "'" << *it << "' ";
      }
      std::cout << std::endl;
    }
  }

  void Print(const OpcUa::EndpointDescription& desc, const Tabs& tab)
  {
    std::cout << tab << "URL: " << desc.EndpointURL << std::endl;
    std::cout << tab << "SecurityPolicyURI: " << desc.SecurityPolicyURI << std::endl;
    std::cout << tab << "SecurityLevel: " << GetName(desc.SecurityMode) << " (" << (int)desc.SecurityMode << ")" << std::endl;
    std::cout << tab << "TransportProfileURI: " << desc.TransportProfileURI << std::endl;
    std::cout << tab << "SecurityLevel: " << (int)desc.SecurityLevel << std::endl;
    std::cout << tab << "Server description: " << std::endl;
    Print(desc.ServerDescription, Tabs(tab.Num + 2));

    if (!desc.UserIdentifyTokens.empty())
    {
      std::cout << tab << "User identify tokens: " << std::endl;
      for (auto it = desc.UserIdentifyTokens.begin(); it != desc.UserIdentifyTokens.end(); ++it)
      {
        std::cout << Tabs(tab.Num + 2) << "token: " << std::endl;
        Print(*it, Tabs(tab.Num + 4));
      }
      std::cout << std::endl;
    }
//    CertificateData ServerCertificate;
//    std::vector<UserTokenPolicy> ;
  }

  void PrintEndpoints(OpcUa::Services& computer)
  {
    std::shared_ptr<OpcUa::EndpointServices> service = computer.Endpoints();
    OpcUa::EndpointsFilter filter;
    std::vector<OpcUa::EndpointDescription> endpoints = service->GetEndpoints(filter);
    for(auto it = endpoints.begin(); it != endpoints.end(); ++it)
    {
      std::cout << "endpoint:" << std::endl;
      Print(*it, Tabs(2));
    }
  }

  void PrintServers(OpcUa::Services& computer)
  {
    std::shared_ptr<OpcUa::EndpointServices> service = computer.Endpoints();
    OpcUa::FindServersParameters filter;
    std::vector<OpcUa::ApplicationDescription> applications = service->FindServers(filter);
    for(const OpcUa::ApplicationDescription& desc : applications)
    {
      std::cout << "Application:" << std::endl;
      Print(desc, Tabs(2));
    }
  }

  inline void PrintReference(const OpcUa::ReferenceDescription& desc, const Tabs& tabs)
  {
    const Tabs tabs1(tabs.Num + 2);
    std::cout << tabs << "DisplayName: " << desc.DisplayName.Text << std::endl;
    std::cout << tabs << "Browse Name: " << desc.BrowseName.NamespaceIndex << ":" << desc.BrowseName.Name << std::endl;
    std::cout << tabs << "Is Forward: " << desc.IsForward << std::endl;

    std::cout << tabs << "Target Node class: " << GetNodeClassName(static_cast<unsigned>(desc.TargetNodeClass))  << std::endl;
    std::cout << tabs << "Target NodeID:" << std::endl;
    Print(desc.TargetNodeID, tabs1);

    std::cout << tabs << "TypeID:" << std::endl;
    Print(desc.ReferenceTypeID, tabs1);

    std::cout << tabs << "Type definition ID:" << std::endl;
    Print(desc.TargetNodeTypeDefinition, tabs1);
  }

  void Browse(OpcUa::ViewServices& view, OpcUa::NodeID nodeID)
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = nodeID;
    description.Direction = OpcUa::BrowseDirection::Forward;
    description.IncludeSubtypes = true;
    description.NodeClasses = OpcUa::NODE_CLASS_ALL;
    description.ResultMask = OpcUa::REFERENCE_ALL;

    OpcUa::NodesQuery query;
    query.View.Timestamp = OpcUa::CurrentDateTime();
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = 100;

    std::vector<OpcUa::BrowseResult> results = view.Browse(query);
    while(true)
    {
      if (results.empty() ) 
      {
        return;
      }
      if (results[0].Referencies.empty())
      {
        break;
      }
      for (auto refIt : results[0].Referencies)
      {
        std::cout << "reference:" << std::endl;
        PrintReference(refIt, Tabs(2));
        std::cout << std::endl;
      }
      results = view.BrowseNext();
    }
  }


  struct VariantPrinter
  {
    template <typename T>
    void PrintVallue(const T& val)
    {
      std::cout << val;
    }

    void PrintValue(const OpcUa::DiagnosticInfo& info)
    {
      std::cout << "!!!TODO!!!";
    }

    void PrintValue(const OpcUa::Variant& info)
    {
      std::cout << "!!!TODO!!!";
    }

    void PrintValue(const OpcUa::LocalizedText& text)
    {
      std::cout << text.Text << std::endl;
    }

    void PrintValue(const OpcUa::StatusCode& code)
    {
      std::cout << OpcUa::ToString(code) << std::endl;
    }

    template <typename T>
    void OnScalar(const T& val)
    {
      PrintValue(val);
      std::cout << std::endl;
    }

    template <typename T>
    void OnContainer(const std::vector<T>& vals)
    {
      typedef typename std::vector<T>::const_iterator Iterator;
      for (Iterator it = vals.begin(); it != vals.end(); ++it)
      {
	PrintValue(*it);
        std::cout << " ";
      }
      std::cout << std::endl;
    }
  };

  void Print(const OpcUa::Variant& var, const Tabs& tabs)
  {
    VariantPrinter printer;
    TypedVisitor<VariantPrinter> visitor(printer);

    switch (var.Type())
    {
      case VariantType::BOOLEAN:
      {
        std::cout << tabs << "boolean: ";
        break;
      }
      case VariantType::SBYTE:
      {
        std::cout << tabs << "signed byte: ";
        break;
      }
      case VariantType::BYTE:
      {
        std::cout << tabs << "byte: ";
        break;
      }
      case VariantType::INT16:
      {
        std::cout << tabs << "int16: ";
        break;
      }
      case VariantType::UINT16:
      {
        std::cout << tabs << "unsigned int16: ";
        break;
      }
      case VariantType::INT32:
      {
        std::cout << tabs << "int32: ";
        break;
      }
      case VariantType::UINT32:
      {
        std::cout << tabs << "unsigned int32: ";
        break;
      }

      case VariantType::INT64:
      {
        std::cout << tabs << "int64: ";
        break;
      }


      case VariantType::UINT64:
      {
        std::cout << tabs << "unsigned int64: ";
        break;
      }


      case VariantType::FLOAT:
      {
        std::cout << tabs << "float: ";
        break;
      }


      case VariantType::DOUBLE:
      {
        std::cout << tabs << "double: ";
        break;
      }


      case VariantType::STRING:
      {
        std::cout << tabs << "string: ";
        break;
      }


      case VariantType::EXPANDED_NODE_ID:
      case VariantType::NODE_ID:
      {
        std::cout << tabs << "NodeID: " << std::endl;
        break;
      }

      case VariantType::QUALIFIED_NAME:
      {
        std::cout << tabs << "Name: ";
        break;
      }

      case VariantType::LOCALIZED_TEXT:
      {
        std::cout << tabs << "Text: ";
        break;
      }


      case VariantType::DATE_TIME:
      {
        std::cout << "DateTime: " << OpcUa::ToString(var.As<DateTime>()) << std::endl;
        break;
      }
      case VariantType::GUID:
      case VariantType::BYTE_STRING:
      case VariantType::XML_ELEMENT:
      case VariantType::STATUS_CODE:
      case VariantType::DIAGNOSTIC_INFO:
      case VariantType::VARIANT:
      case VariantType::DATA_VALUE:
      case VariantType::NUL:
      case VariantType::EXTENSION_OBJECT:
        break;
      default:
        throw std::logic_error("Unknown variant type.");
    }
    var.Visit(visitor);
    std::cout << std::endl;
  }

  void Print(const DataValue& value, const Tabs& tabs)
  {
    const Tabs tabs1(tabs.Num + 2);
    if (value.Encoding & DATA_VALUE_STATUS_CODE)
    {
      std::cout << tabs << "Status code:" << std::endl;
      std::cout << tabs1 << "0x" << std::hex << static_cast<uint32_t>(value.Status) << std::endl;
    }
    if (value.Encoding & DATA_VALUE)
    {
      std::cout << tabs << "Value:" << std::endl;
      Print(value.Value, tabs1);
    }
  }


  void Read(OpcUa::AttributeServices& attributes, OpcUa::NodeID nodeID, OpcUa::AttributeID attributeID)
  {
    ReadParameters params;
    AttributeValueID attribute;
    attribute.Node = nodeID;
    attribute.Attribute = attributeID;
    params.AttributesToRead.push_back(attribute);
    const std::vector<DataValue> values = attributes.Read(params);
    if (values.size() != 1)
    {
      std::cout << "Server returned " << values.size() << " instead of 1." << std::endl;
      return;
    }
    std::cout << "data value:" << std::endl;
    Print(values.front(), Tabs(2));
  }

  void Write(OpcUa::AttributeServices& attributes, OpcUa::NodeID nodeID, OpcUa::AttributeID attributeID, const OpcUa::Variant& value)
  {
    OpcUa::WriteValue attribute;
    attribute.Node = nodeID;
    attribute.Attribute = attributeID;
    attribute.Data = value;
    std::vector<StatusCode> statuses = attributes.Write(std::vector<OpcUa::WriteValue>(1, attribute));
    for (OpcUa::StatusCode status : statuses)
    {
      std::cout << "Status code: 0x" << std::hex << static_cast<uint32_t>(status) << std::endl;
    }
  }

  void CreateSubscription(OpcUa::SubscriptionServices& subscriptions)
  {
    OpcUa::CreateSubscriptionRequest request;
    request.Parameters.MaxNotificationsPerPublish = 1;
    request.Parameters.Priority = 0;
    request.Parameters.PublishingEnabled = false;
    request.Parameters.RequestedLifetimeCount = 1;
    request.Parameters.RequestedMaxKeepAliveCount = 1;
    request.Parameters.RequestedPublishingInterval = 1000;
    const OpcUa::SubscriptionData data = subscriptions.CreateSubscription(request, [](PublishResult){});
    std::cout << "ID: " << data.ID << std::endl;
    std::cout << "RevisedPublishingInterval: " << data.RevisedPublishingInterval << std::endl;
    std::cout << "RevisedLifetimeCount: " << data.RevisedLifetimeCount << std::endl;
    std::cout << "RevizedMaxKeepAliveCount: " << data.RevizedMaxKeepAliveCount << std::endl;
  }

  void Process(OpcUa::CommandLine& cmd, const Common::AddonsManager& addons)
  {
    const std::string serverURI = cmd.GetServerURI();
    const Common::Uri uri(serverURI);
    OpcUa::Client::Addon::SharedPtr addon = addons.GetAddon<OpcUa::Client::Addon>(uri.Scheme());
    std::shared_ptr<OpcUa::Services> computer = addon->Connect(serverURI);

    if (cmd.IsGetEndpointsOperation())
    {
      PrintEndpoints(*computer);
      return;
    }
    else if (cmd.IsFindServersOperation())
    {
      PrintServers(*computer);
    }

    OpcUa::RemoteSessionParameters session;
    session.ClientDescription.URI = "https://github.com/treww/opc_layer.git";
    session.ClientDescription.ProductURI = "https://github.com/treww/opc_layer.git";
    session.ClientDescription.Name.Text = "opcua client";
    session.ClientDescription.Type = OpcUa::ApplicationType::CLIENT;
    session.SessionName = "opua command line";
    session.EndpointURL = serverURI;
    session.Timeout = 1200000;

    computer->CreateSession(session);
    computer->ActivateSession();

    if (cmd.IsBrowseOperation())
    {
      const OpcUa::NodeID nodeID = cmd.GetNodeID();
      Print(nodeID, Tabs(0));
      Browse(*computer->Views(), nodeID);
    }
    else if (cmd.IsReadOperation())
    {
      const OpcUa::NodeID nodeID = cmd.GetNodeID();
      const OpcUa::AttributeID attributeID = cmd.GetAttribute();
      Read(*computer->Attributes(), nodeID, attributeID);
    }
    else if (cmd.IsWriteOperation())
    {
      const OpcUa::NodeID nodeID = cmd.GetNodeID();
      const OpcUa::AttributeID attributeID = cmd.GetAttribute();
      const OpcUa::Variant value = cmd.GetValue();
      Write(*computer->Attributes(), nodeID, attributeID, value);
    }
    else if (cmd.IsCreateSubscriptionOperation())
    {
      CreateSubscription(*computer->Subscriptions());
    }
    else
    {
      std::cout << "nothing to do" << std::endl;
    }


    computer->CloseSession();
  }

  int RegisterNewModule(const OpcUa::CommandLine& cmd)
  {
    std::cout << "Registering new module." << std::endl;
    const std::string& configDir = cmd.GetConfigDir();
    const std::string& addonID = cmd.GetModuleID();
    const std::string& modulePath = cmd.GetModulePath();

    std::cout << "ID: " << addonID << std::endl;
    std::cout << "Path: " << modulePath << std::endl;
    std::cout << "Configuration file: " << configDir << std::endl;

    Common::Configuration config = Common::ParseConfigurationFiles(configDir);
    const Common::ModulesConfiguration::const_iterator moduleIt = std::find_if(config.Modules.begin(), config.Modules.end(), [&addonID](const Common::ModuleConfiguration& config){return config.ID == addonID;});
    if (moduleIt != config.Modules.end())
    {
      std::cerr << "Module already registered." << std::endl;
      return -1;
    }

    Common::ModuleConfiguration module;
    module.ID = addonID;
    module.Path = modulePath;

    config.Modules.push_back(module);
    Common::SaveConfiguration(config.Modules, configDir);
    std::cout << "Successfully registered." << std::endl;
    return 0;
  }

  int UnregisterModule(const OpcUa::CommandLine& cmd)
  {
    const Common::AddonID addonID = cmd.GetModuleID();
    const std::string& configDir = cmd.GetConfigDir();
    std::cout << "Unregistering module." << std::endl;
    std::cout << "ID: " << addonID << std::endl;
    std::cout << "Configuration file: " << configDir << std::endl;

    Common::Configuration config = Common::ParseConfigurationFiles(configDir);
    Common::ModulesConfiguration::iterator moduleIt = std::find_if(config.Modules.begin(), config.Modules.end(), [&addonID](const Common::ModuleConfiguration& config){return config.ID == addonID;});
    if (moduleIt == config.Modules.end())
    {
      std::cerr << "Module not found" << std::endl;
      return -1;
    }
    config.Modules.erase(moduleIt);
    Common::SaveConfiguration(config.Modules, configDir);

    std::cout << "Successfully unregistered." << std::endl;
    return 0;
  }
}

int main(int argc, char** argv)
{
  try
  {
    OpcUa::CommandLine cmd(argc, argv);
    if (cmd.IsHelpOperation())
    {
      return 0;
    }

    if (cmd.IsRegisterModuleOperation())
    {
      return RegisterNewModule(cmd);
    }

    if (cmd.IsUnregisterModuleOperation())
    {
      return UnregisterModule(cmd);
    }

    const std::string configDir = cmd.GetConfigDir();
    const Common::Configuration& config = Common::ParseConfigurationFiles(configDir);

    std::vector<Common::AddonInformation> infos(config.Modules.size());
    std::transform(config.Modules.begin(), config.Modules.end(), infos.begin(), std::bind(&Common::GetAddonInfomation, std::placeholders::_1));

    Common::AddonsManager::UniquePtr manager = Common::CreateAddonsManager();
    std::for_each(infos.begin(), infos.end(), [&manager](const Common::AddonInformation& addon){
      manager->Register(addon);
    });
    manager->Start();
    Process(cmd, *manager);
    manager->Stop();
    return 0;
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown error." << std::endl;
  }
  return -1;
}

