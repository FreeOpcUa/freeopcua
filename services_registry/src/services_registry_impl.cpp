/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Server wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/services_registry.h>

#include <opc/ua/protocol/endpoints.h>

namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class ServicesRegistry : public OpcUa::UaServer::ServicesRegistry
  {
  public:
    ServicesRegistry();

  public: // InternalServerAddon
    virtual OpcUa::Remote::Server::SharedPtr GetServer() const;
    virtual void RegisterEndpointsServices(EndpointServices::SharedPtr endpoints) override;
    virtual void UnregisterEndpointsServices()  override;
    virtual void RegisterViewServices(ViewServices::SharedPtr views) override;
    virtual void UnregisterViewServices() override;
    virtual void RegisterNodeManagementServices(NodeManagementServices::SharedPtr addr) override;
    virtual void UnregisterNodeManagementServices() override;
    virtual void RegisterAttributeServices(AttributeServices::SharedPtr attributes) override;
    virtual void UnregisterAttributeServices() override;
    virtual void RegisterSubscriptionServices(SubscriptionServices::SharedPtr service) override;
    virtual void UnregisterSubscriptionServices() override;

  private:
    class InternalServer;
    std::shared_ptr<InternalServer> Comp;
  };


  class DefaultServices
    : public EndpointServices
    , public ViewServices
    , public AttributeServices
    , public NodeManagementServices
    , public SubscriptionServices
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

    virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items)
    {
      return std::vector<AddNodesResult>();
    }

    virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
    {
      return std::vector<StatusCode>();
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

    virtual std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIDs)
    {
      return std::vector<PublishResult>();
    }

    virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters)
    {
      return SubscriptionData();
    }

    virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID> subscriptions)
    {
      return std::vector<StatusCode>();
    }

    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      return MonitoredItemsData();
    }

    virtual void CreatePublishRequest(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
    {
    }
    
  };

  class ServicesRegistry::InternalServer : public Server
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

    virtual EndpointServices::SharedPtr Endpoints() const override
    {
      return EndpointsServices;
    }

    virtual std::shared_ptr<ViewServices> Views() const override
    {
      return ViewsServices;
    }

    virtual std::shared_ptr<NodeManagementServices> NodeManagement() const  override
    {
      return NodeServices;
    }

    virtual std::shared_ptr<AttributeServices> Attributes() const  override
    {
      return AttributesServices;
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions() const  override
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

    void SetAddressSpace(std::shared_ptr<NodeManagementServices> addrs)
    {
      NodeServices = addrs ? addrs : Services;
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
    OpcUa::Remote::AttributeServices::SharedPtr AttributesServices;
    OpcUa::Remote::ViewServices::SharedPtr ViewsServices;
    OpcUa::Remote::NodeManagementServices::SharedPtr NodeServices;
    OpcUa::Remote::EndpointServices::SharedPtr EndpointsServices;
    OpcUa::Remote::SubscriptionServices::SharedPtr SubscriptionsServices;
    std::shared_ptr<DefaultServices> Services;
  };


  ServicesRegistry::ServicesRegistry()
    : Comp(new InternalServer())
  {
  }

  std::shared_ptr<Server> ServicesRegistry::GetServer() const
  {
    return Comp;
  }

  void ServicesRegistry::RegisterEndpointsServices(EndpointServices::SharedPtr endpoints)
  {
    Comp->SetEndpoints(endpoints);
  }

  void ServicesRegistry::UnregisterEndpointsServices()
  {
    Comp->SetEndpoints(EndpointServices::SharedPtr());
  }

  void ServicesRegistry::RegisterViewServices(ViewServices::SharedPtr views)
  {
    Comp->SetViews(views);
  }

  void ServicesRegistry::UnregisterViewServices()
  {
    Comp->SetViews(ViewServices::SharedPtr());
  }

  void ServicesRegistry::RegisterNodeManagementServices(NodeManagementServices::SharedPtr addr)
  {
    Comp->SetAddressSpace(addr);
  }

  void ServicesRegistry::UnregisterNodeManagementServices()
  {
    Comp->SetAddressSpace(NodeManagementServices::SharedPtr());
  }


  void ServicesRegistry::RegisterAttributeServices(AttributeServices::SharedPtr attributes)
  {
    Comp->SetAttributes(attributes);
  }

  void ServicesRegistry::UnregisterAttributeServices()
  {
    Comp->SetAttributes(AttributeServices::SharedPtr());
  }

  void ServicesRegistry::RegisterSubscriptionServices(SubscriptionServices::SharedPtr service)
  {
    Comp->SetSubscriptions(service);
  }

  void ServicesRegistry::UnregisterSubscriptionServices()
  {
    Comp->SetSubscriptions(SubscriptionServices::SharedPtr());
  }

}

namespace OpcUa
{

  UaServer::ServicesRegistry::UniquePtr UaServer::CreateServicesRegistry()
  {
    return UaServer::ServicesRegistry::UniquePtr(new ::ServicesRegistry());
  }

}
