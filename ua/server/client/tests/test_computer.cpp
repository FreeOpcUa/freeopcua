/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Implementation of computer for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/computer.h>

#include <stdexcept>

namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class TestEndpoints : public EndpointServices
  {
  public:
    TestEndpoints(const std::string& url)
      : Applications(GetApplications(url))
      , Endpoints(GetEndpoints(url))
    {
    }

    virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
    {
      return Applications;
    }

    virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      return Endpoints;
    }

    virtual void RegisterServer(const ServerParameters& parameters)
    {
      throw std::logic_error("not implemented.");
    }

  private:
    std::vector<ApplicationDescription> GetApplications(const std::string& url) const
    {
      ApplicationDescription app;
      app.URI = "URI";
      app.Type = ApplicationType::CLIENT;
      app.ProductURI = "ProductURI";
      app.Name.Text = "Name";
      app.GatewayServerURI = "GatewayServerURI";
      app.DiscoveryProfileURI = "DiscoveryProfileURI";
      app.DiscoveryURLs.push_back(url);
      return std::vector<ApplicationDescription>(1, app);
    }

    std::vector<EndpointDescription> GetEndpoints(const std::string& url) const
    {
      EndpointDescription endpoint;
      endpoint.EndpointURL = url;
      endpoint.SecurityLevel = 1;
      endpoint.SecurityMode = OpcUa::MessageSecurityMode::MSM_NONE;
      endpoint.SecurityPolicyURI = "SecurityPolicyURI";
      endpoint.ServerCertificate.push_back(1);
      endpoint.ServerDescription = GetApplications(url).front();
      endpoint.TransportProfileURI = "TransportProfileURI";
      endpoint.UserIdentifyTokens = GetUserTokens();
      return std::vector<EndpointDescription>(1, endpoint);
    }

    std::vector<OpcUa::UserTokenPolicy> GetUserTokens() const
    {
      OpcUa::UserTokenPolicy policy;
      policy.IssuedTokenType = "IssuedTokenType";
      policy.IssuerEndpointURL = "IssuerEndpointURL";
      policy.PolicyID = "PolicyID";
      policy.SecurityPolicyURI = "SecurityPolicyURI";
      policy.TokenType = UserIdentifyTokenType::USERNAME;
      return std::vector<OpcUa::UserTokenPolicy>(1, policy);
    }

  private:
    std::vector<ApplicationDescription> Applications;
    std::vector<EndpointDescription> Endpoints;
  };

  class TestViewServices : public OpcUa::Remote::ViewServices
  {
  public:
    virtual std::vector<ReferenceDescription> Browse(const BrowseParameters& params) const
    {
      ReferenceDescription ref;
      ref.BrowseName.Name = "Name";
      ref.BrowseName.NamespaceIndex = 1;
      ref.DisplayName.Text = "Text";
      ref.IsForward = true;
      ref.ReferenceTypeID.Encoding = OpcUa::NodeIDEncoding::EV_STRING;
      ref.ReferenceTypeID.StringData.NamespaceIndex = 2;
      ref.ReferenceTypeID.StringData.Identifier = "Identifier";
      ref.TargetNodeClass = OpcUa::NodeClass::Variable;
      ref.TargetNodeID.Encoding = OpcUa::NodeIDEncoding::EV_FOUR_BYTE;
      ref.TargetNodeID.FourByteData.NamespaceIndex = 3;
      ref.TargetNodeID.FourByteData.Identifier = 4;
      ref.TargetNodeTypeDefinition.Encoding = OpcUa::NodeIDEncoding::EV_NUMERIC;
      ref.TargetNodeTypeDefinition.NumericData.NamespaceIndex = 5;
      ref.TargetNodeTypeDefinition.NumericData.Identifier = 6;
      return std::vector<ReferenceDescription>(1, ref);
    }

    virtual std::vector<ReferenceDescription> BrowseNext() const
    {
      return std::vector<ReferenceDescription>();
    }
  };

  class TestComputer : public Computer
  {
  public:
    TestComputer(const std::string& url)
      : EndpointsImpl(new TestEndpoints(url))
      , ViewsImpl(new TestViewServices())
    {
    }

    virtual void CreateSession(const Remote::SessionParameters& parameters)
    {
      throw std::logic_error("not implemented.");
    }

    virtual void ActivateSession()
    {
      throw std::logic_error("not implemented.");
    }

    virtual void CloseSession()
    {
      throw std::logic_error("not implemented.");
    }

    virtual std::shared_ptr<EndpointServices> Endpoints() const
    {
      return EndpointsImpl;
    }

    virtual std::shared_ptr<ViewServices> Views() const
    {
      return ViewsImpl;
    }

    virtual std::shared_ptr<AttributeServices> Attributes() const
    {
      throw std::logic_error("not implemented.");
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions() const
    {
      throw std::logic_error("not implemented.");
    }

  private:
    std::shared_ptr<EndpointServices> EndpointsImpl;
    std::shared_ptr<ViewServices> ViewsImpl;
  };

}

namespace OpcUa
{
  namespace Remote
  {

    std::unique_ptr<Computer> Connect(const std::string& url)
    {
      return std::unique_ptr<Computer>(new TestComputer(url));
    }

  }
}
