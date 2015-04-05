/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/server/addons/subscription_service.h>

#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/asio_addon.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/server/subscription_service.h>

#include <iostream>

namespace
{

  class SubscriptionAddon:
	public Common::Addon,
	public OpcUa::Server::SubscriptionService
  {
  public:
    void Initialize(Common::AddonsManager& manager, const Common::AddonParameters& parameters)
    {
      ApplyAddonParameters(parameters);
      Services = manager.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);
      OpcUa::Server::AddressSpace::SharedPtr addressSpace = manager.GetAddon<OpcUa::Server::AddressSpace>(OpcUa::Server::AddressSpaceRegistryAddonId);
      OpcUa::Server::AsioAddon::SharedPtr asio = manager.GetAddon<OpcUa::Server::AsioAddon>(OpcUa::Server::AsioAddonId);
      Subscriptions = OpcUa::Server::CreateSubscriptionService(addressSpace, asio->GetIoService(), Debug);
      Services->RegisterSubscriptionServices(Subscriptions);
    }

    void Stop()
    {
      Services->UnregisterSubscriptionServices();
      Subscriptions.reset();
      Services.reset();
    }

  public:
    void TriggerEvent(OpcUa::NodeId node, OpcUa::Event event)
    {
      Subscriptions->TriggerEvent(node, event);
    }

  public:
    OpcUa::SubscriptionData CreateSubscription(const OpcUa::CreateSubscriptionRequest& request, std::function<void (OpcUa::PublishResult)> callback)
    {
      return Subscriptions->CreateSubscription(request, callback);
    }

    std::vector<OpcUa::StatusCode> DeleteSubscriptions(const std::vector<OpcUa::IntegerId>& subscriptions)
    {
      return Subscriptions->DeleteSubscriptions(subscriptions);
    }

    void Publish(const OpcUa::PublishRequest& request)
    {
      Subscriptions->Publish(request);
    }

    OpcUa::RepublishResponse Republish(const OpcUa::RepublishParameters& request)
    {
      return Subscriptions->Republish(request);
    }

    OpcUa::MonitoredItemsData CreateMonitoredItems(const OpcUa::MonitoredItemsParameters& parameters)
    {
      return Subscriptions->CreateMonitoredItems(parameters);
    }

    std::vector<OpcUa::StatusCode> DeleteMonitoredItems(const OpcUa::DeleteMonitoredItemsParameters& parameters)
    {
      return Subscriptions->DeleteMonitoredItems(parameters);
    }


  private:
    void ApplyAddonParameters(const Common::AddonParameters& addons)
    {
      for (const Common::Parameter parameter : addons.Parameters)
      {
        if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
        {
          std::cout << "SubscriptionService | Debug mode enabled." << std::endl;
          Debug = true;
        }
      }
    }

  private:
    SubscriptionService::SharedPtr Subscriptions;
    OpcUa::Server::ServicesRegistry::SharedPtr Services;
    bool Debug = false;
  };

}

namespace OpcUa
{
  namespace Server
  {
    Common::Addon::UniquePtr SubscriptionServiceAddonFactory::CreateAddon()
    {
	  return Common::Addon::UniquePtr(new SubscriptionAddon());
    }
  }
}
