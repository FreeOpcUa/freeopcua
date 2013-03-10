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

  const char* OPTION_HELP = "help";
  const char* OPTION_GET_ENDPOINTS = "get-endpoints";
  const char* OPTION_SERVER_URI = "server-uri";

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

  void Process(int argc, char** argv)
  {
    namespace po = boost::program_options;
    // Declare the supported options.
    po::options_description desc("Parameters");
    desc.add_options()
      (OPTION_HELP, "produce help message")
      (OPTION_GET_ENDPOINTS, "List endpoints endpoints.")
      (OPTION_SERVER_URI, po::value<std::string>(), "Uri of the server.");

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

    if (vm.count(OPTION_GET_ENDPOINTS)) 
    {
      PrintEndpoints(*computer);
    }
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

