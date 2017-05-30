/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/services/endpoints.h>

namespace OpcUa
{
namespace Server
{

struct ApplicationData
{
  ApplicationDescription Application;
  std::vector<EndpointDescription> Endpoints;
};

class EndpointsRegistry : public OpcUa::EndpointServices
{
public:
  DEFINE_CLASS_POINTERS(EndpointsRegistry)

public:
  virtual void AddEndpoints(const std::vector<EndpointDescription> & endpoints) = 0;
  virtual void AddApplications(const std::vector<OpcUa::ApplicationDescription> & application) = 0;
};

EndpointsRegistry::UniquePtr CreateEndpointsRegistry();

} // namespace UaServer
} // namespace OpcUa
