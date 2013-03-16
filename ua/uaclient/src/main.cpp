/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/client/computer.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


namespace
{

  using namespace OpcUa;
  namespace po = boost::program_options;

  const char* OPTION_HELP = "help";
  const char* OPTION_GET_ENDPOINTS = "get-endpoints";
  const char* OPTION_SERVER_URI = "uri";

  const char* OPTION_BROWSE = "browse";
  const char* OPTION_NAMESPACE_INDEX = "namespace-index";
  const char* OPTION_NODE_ID_TWO_BYTE = "node-id-two-byte";
  const char* OPTION_NODE_ID_STRING = "node-id-string";

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

  std::string GetNodeClassName(unsigned nodeClass)
  {
    std::string result;
    if (nodeClass & NODE_CLASS_OBJECT)
    {
      result += "object";
    }

    if (nodeClass & NODE_CLASS_VARIABLE)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "variable";
    }

    if (nodeClass & NODE_CLASS_METHOD)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "method";
    }

    if (nodeClass & NODE_CLASS_OBJECT_TYPE)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "object type";
    }

    if (nodeClass & NODE_CLASS_VARIABLE_TYPE)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "variable type";
    }

    if (nodeClass & NODE_CLASS_REFERENCE_TYPE)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "reference type";
    }

    if (nodeClass & NODE_CLASS_DATA_TYPE)
    {
      if (!result.empty())
      {
        result += ", ";
      }
      result += "data type";
    }

    if (nodeClass & NODE_CLASS_VIEW)
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
        std::cout << "'" << desc.DiscoveryProfileURI << "' ";
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

  void PrintEndpoints(const OpcUa::Remote::Computer& computer)
  {
    std::shared_ptr<OpcUa::Remote::EndpointServices> service = computer.Endpoints();
    OpcUa::Remote::EndpointFilter filter;
    std::vector<OpcUa::EndpointDescription> endpoints = service->GetEndpoints(filter);
    for(auto it = endpoints.begin(); it != endpoints.end(); ++it)
    {
      std::cout << "endpoint:" << std::endl;
      Print(*it, Tabs(2));
    }
  }

  inline void PrintReference(const OpcUa::ReferenceDescription& desc, const Tabs& tabs)
  {
    const Tabs tabs1(tabs.Num + 2);
    std::cout << tabs << "DisplayName: " << desc.DisplayName.Text << std::endl;
    std::cout << tabs << "Browse Name: " << desc.BrowseName.NamespaceIndex << ":" << desc.BrowseName.Name << std::endl;
    std::cout << tabs << "Is Forward: " << desc.IsForward << std::endl;
 
    std::cout << tabs << "Target Node class: " << GetNodeClassName(desc.TargetNodeClass)  << std::endl;
    std::cout << tabs << "Target NodeID:" << std::endl;
    Print(desc.TargetNodeID, tabs1);

    std::cout << tabs << "TypeID:" << std::endl;
    Print(desc.TypeID, tabs1);

    std::cout << tabs << "Type definition ID:" << std::endl;
    Print(desc.TypeDefinition, tabs1);


  }

  void Browse(OpcUa::Remote::ViewServices& view, OpcUa::NodeID nodeID)
  {
    OpcUa::Remote::BrowseParameters params;
    params.Description.NodeToBrowse = nodeID;
    params.Description.Direction = OpcUa::BrowseDirection::FORWARD;
    params.Description.IncludeSubtypes = true;
    params.Description.NodeClasses = OpcUa::NODE_CLASS_ALL;
    params.Description.ResultMask = OpcUa::REFERENCE_ALL;
    params.MaxReferenciesCount = 100;

    std::vector<OpcUa::ReferenceDescription> refs = view.Browse(params);
    while(true)
    {
      if (refs.empty())
      {
        break;
      }
      for (auto refIt : refs)
      {
        std::cout << "reference:" << std::endl;
        PrintReference(refIt, Tabs(2));
        std::cout << std::endl;
      }
      refs = view.BrowseNext();
    }
  }

  uint16_t GetNamespaceIndexOptionValue(const po::variables_map& vm)
  {
    if (vm.count(OPTION_NAMESPACE_INDEX))
    {
      return vm[OPTION_NAMESPACE_INDEX].as<uint16_t>();
    }
    return 0;
  }

  OpcUa::NodeID GetNodeIDOptionValue(const po::variables_map& vm)
  {
    OpcUa::NodeID nodeID;
    if (vm.count(OPTION_NODE_ID_TWO_BYTE))
    {
      nodeID.Encoding = OpcUa::EV_TWO_BYTE;
      nodeID.TwoByteData.Identifier = vm[OPTION_NODE_ID_TWO_BYTE].as<unsigned>();
    }
    else if (vm.count(OPTION_NODE_ID_STRING))
    {
      nodeID.Encoding = OpcUa::EV_STRING;
      nodeID.StringData.NamespaceIndex = GetNamespaceIndexOptionValue(vm);
      nodeID.StringData.Identifier = vm[OPTION_NODE_ID_STRING].as<std::string>();
    }
    else
    {
      nodeID.Encoding = OpcUa::EV_TWO_BYTE;
      nodeID.TwoByteData.Identifier = static_cast<uint8_t>(OpcUa::ObjectID::RootFolder);
    }
    return nodeID;
  }

  void Process(int argc, char** argv)
  {
    // Declare the supported options.
    po::options_description desc("Parameters");
    desc.add_options()
      (OPTION_HELP, "produce help message")
      (OPTION_GET_ENDPOINTS, "List endpoints endpoints.")
      (OPTION_BROWSE, "browse command.")
      (OPTION_SERVER_URI, po::value<std::string>(), "Uri of the server.")
      (OPTION_NODE_ID_TWO_BYTE, po::value<unsigned>(), "Two byte NodeId.")
      (OPTION_NODE_ID_STRING, po::value<std::string>(), "string NodeId.")
      (OPTION_NAMESPACE_INDEX, po::value<uint16_t>(), "Namespace index of the node.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count(OPTION_HELP)) 
    {
      desc.print(std::cout);
      return;
    }

    if (!vm.count(OPTION_SERVER_URI))
    {
      throw po::invalid_command_line_syntax(OPTION_SERVER_URI, po::invalid_command_line_syntax::missing_parameter);
    }

    const std::string serverUri = vm[OPTION_SERVER_URI].as<std::string>();
    std::shared_ptr<OpcUa::Remote::Computer> computer = OpcUa::Remote::Connect(serverUri);

    OpcUa::Remote::SessionParameters session;
    session.ClientDescription.Name.Text = "opcua client";
    session.SessionName = "opua command line";
    session.EndpointURL = serverUri;
    session.Timeout = 1000;

    computer->CreateSession(session);
    computer->ActivateSession();

    if (vm.count(OPTION_GET_ENDPOINTS)) 
    {
      PrintEndpoints(*computer);
    }
    if (vm.count(OPTION_BROWSE)) 
    {
      const OpcUa::NodeID nodeID = GetNodeIDOptionValue(vm);
      Print(nodeID, Tabs(0));
      Browse(*computer->Views(), nodeID);
    }

    computer->CloseSession();
  }
}

int main(int argc, char** argv)
{
  try
  {
    Process(argc, argv);
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

