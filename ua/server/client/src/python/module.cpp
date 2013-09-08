/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>

#include <opc/ua/client/remote_computer.h>
#include <opc/ua/computer.h>

#include <functional>

namespace OpcUa
{

  using namespace boost;

  template <typename T>
  python::list ToList(const std::vector<T> objects)
  {
    python::list result;
    std::for_each(objects.begin(), objects.end(),
      [&result](const T& obj)
      {
        result.append(obj);
      }
    );

    return result;
  }

  struct PyApplicationDescription
  {
    std::string URI;
    std::string ProductURI;
    std::string Name;
    unsigned Type;
    std::string GatewayServerURI;
    std::string DiscoveryProfileURI;
    python::list DiscoveryURLs;

    PyApplicationDescription()
      : Type(0)
    {
    }

    explicit PyApplicationDescription(const OpcUa::ApplicationDescription& desc)
      : URI(desc.URI)
      , ProductURI(desc.ProductURI)
      , Name(desc.Name.Text) // NOTE: encoding mask doesn't check.
      , Type(static_cast<unsigned>(desc.Type))
      , GatewayServerURI(desc.GatewayServerURI)
      , DiscoveryProfileURI(desc.DiscoveryProfileURI)
      , DiscoveryURLs(ToList(desc.DiscoveryURLs))
    {
    }
  };

  boost::python::list ToList(const std::vector<ApplicationDescription> nativeApps)
  {
    boost::python::list resultApps;
    std::for_each(
      nativeApps.begin(),
      nativeApps.end(),
      [&resultApps](const ApplicationDescription& app)
      {
        resultApps.append(PyApplicationDescription(app));
      });

    return resultApps;
  }

  struct PyUserTokenPolicy
  {
    std::string PolicyID;
    unsigned TokenType;
    std::string IssuedTokenType;
    std::string IssuerEndpointURL;
    std::string SecurityPolicyURI;

    PyUserTokenPolicy()
      : TokenType(0)
    {
    }

    PyUserTokenPolicy(const UserTokenPolicy& policy)
      : PolicyID(policy.PolicyID)
      , TokenType(static_cast<unsigned>(policy.TokenType))
      , IssuedTokenType(policy.IssuedTokenType)
      , IssuerEndpointURL(policy.IssuerEndpointURL)
      , SecurityPolicyURI(policy.SecurityPolicyURI)
    {
    }
  };

  boost::python::list ToList(const std::vector<OpcUa::UserTokenPolicy> policies)
  {
    boost::python::list result;
    std::for_each(
      policies.begin(),
      policies.end(),
      [&result](const UserTokenPolicy& policy)
      {
        result.append(PyUserTokenPolicy(policy));
      }
    );
    return result;
  }

  struct PyEndpointDescription
  {
    std::string EndpointURL;
    PyApplicationDescription ServerDescription;
    //CertificateData ServerCertificate; TODO
    unsigned SecurityMode;
    std::string SecurityPolicyURI;
    python::list UserIdentifyTokens;
    std::string TransportProfileURI;
    unsigned SecurityLevel;

    PyEndpointDescription()
      : SecurityMode(0)
      , SecurityLevel(0)
    {
    }

    explicit PyEndpointDescription(const EndpointDescription& endpoint)
      : EndpointURL(endpoint.EndpointURL)
      , ServerDescription(endpoint.ServerDescription)
      // , ServerCertificate(endpoint.ServerCertificate)
      , SecurityMode(endpoint.SecurityMode)
      , SecurityPolicyURI(endpoint.SecurityPolicyURI)
      , UserIdentifyTokens(ToList(endpoint.UserIdentifyTokens))
      , TransportProfileURI(endpoint.TransportProfileURI)
      , SecurityLevel(endpoint.SecurityLevel)
    {
    }
  };

  boost::python::list ToList(const std::vector<EndpointDescription> endpoints)
  {
    boost::python::list resultApps;
    std::for_each(
      endpoints.begin(),
      endpoints.end(),
      [&resultApps](const EndpointDescription& endpoint)
      {
        resultApps.append(PyEndpointDescription(endpoint));
      });

    return resultApps;
  }

  class PyComputer
  {
  public:
    explicit PyComputer(const std::string& endpointUrl)
      : Impl(OpcUa::Remote::Connect(endpointUrl))
    {
    }

