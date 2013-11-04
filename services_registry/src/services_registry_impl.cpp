/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
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

    virtual std::vector<ReferenceDescription> Browse(const NodesQuery& query) const
    {
      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<ReferenceDescription> BrowseNext() const
    {
      return std::vector<ReferenceDescription>();
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
  };


}

using namespace OpcUa::Impl;

class RequestProcessor::InternalComputer : public Computer
{
public:
  InternalComputer()
    : Services(new DefaultServices())
  {
    SetEndpoints(Services);
    SetViews(Services);
    SetAttributes(Services);
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

  virtual std::shared_ptr<AttributeServices> Attributes() const
  {
    return AttributesServices;
  }

  virtual std::shared_ptr<SubscriptionServices> Subscriptions() const
  {
    return std::shared_ptr<SubscriptionServices>();
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

  void SetAttributes(std::shared_ptr<AttributeServices> attributes)
  {
    AttributesServices = attributes ? attributes : Services;
  }

public:
  std::shared_ptr<AttributeServices> AttributesServices;
  std::shared_ptr<ViewServices> ViewsServices;
  std::shared_ptr<EndpointServices> EndpointsServices;
  std::shared_ptr<DefaultServices> Services;
};


RequestProcessor::RequestProcessor()
  : Comp(new InternalComputer())
{
}

void RequestProcessor::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
}

void RequestProcessor::Stop()
{
}

std::shared_ptr<Computer> RequestProcessor::GetComputer() const
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

void RequestProcessor::RegisterAttributeServices(std::shared_ptr<OpcUa::Remote::AttributeServices> attributes)
{
  Comp->SetAttributes(attributes);
}

void RequestProcessor::UnregisterAttributeServices()
{
  Comp->SetAttributes(std::shared_ptr<OpcUa::Remote::AttributeServices>());
}
