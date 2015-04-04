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

#include <src/server/opcua_protocol.h>

#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/common/addons_core/addon_manager.h>


/*
  <parameters>
    <!-- Enable/disable debuging of module. -->
    <debug>1</debug>
    <application>
      <!-- Common parameters for all endpoints  -->
      <application_name>Test OPC UA Server</application_name>
      <application_uri>opcua.treww.org</application_uri>
      <application_type>server</application_type>
      <security_mode>none</security_mode>
      <security_policy_uri>http://opcfoundation.org/UA/SecurityPolicy#None</security_policy_uri>
      <transport_profile_uri>http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary</transport_profile_uri>
      <user_token_policy>
        <id>anonymous</id>
        <type>anonymous</type>
        <uri>http://opcfoundation.org/UA/SecurityPolicy#None</uri>
      </user_token_policy>

      <!-- endpoint specific parameters. It may overwrite common parameters. -->
      <endpoint>
        <url>opc.tcp://localhost:4841</url>
      </endpoint>
    <application>
  </parameters>
*/


namespace OpcUa
{
  namespace Test
  {
/*
    void RegisterOpcTcpAddon(Common::AddonsManager& addons)
    {
      Common::AddonInformation opcTcp;
      opcTcp.Factory.reset(new OpcUa::UaServer::OpcUaProtocolAddonFactory());
      opcTcp.Id = OpcUa::UaServer::OpcUaProtocolAddonId;
      opcTcp.Dependencies.push_back(OpcUa::UaServer::EndpointsRegistryAddonId);

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
      //opcTcp.Parameters.Parameters.push_back(Common::Parameter("debug", "1"));

      addons.Register(opcTcp);
    }
*/
  }
}
