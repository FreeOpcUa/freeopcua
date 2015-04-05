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

  class ServicesRegistry : public OpcUa::Server::ServicesRegistry
  {
  public:
    ServicesRegistry();

  public: // InternalServerAddon
    virtual OpcUa::Services::SharedPtr GetServer() const;
    virtual void RegisterEndpointsServices(EndpointServices::SharedPtr endpoints) override;
    virtual void UnregisterEndpointsServices()  override;
    virtual void RegisterViewServices(ViewServices::SharedPtr views) override;
    virtual void UnregisterViewServices() override;
    virtual void RegisterMethodServices(MethodServices::SharedPtr method) override;
    virtual void UnregisterMethodServices() override;
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
    , public MethodServices
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

    virtual std::vector<CallMethodResult> Call(const std::vector<CallMethodRequest>& methodsToCall)
    {
      return std::vector<CallMethodResult>();
    }

    virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items)
    {
      return std::vector<AddNodesResult>();
    }

    virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
    {
      return std::vector<StatusCode>();
    }

    virtual void AddAttribute(const NodeId& node, AttributeId attribute, const Variant& value)
    {
    }

    virtual void AddReference(const NodeId& sourceNode, const ReferenceDescription& reference)
    {
    }

    virtual std::vector<BrowseResult> Browse(const NodesQuery& query) const
    {
      return std::vector<BrowseResult>();
    }

    virtual std::vector<BrowseResult> BrowseNext() const
    {
      return std::vector<BrowseResult>();
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

    virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback=0)
    {
      return SubscriptionData();
    }

    virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerId>& subscriptions)
    {
      return std::vector<StatusCode>();
    }

    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      return MonitoredItemsData();
    }

    virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params)
    {
      return std::vector<StatusCode>();
    }

    virtual void Publish(const PublishRequest& request)
    {
    }
    
    virtual RepublishResponse Republish(const RepublishParameters& params)
    {
      RepublishResponse response;
      response.Header.ServiceResult = StatusCode::BadNotImplemented; 
      return response;
    }
    
  };

  class ServicesRegistry::InternalServer : public Services
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

    virtual CreateSessionResponse CreateSession(const RemoteSessionParameters& parameters)
    {
      return CreateSessionResponse();
    }

    virtual ActivateSessionResponse ActivateSession(const UpdatedSessionParameters &session_parameters) override
    {
      (void)session_parameters;
      return ActivateSessionResponse();
    }

    virtual OpenSecureChannelResponse OpenSecureChannel(const OpenSecureChannelParameters& parameters)
    {
      return OpenSecureChannelResponse();
    }

    virtual void CloseSecureChannel(uint32_t channelId)
    {
    }

    virtual CloseSessionResponse CloseSession()
    {
      return CloseSessionResponse();
    }

    virtual EndpointServices::SharedPtr Endpoints() override
    {
      return EndpointsServices;
    }

    virtual std::shared_ptr<ViewServices> Views() override
    {
      return ViewsServices;
    }

    virtual std::shared_ptr<MethodServices> Method() override
    {
      return MethodsServices;
    }

    virtual std::shared_ptr<NodeManagementServices> NodeManagement() override
    {
      return NodeServices;
    }

    virtual std::shared_ptr<AttributeServices> Attributes() override
    {
      return AttributesServices;
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions() override
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

    void SetMethod(std::shared_ptr<MethodServices> method)
    {
      MethodsServices = method ? method : Services;
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
    OpcUa::AttributeServices::SharedPtr AttributesServices;
    OpcUa::ViewServices::SharedPtr ViewsServices;
    OpcUa::MethodServices::SharedPtr MethodsServices;
    OpcUa::NodeManagementServices::SharedPtr NodeServices;
    OpcUa::EndpointServices::SharedPtr EndpointsServices;
    OpcUa::SubscriptionServices::SharedPtr SubscriptionsServices;
    std::shared_ptr<DefaultServices> Services;
  };


  ServicesRegistry::ServicesRegistry()
    : Comp(new InternalServer())
  {
  }

  std::shared_ptr<Services> ServicesRegistry::GetServer() const
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

  void ServicesRegistry::RegisterMethodServices(MethodServices::SharedPtr method)
  {
    Comp->SetMethod(method);
  }

  void ServicesRegistry::UnregisterMethodServices()
  {
    Comp->SetMethod(MethodServices::SharedPtr());
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

  Server::ServicesRegistry::UniquePtr Server::CreateServicesRegistry()
  {
    return Server::ServicesRegistry::UniquePtr(new ::ServicesRegistry());
  }

}
