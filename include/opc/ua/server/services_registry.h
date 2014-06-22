/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Server wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/server.h>

namespace OpcUa
{
  namespace UaServer
  {

    class ServicesRegistry : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(ServicesRegistry);

    public:
      virtual std::shared_ptr<OpcUa::Remote::Server> GetServer() const = 0;

      virtual void RegisterEndpointsServices(OpcUa::Remote::EndpointServices::SharedPtr endpoints) = 0;
      virtual void UnregisterEndpointsServices() = 0;

      virtual void RegisterViewServices(OpcUa::Remote::ViewServices::SharedPtr views) = 0;
      virtual void UnregisterViewServices() = 0;

      virtual void RegisterAttributeServices(OpcUa::Remote::AttributeServices::SharedPtr attributes) = 0;
      virtual void UnregisterAttributeServices() = 0;

      virtual void RegisterNodeManagementServices(OpcUa::Remote::NodeManagementServices::SharedPtr attributes) = 0;
      virtual void UnregisterNodeManagementServices() = 0;

      virtual void RegisterSubscriptionServices(OpcUa::Remote::SubscriptionServices::SharedPtr attributes) = 0;
      virtual void UnregisterSubscriptionServices() = 0;
    };

    ServicesRegistry::UniquePtr CreateServicesRegistry();

  }
} // namespace OpcUa