    python::list FindServers() const
    {
      const OpcUa::FindServersParameters params;
      const std::vector<ApplicationDescription> nativeApps = Impl->Endpoints()->FindServers(params);
      return ToList(nativeApps);
    }

    python::list GetEndpoints() const
    {
      const EndpointsFilter filter;
      const std::vector<EndpointDescription> endpoints = Impl->Endpoints()->GetEndpoints(filter);
      return ToList(endpoints);
    }

  private:
    OpcUa::Remote::Computer::SharedPtr Impl;
  };

}


BOOST_PYTHON_MODULE(MODULE_NAME) // MODULE_NAME specifies via preprocessor in command line
{
  using namespace boost::python;
  using namespace OpcUa;

  enum_<OpcUa::ApplicationType>("ApplicationType")
    .value("CLIENT", OpcUa::ApplicationType::CLIENT)
    .value("CLIENT_AND_SERVER", OpcUa::ApplicationType::CLIENT_AND_SERVER)
    .value("DISCOVERY_SERVER", OpcUa::ApplicationType::DISCOVERY_SERVER)
    .value("SERVER", OpcUa::ApplicationType::SERVER);

  enum_<OpcUa::MessageSecurityMode>("MessageSecurityMode")
    .value("NONE", OpcUa::MessageSecurityMode::MSM_NONE)
    .value("SIGN", OpcUa::MessageSecurityMode::MSM_SIGN)
    .value("SIGN_AND_ENCRYPT", OpcUa::MessageSecurityMode::MSM_SIGN);

  enum_<OpcUa::UserIdentifyTokenType>("UserIdentifyTokenType")
    .value("ANONYMOUS", OpcUa::UserIdentifyTokenType::ANONYMOUS)
    .value("USERNAME", OpcUa::UserIdentifyTokenType::USERNAME)
    .value("CERTIFICATE", OpcUa::UserIdentifyTokenType::CERTIFICATE)
    .value("ISSUED_TOKEN", OpcUa::UserIdentifyTokenType::ISSUED_TOKEN);

  class_<PyApplicationDescription>("ApplicationDescription")
    .def_readwrite("uri", &PyApplicationDescription::URI)
    .def_readwrite("product_uri", &PyApplicationDescription::ProductURI)
    .def_readwrite("name", &PyApplicationDescription::Name)
    .def_readwrite("type", &PyApplicationDescription::Type)
    .def_readwrite("gateway_server_uri", &PyApplicationDescription::GatewayServerURI)
    .def_readwrite("discovery_profile_uri", &PyApplicationDescription::DiscoveryProfileURI)
    .def_readwrite("discovery_urls", &PyApplicationDescription::DiscoveryURLs);


  class_<PyEndpointDescription>("EndpointDescription")
    .def_readwrite("url", &PyEndpointDescription::EndpointURL)
    .def_readwrite("server_description", &PyEndpointDescription::ServerDescription)
    //.def_readwrite("certificate", &PyEndpointDescription::ServerCertificate)
    .def_readwrite("security_mode", &PyEndpointDescription::SecurityMode)
    .def_readwrite("security_policy_uri", &PyEndpointDescription::SecurityPolicyURI)
    .def_readwrite("user_identify_tokens", &PyEndpointDescription::UserIdentifyTokens)
    .def_readwrite("transport_profile_uri", &PyEndpointDescription::TransportProfileURI)
    .def_readwrite("security_level", &PyEndpointDescription::SecurityLevel);

  class_<PyUserTokenPolicy>("UserTokenPolicy")
    .def_readwrite("policy_id", &PyUserTokenPolicy::PolicyID)
    .def_readwrite("token_type", &PyUserTokenPolicy::TokenType)
    .def_readwrite("issued_token_type", &PyUserTokenPolicy::IssuedTokenType)
    .def_readwrite("issuer_endpoint_url", &PyUserTokenPolicy::IssuerEndpointURL)
    .def_readwrite("security_policy_uri", &PyUserTokenPolicy::SecurityPolicyURI);

  class_<PyComputer>("Computer", "Interface for remote opc ua server.", init<std::string>())
    .def("find_servers", &PyComputer::FindServers)
    .def("get_endpoints", &PyComputer::GetEndpoints);
}
