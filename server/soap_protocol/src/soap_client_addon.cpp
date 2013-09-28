/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/client/addon.h>

namespace
{

  class SoapEndpoints : public OpcUa::Remote::EndpointServices
  {
  public:
    virtual std::vector<OpcUa::ApplicationDescription> FindServers(const OpcUa::FindServersParameters& params) const
    {
      return std::vector<OpcUa::ApplicationDescription>();
    }

    virtual std::vector<OpcUa::EndpointDescription> GetEndpoints(const OpcUa::EndpointsFilter& filter) const
    {
      return std::vector<OpcUa::EndpointDescription>();
    }

    virtual void RegisterServer(const OpcUa::Remote::ServerParameters& parameters)
    {

    }
  };

  class SoapViews : public OpcUa::Remote::ViewServices
  {
  public:
    virtual std::vector<OpcUa::ReferenceDescription> Browse(const OpcUa::Remote::BrowseParameters& params) const
    {
      return std::vector<OpcUa::ReferenceDescription>();
    }

    virtual std::vector<OpcUa::ReferenceDescription> BrowseNext() const
    {
      return std::vector<OpcUa::ReferenceDescription>();
    }
  };

  class SoapAttributes : public OpcUa::Remote::AttributeServices
  {
  public:
    virtual std::vector<OpcUa::DataValue> Read(const OpcUa::ReadParameters& filter) const
    {
      return std::vector<OpcUa::DataValue>();
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter)
    {
      return std::vector<OpcUa::StatusCode>();
    }
  };

  class SoapSubscriptions : public OpcUa::Remote::SubscriptionServices
  {
  public:
    virtual OpcUa::SubscriptionData CreateSubscription(const OpcUa::SubscriptionParameters& parameters)
    {
      return OpcUa::SubscriptionData();
    }
  };

  class SoapComputer : public OpcUa::Remote::Computer
  {
  public:
    virtual void CreateSession(const OpcUa::Remote::SessionParameters& parameters)
    {

    }

    virtual void ActivateSession()
    {

    }

    virtual void CloseSession()
    {

    }

    virtual OpcUa::Remote::EndpointServices::SharedPtr Endpoints() const
    {
      return OpcUa::Remote::EndpointServices::SharedPtr();
    }

    virtual OpcUa::Remote::ViewServices::SharedPtr Views() const
    {
      return OpcUa::Remote::ViewServices::SharedPtr();
    }

    virtual OpcUa::Remote::AttributeServices::SharedPtr Attributes() const
    {
      return OpcUa::Remote::AttributeServices::SharedPtr();
    }

    virtual OpcUa::Remote::SubscriptionServices::SharedPtr Subscriptions() const
    {
      return OpcUa::Remote::SubscriptionServices::SharedPtr();
    }
  };


  class SoapClientAddon : public OpcUa::Client::Addon
  {
  public:
    virtual void Initialize(Common::AddonsManager& manager, const Common::AddonParameters& parameters)
    {

    }

    virtual void Stop()
    {

    }

  public:
    virtual std::string GetProtocol() const
    {
      return "http";
    }

    virtual OpcUa::Remote::Computer::SharedPtr Connect(const std::string& url)
    {
      return OpcUa::Remote::Computer::SharedPtr();
    }
  };
}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new SoapClientAddon());
}
