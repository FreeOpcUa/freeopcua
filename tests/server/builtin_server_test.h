/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include "builtin_server_addon.h"
#include "builtin_server.h"
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/subscription_service.h>

namespace OpcUa
{
  namespace Test
  {
    // This is a addon that emulates network connection and therefore no real tcp connection is required for testing.
    // It provides communication between client and server in one process
    // With exchanging data through memory.

    void RegisterBuiltinServerAddon(Common::AddonsManager& addons)
    {
      Common::AddonInformation opcTcp;
      opcTcp.Factory.reset(new OpcUa::Server::BuiltingServerFactory());
      opcTcp.Id = OpcUa::Server::OpcUaProtocolAddonId;
      opcTcp.Dependencies.push_back(OpcUa::Server::EndpointsRegistryAddonId);
      opcTcp.Dependencies.push_back(OpcUa::Server::SubscriptionServiceAddonId);

      Common::ParametersGroup application("application");
      application.Parameters.push_back(Common::Parameter("application_name","Test OPC UA Server"));
      application.Parameters.push_back(Common::Parameter("application_uri","opcua.treww.org"));
      application.Parameters.push_back(Common::Parameter("application_type","server"));

      Common::ParametersGroup userTokenPolicy("user_token_policy");
      userTokenPolicy.Parameters.push_back(Common::Parameter("id", "anonymous"));
      userTokenPolicy.Parameters.push_back(Common::Parameter("type", "anonymous"));
      userTokenPolicy.Parameters.push_back(Common::Parameter("uri", "http://opcfoundation.org/UA/SecurityPolicy#None"));
      application.Groups.push_back(userTokenPolicy);

      Common::ParametersGroup endpoint("endpoint");
      endpoint.Parameters.push_back(Common::Parameter("url", "opc.tcp://localhost:4841"));
      endpoint.Parameters.push_back(Common::Parameter("security_mode","none"));
      endpoint.Parameters.push_back(Common::Parameter("transport_profile_uri","http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary"));
      application.Groups.push_back(endpoint);

      opcTcp.Parameters.Groups.push_back(application);
      opcTcp.Parameters.Parameters.push_back(Common::Parameter("debug", "0"));

      addons.Register(opcTcp);

    }

  } // namespace Test
} // namespace OpcUa


