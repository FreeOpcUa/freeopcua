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

#include <opc/common/addons_core/addon.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/tcp_server_addon.h>


namespace OpcUa
{
  namespace UaServer
  {

    const char OpcUaProtocolAddonID[] = "opcua_protocol";

    Common::Addon::UniquePtr CreateOpcUaProtocol(UaServer::ServicesRegistryAddon::SharedPtr Registry, OpcUa::UaServer::TcpServerAddon::SharedPtr TcpAddon, const std::vector<EndpointDescription>& Endpoints);

  } // namespace UaServer
} // nmespace OpcUa
