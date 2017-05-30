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

#include <opc/ua/services/services.h>

namespace OpcUa
{
namespace Server
{

class ServicesRegistry : private Common::Interface
{
public:
  DEFINE_CLASS_POINTERS(ServicesRegistry)

public:
  virtual std::shared_ptr<OpcUa::Services> GetServer() const = 0;

  virtual void RegisterEndpointsServices(OpcUa::EndpointServices::SharedPtr endpoints) = 0;
  virtual void UnregisterEndpointsServices() = 0;

  virtual void RegisterViewServices(OpcUa::ViewServices::SharedPtr views) = 0;
  virtual void UnregisterViewServices() = 0;

  virtual void RegisterAttributeServices(OpcUa::AttributeServices::SharedPtr attributes) = 0;
  virtual void UnregisterAttributeServices() = 0;

  virtual void RegisterMethodServices(MethodServices::SharedPtr method) = 0;
  virtual void UnregisterMethodServices() = 0;

  virtual void RegisterNodeManagementServices(OpcUa::NodeManagementServices::SharedPtr attributes) = 0;
  virtual void UnregisterNodeManagementServices() = 0;

  virtual void RegisterSubscriptionServices(OpcUa::SubscriptionServices::SharedPtr attributes) = 0;
  virtual void UnregisterSubscriptionServices() = 0;
};

ServicesRegistry::UniquePtr CreateServicesRegistry();

}
} // namespace OpcUa

