/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Implementation of computer for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/services/services.h>

#include <stdexcept>

namespace
{
  using namespace OpcUa;

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
      app.Type = ApplicationType::Client;
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
      endpoint.SecurityMode = OpcUa::MessageSecurityMode::None;
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
      policy.PolicyId = "PolicyId";
      policy.SecurityPolicyURI = "SecurityPolicyURI";
      policy.TokenType = UserIdentifyTokenType::USERNAME;
      return std::vector<OpcUa::UserTokenPolicy>(1, policy);
    }

  private:
    std::vector<ApplicationDescription> Applications;
    std::vector<EndpointDescription> Endpoints;
  };

  void Assert(bool isTrue, const char* msg)
  {
    if (!isTrue)
    {
      throw std::logic_error(msg);
    }
  }

  class TestAttributes : public AttributeServices
  {
  public:
    virtual std::vector<DataValue> Read(const OpcUa::ReadParameters& params) const
    {
      Assert(params.MaxAge == 1, "Invalid MaxAgeValue");
      Assert(params.TimestampsType == TimestampsToReturn::BOTH, "Invalid value of TimestampsToReturn.");
      Assert(params.AttributesToRead.size() == 1, "Invalid size of AttributesToRead.");

      OpcUa::ReadValueId id = params.AttributesToRead[0];
      Assert(id.Attribute == AttributeId::Value, "Invalid value of Attribute Id.");
      Assert(id.DataEncoding.NamespaceIndex == 3, "Invalid namespace index in DataEncoding.");
      Assert(id.DataEncoding.Name == "binary", "Invalid name in DataEncoding.");
      Assert(id.IndexRange == "1:2", "Invalid value of IndexRange.");
      Assert(id.Node.IsInteger(), "Node id is not integer.");
      Assert(id.Node.GetIntegerIdentifier() == 2, "Node id is not equal to 2.");
      Assert(id.Node.GetNamespaceIndex() == 1, "NodeId's namespace index is not equal to 1.");

      DataValue data;
      data.Encoding =
        DATA_VALUE |
        DATA_VALUE_STATUS_CODE |
        DATA_VALUE_SOURCE_TIMESTAMP |
        DATA_VALUE_Server_TIMESTAMP |
        DATA_VALUE_SOURCE_PICOSECONDS |
        DATA_VALUE_Server_PICOSECONDS;
      data.ServerPicoseconds = 1;
      data.ServerTimestamp.Value = 2;
      data.SourcePicoseconds = 3;
      data.SourceTimestamp.Value = 4;
      data.Status = StatusCode::BadNotReadable;
      data.Value = std::string("value");

      return std::vector<DataValue>(1, data);
    }

    virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& data)
    {
      Assert(data.size() == 1, "Invalid number od data for write.");
      const OpcUa::WriteValue& value = data[0];
      Assert(value.Attribute == OpcUa::AttributeId::Value, "Invalid id of attribute.");
      Assert(value.Node.Encoding == NodeIdEncoding::EV_STRING, "Invalid encoding of node.");
      Assert(value.Node.StringData.NamespaceIndex == 1, "Invalid namespace of node.");
      Assert(value.Node.StringData.Identifier == "node", "Invalid identifier of node.");
      Assert(value.NumericRange == "1:2", "Invalid numeric range.");
      Assert(value.Data.ServerPicoseconds == 1, "Invalid ServerPicoseconds.");
      Assert(value.Data.ServerTimestamp.Value == 2, "Invalid ServerTimeStamp.");
      Assert(value.Data.SourcePicoseconds == 3, "Invalid SourcePicoseconds.");
      Assert(value.Data.SourceTimestamp.Value == 4, "Invalid SourceTimeStamp.");
      Assert(value.Data.Status == StatusCode::BadNotReadable, "Invalid data status.");
      Assert(value.Data.Value.Type() == VariantType::STRING, "Invalid data type.");
      Assert(value.Data.Value.As<std::string>().size() == 1, "Invalid number of strings in variant.");
      Assert(value.Data.Value == std::vector<std::string>(1, "value"), "Invalid data value.");

      const uint8_t encoding =
          DATA_VALUE |
          DATA_VALUE_STATUS_CODE |
          DATA_VALUE_SOURCE_TIMESTAMP |
          DATA_VALUE_Server_TIMESTAMP |
          DATA_VALUE_SOURCE_PICOSECONDS |
          DATA_VALUE_Server_PICOSECONDS;

      Assert(value.Data.Encoding == encoding, "Invalid encoding mask.");

      return std::vector<StatusCode>(1, StatusCode::BadNotReadable);
    }
  };

  class TestViewServices : public ViewServices
  {
  public:
    virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
    {
      ReferenceDescription ref;
      ref.BrowseName.Name = "Name";
      ref.BrowseName.NamespaceIndex = 1;
      ref.DisplayName.Text = "Text";
      ref.IsForward = true;
      ref.ReferenceTypeId.Encoding = OpcUa::NodeIdEncoding::EV_STRING;
      ref.ReferenceTypeId.StringData.NamespaceIndex = 2;
      ref.ReferenceTypeId.StringData.Identifier = "Identifier";
      ref.TargetNodeClass = OpcUa::NodeClass::Variable;
      ref.TargetNodeId.Encoding = OpcUa::NodeIdEncoding::EV_FOUR_BYTE;
      ref.TargetNodeId.FourByteData.NamespaceIndex = 3;
      ref.TargetNodeId.FourByteData.Identifier = 4;
      ref.TargetNodeTypeDefinition.Encoding = OpcUa::NodeIdEncoding::EV_NUMERIC;
      ref.TargetNodeTypeDefinition.NumericData.NamespaceIndex = 5;
      ref.TargetNodeTypeDefinition.NumericData.Identifier = 6;
      return std::vector<ReferenceDescription>(1, ref);
    }

    virtual std::vector<ReferenceDescription> BrowseNext() const
    {
      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      throw std::logic_error("not implemented.");
    }
  };

  class TestComputer : public Services
  {
  public:
    TestComputer(const std::string& url)
      : EndpointsImpl(new TestEndpoints(url))
      , ViewsImpl(new TestViewServices())
      , AttributesImpl(new TestAttributes)
    {
    }

    virtual void CreateSession(const RemoteSessionParameters& parameters)
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

    virtual std::shared_ptr<EndpointServices> Endpoints()
    {
      return EndpointsImpl;
    }

    virtual std::shared_ptr<ViewServices> Views()
    {
      return ViewsImpl;
    }

    virtual std::shared_ptr<AttributeServices> Attributes()
    {
      return AttributesImpl;
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions()
    {
      throw std::logic_error("not implemented.");
    }

    virtual std::shared_ptr<NodeManagementServices> NodeManagement()
    {
      throw std::logic_error("not implemented.");
    }

  private:
    EndpointServices::SharedPtr EndpointsImpl;
    ViewServices::SharedPtr ViewsImpl;
    AttributeServices::SharedPtr AttributesImpl;
  };

}

namespace OpcUa
{

  std::unique_ptr<Services> Connect(const std::string& url)
  {
    return std::unique_ptr<Services>(new TestComputer(url));
  }

}

