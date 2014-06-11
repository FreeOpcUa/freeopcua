/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/services_registry.h>

namespace
{

  class ServicesRegistryAddon : public Common::Addon, public OpcUa::UaServer::ServicesRegistry
  {
  public:
    DEFINE_CLASS_POINTERS(ServicesRegistryAddon);

  public:
    virtual void Initialize(Common::AddonsManager&, const Common::AddonParameters&)
    {
      Impl = OpcUa::UaServer::CreateServicesRegistry();
    }

    virtual void Stop()
    {
      Impl.reset();
    }

    virtual OpcUa::Remote::Server::SharedPtr GetServer() const
    {
      return Impl->GetServer();
    }

    virtual void RegisterEndpointsServices(std::shared_ptr<OpcUa::Remote::EndpointServices> endpoints)
    {
      Impl->RegisterEndpointsServices(endpoints);
    }

    virtual void UnregisterEndpointsServices()
    {
      Impl->UnregisterEndpointsServices();
    }

    virtual void RegisterViewServices(std::shared_ptr<OpcUa::Remote::ViewServices> views)
    {
      Impl->RegisterViewServices(views);
    }

    virtual void UnregisterViewServices()
    {
      Impl->UnregisterViewServices();
    }

    virtual void RegisterAttributeServices(std::shared_ptr<OpcUa::Remote::AttributeServices> attributes)
    {
      Impl->RegisterAttributeServices(attributes);
    }

    virtual void UnregisterAttributeServices()
    {
      Impl->UnregisterAttributeServices();
    }

    virtual void RegisterNodeManagementServices(std::shared_ptr<OpcUa::Remote::NodeManagementServices> nodes)
    {
      Impl->RegisterNodeManagementServices(nodes);
    }

    virtual void UnregisterNodeManagementServices()
    {
      Impl->UnregisterNodeManagementServices();
    }

    virtual void RegisterSubscriptionServices(std::shared_ptr<OpcUa::Remote::SubscriptionServices> subscriptions)
    {
      Impl->RegisterSubscriptionServices(subscriptions);
    }

    virtual void UnregisterSubscriptionServices()
    {
      Impl->UnregisterSubscriptionServices();
    }

  private:
    ServicesRegistry::UniquePtr Impl;
  };


} // namespace

namespace OpcUa
{
  namespace UaServer
  {


    Common::Addon::UniquePtr ServicesRegistryFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new ::ServicesRegistryAddon());
    }

  }
}
