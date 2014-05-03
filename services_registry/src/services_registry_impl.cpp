/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Server wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "services_registry_impl.h"

#include <opc/ua/protocol/endpoints.h>


namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class DefaultServices
    : public EndpointServices
    , public ViewServices
    , public AttributeServices
    , public AddressSpaceServices
    , public SubscriptionServicesServer
  {
  public:
    virtual std::vector<OpcUa::ApplicationDescription> FindServers(const FindServersParameters& params) const
    {
      return std::vector<OpcUa::ApplicationDescription>();
    }
    virtual std::vector<OpcUa::EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      return std::vector<OpcUa::EndpointDescription>();
    }
    virtual void RegisterServer(const ServerParameters& parameters)
    {
    }


    virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
    {
    }

    virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
    {
    }

    virtual std::vector<ReferenceDescription> Browse(const NodesQuery& query) const
    {
      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<ReferenceDescription> BrowseNext() const
    {
      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      return std::vector<BrowsePathResult>();
    }

    virtual std::vector<OpcUa::DataValue> Read(const OpcUa::ReadParameters& filter) const
    {
      DataValue value;
      value.Encoding = DATA_VALUE_STATUS_CODE;
      value.Status = StatusCode::BadNotImplemented;
      return std::vector<OpcUa::DataValue>(filter.AttributesToRead.size(), value);
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter)
    {
      return std::vector<OpcUa::StatusCode>(filter.size(), StatusCode::BadNotImplemented);
    }

    virtual std::vector<MonitoredItemData> PopItemsToPublish(const std::vector<IntegerID>& subscriptions)
    {
      return std::vector<MonitoredItemData>();
    }
    virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters)
    {
      return SubscriptionData();
    }
    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      return MonitoredItemsData();
    }
 


  };

}

using namespace OpcUa::Impl;

class RequestProcessor::InternalServer : public Server
{
public:
  InternalServer()
    : Services(new DefaultServices())
  {
    SetEndpoints(Services);
    SetViews(Services);
    SetAttributes(Services);
    SetSubscriptions(Services);
  }

  virtual void CreateSession(const SessionParameters& parameters)
  {
  }

  virtual void ActivateSession()
  {
  }

  virtual void CloseSession()
  {
  }

  virtual std::shared_ptr<EndpointServices> Endpoints() const
  {
    return EndpointsServices;
  }

  virtual std::shared_ptr<ViewServices> Views() const
  {
    return ViewsServices;
  }

  virtual std::shared_ptr<AddressSpaceServices> AddressSpace() const
  {
    return AddressSpacesServices;
  }


  virtual std::shared_ptr<AttributeServices> Attributes() const
  {
    return AttributesServices;
  }

  virtual std::shared_ptr<SubscriptionServices> Subscriptions() const
  {
    return SubscriptionsServices;
  }

public:
  void SetEndpoints(std::shared_ptr<EndpointServices> endpoints)
  {
    EndpointsServices = endpoints ? endpoints : Services;
  }

  void SetViews(std::shared_ptr<ViewServices> views)
  {
    ViewsServices = views ? views : Services;
  }

  void SetAddressSpace(std::shared_ptr<AddressSpaceServices> addrs)
  {
    AddressSpacesServices = addrs ? addrs : Services;
  }

  void SetAttributes(std::shared_ptr<AttributeServices> attributes)
  {
    AttributesServices = attributes ? attributes : Services;
  }

  void SetSubscriptions(std::shared_ptr<SubscriptionServices> subscriptions)
  {
    SubscriptionsServices = subscriptions ? subscriptions : Services;
  }

public:
  std::shared_ptr<AttributeServices> AttributesServices;
  std::shared_ptr<ViewServices> ViewsServices;
  std::shared_ptr<AddressSpaceServices> AddressSpacesServices;
  std::shared_ptr<EndpointServices> EndpointsServices;
  std::shared_ptr<SubscriptionServices> SubscriptionsServices;
  std::shared_ptr<DefaultServices> Services;
};


RequestProcessor::RequestProcessor()
  : Comp(new InternalServer())
{
}

void RequestProcessor::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
}

void RequestProcessor::Stop()
{
}

std::shared_ptr<Server> RequestProcessor::GetServer() const
{
  return Comp;
}

void RequestProcessor::RegisterEndpointsServices(std::shared_ptr<EndpointServices> endpoints)
{
  Comp->SetEndpoints(endpoints);
}

void RequestProcessor::UnregisterEndpointsServices()
{
  Comp->SetEndpoints(std::shared_ptr<EndpointServices>());
}

void RequestProcessor::RegisterViewServices(std::shared_ptr<OpcUa::Remote::ViewServices> views)
{
  Comp->SetViews(views);
}

void RequestProcessor::UnregisterViewServices()
{
  Comp->SetViews(std::shared_ptr<OpcUa::Remote::ViewServices>());
}

void RequestProcessor::RegisterAddressSpaceServices(std::shared_ptr<OpcUa::Remote::AddressSpaceServices> addr)
{
  Comp->SetAddressSpace(addr);
}

void RequestProcessor::UnregisterAddressSpaceServices()
{
  Comp->SetAddressSpace(std::shared_ptr<OpcUa::Remote::AddressSpaceServices>());
}


void RequestProcessor::RegisterAttributeServices(std::shared_ptr<OpcUa::Remote::AttributeServices> attributes)
{
  Comp->SetAttributes(attributes);
}

void RequestProcessor::UnregisterAttributeServices()
{
  Comp->SetAttributes(std::shared_ptr<OpcUa::Remote::AttributeServices>());
}

void RequestProcessor::RegisterSubscriptionServices(std::shared_ptr<OpcUa::SubscriptionServicesServer> service)
{
  Comp->SetSubscriptions(service);
}

void RequestProcessor::UnregisterSubscriptionServices()
{
  Comp->SetSubscriptions(std::shared_ptr<OpcUa::SubscriptionServicesServer>());
}


