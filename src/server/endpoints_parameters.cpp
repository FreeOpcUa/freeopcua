/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "endpoints_parameters.h"

#include <iostream>
#include <stdexcept>

namespace
{

using namespace OpcUa;
using namespace OpcUa::Server;

class OpcUaParameters
{
public:
  OpcUaParameters(const Common::Logger::SharedPtr & logger)
    : Logger(logger)
  {
  }

  std::vector<ApplicationData> GetApplications(const std::vector<Common::ParametersGroup> & applicationGroups) const
  {
    std::vector<ApplicationData> applications;

    for (const Common::ParametersGroup subGroup : applicationGroups)
      {
        Log("Found group : ", subGroup.Name);

        if (subGroup.Name == "application")
          {
            const ApplicationData application = GetApplicationData(subGroup);
            applications.push_back(application);
          }

        else
          {
            Log("Unknown parameters group: ", subGroup.Name);
          }
      }

    return applications;
  }

  std::vector<Common::ParametersGroup> GetAddonParameters(const std::vector<ApplicationData> & endpoints) const
  {
    std::vector<Common::ParametersGroup> result(endpoints.size());
    std::transform(endpoints.begin(), endpoints.end(), result.begin(), [this](const ApplicationData & app)
    {
      return ApplicationToParametersGroup(app);
    });
    return result;
  }

private:
  UserTokenType GetTokenType(const std::string & typeName) const
  {
    if (typeName == "anonymous" || typeName.empty())
      { return UserTokenType::Anonymous; }

    else if (typeName == "user_name")
      { return UserTokenType::UserName; }

    else if (typeName == "certificate")
      { return UserTokenType::Certificate; }

    else if (typeName == "issued_token")
      { return UserTokenType::IssuedToken; }

    throw std::logic_error("Unknown token type '" + typeName + "'");
  }

  std::string GetTokenType(OpcUa::UserTokenType type) const
  {
    switch (type)
      {
      case UserTokenType::Anonymous:
        return "anonymous";

      case UserTokenType::UserName:
        return "user_name";

      case UserTokenType::Certificate:
        return "certificate";

      case UserTokenType::IssuedToken:
        return "issued_token";

      default:
        throw std::logic_error("Unknown token type '" + std::to_string((unsigned)type) + "'");
      }
  }

  ApplicationType GetApplicationType(const std::string & typeName) const
  {
    if (typeName == "client" || typeName.empty())
      { return ApplicationType::Client; }

    else if (typeName == "server")
      { return ApplicationType::Server; }

    else if (typeName == "client_and_server")
      { return ApplicationType::ClientAndServer; }

    else if (typeName == "discovery_server")
      { return ApplicationType::DiscoveryServer; }

    throw std::logic_error("Invalid name of type application type: " + typeName);
  }

  std::string GetApplicationType(ApplicationType type) const
  {
    switch (type)
      {
      case ApplicationType::Server:
        return "server";

      case ApplicationType::Client:
        return "client";

      case ApplicationType::ClientAndServer:
        return "client_and_server";

      case ApplicationType::DiscoveryServer:
        return "discovery_server";

      default:
        throw std::logic_error("Unknown application type: " + std::to_string((unsigned)type));
      }
  }

  MessageSecurityMode GetSecurityMode(const std::string & modeName) const
  {
    if (modeName == "none" || modeName.empty())
      { return MessageSecurityMode::None; }

    else if (modeName == "sign")
      { return MessageSecurityMode::Sign; }

    else if (modeName == "sign_encrypt")
      { return MessageSecurityMode::SignAndEncrypt; }

    throw std::logic_error("Unknown security mode name: " + modeName);
  }

  std::string GetSecurityMode(MessageSecurityMode mode) const
  {
    switch (mode)
      {
      case MessageSecurityMode::None:
        return "none";

      case MessageSecurityMode::Sign:
        return "sign";

      case MessageSecurityMode::SignAndEncrypt:
        return "sign_encrypt";

      default:
        throw std::logic_error("Unknown security mode: " + std::to_string((unsigned)mode));
      }
  }

  UserTokenPolicy GetUserTokenPolicy(const std::vector<Common::Parameter> & params) const
  {
    Log("Parsing user token policy.");
    UserTokenPolicy tokenPolicy;

    for (const Common::Parameter & param : params)
      {
        if (param.Name == "id")
          { tokenPolicy.PolicyId = param.Value; }//"Anonymous";

        else if (param.Name == "type")
          { tokenPolicy.TokenType = GetTokenType(param.Value); }

        else if (param.Name == "uri")
          { tokenPolicy.SecurityPolicyUri = param.Value; } //"http://opcfoundation.org/UA/SecurityPolicy#None";

        else if (param.Name == "issued_token_type")
          { tokenPolicy.IssuedTokenType = param.Value; }

        else if (param.Name == "issuer_endpoint_url")
          { tokenPolicy.IssuerEndpointUrl = param.Value; }

        else
          { Log("Unknown policy token field", param.Name, param.Value); }
      }

    return tokenPolicy;
  }

  Common::ParametersGroup GetUserTokenPolicy(const UserTokenPolicy & policy) const
  {
    Log("Parsing user token policy.");
    Common::ParametersGroup policyGroup("user_token_policy");
    policyGroup.Parameters.push_back(Common::Parameter("id", policy.PolicyId));
    policyGroup.Parameters.push_back(Common::Parameter("type", GetTokenType(policy.TokenType)));
    policyGroup.Parameters.push_back(Common::Parameter("uri", policy.SecurityPolicyUri));
    policyGroup.Parameters.push_back(Common::Parameter("issuer_endpoint_url", policy.IssuerEndpointUrl));
    policyGroup.Parameters.push_back(Common::Parameter("issued_token_type", policy.IssuedTokenType));
    return policyGroup;
  }

  EndpointDescription GetEndpointDescription(const Common::ParametersGroup & group) const
  {
    Log("Parsing endpoint parameters.");
    EndpointDescription endpoint;

    for (const Common::Parameter param : group.Parameters)
      {
        Log("Param is: ", param.Name, " = ", param.Value);

        if (param.Name == "security_mode")
          { endpoint.SecurityMode = GetSecurityMode(param.Value); }

        if (param.Name == "security_level")
          { endpoint.SecurityLevel = std::stoi(param.Value); }

        else if (param.Name == "security_policy_uri")
          { endpoint.SecurityPolicyUri = param.Value; }

        else if (param.Name == "transport_profile_uri")
          { endpoint.TransportProfileUri = param.Value; }//"http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";

        else if (param.Name == "url")
          { endpoint.EndpointUrl = param.Value; }

        else
          { Log("Unknown endpoint parameter: ", param.Name, "=", param.Value); }
      }

    for (const Common::ParametersGroup subGroup : group.Groups)
      {
        if (subGroup.Name == "user_token_policy")
          {
            const UserTokenPolicy tokenPolicy = GetUserTokenPolicy(subGroup.Parameters);
            endpoint.UserIdentityTokens.push_back(tokenPolicy);
          }

        else
          {
            Log("Unknown group of endpoint parameters: ", subGroup.Name);
          }
      }

    return endpoint;
  }

  Common::ParametersGroup GetEndpointDescription(const EndpointDescription & endpoint) const
  {
    Common::ParametersGroup ed("endpoint");
    ed.Parameters.push_back(Common::Parameter("security_level", std::to_string(endpoint.SecurityLevel)));
    ed.Parameters.push_back(Common::Parameter("security_mode", GetSecurityMode(endpoint.SecurityMode)));
    ed.Parameters.push_back(Common::Parameter("security_policy_uri", endpoint.SecurityPolicyUri));
    ed.Parameters.push_back(Common::Parameter("transport_profile_uri", endpoint.TransportProfileUri)); //"http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary"
    ed.Parameters.push_back(Common::Parameter("url", endpoint.EndpointUrl));

    for (const UserTokenPolicy & policy : endpoint.UserIdentityTokens)
      {
        ed.Groups.push_back(GetUserTokenPolicy(policy));
      }

    return ed;
  }

  ApplicationData GetApplicationData(const Common::ParametersGroup & applicationGroup) const
  {
    Log("Parsing application parameters.");
    ApplicationData data;

    for (const Common::Parameter param : applicationGroup.Parameters)
      {
        Log("Parsing app paramter ", param.Name, " = ", param.Value);

        if (param.Name == "uri")
          {
            data.Application.ApplicationUri = param.Value;
          }

        else if (param.Name == "product_uri")
          {
            data.Application.ProductUri = param.Value;
          }

        else if (param.Name == "gateway_server_uri")
          {
            data.Application.GatewayServerUri = param.Value;
          }

        else if (param.Name == "discovery_profile")
          {
            data.Application.DiscoveryProfileUri = param.Value;
          }

        else if (param.Name == "name")
          {
            data.Application.ApplicationName = LocalizedText(param.Value);
          }

        else if (param.Name == "type")
          {
            data.Application.ApplicationType = GetApplicationType(param.Value);
          }

        else
          {
            Log("Unknown application parameter: ", param.Name, "=", param.Value);
          }
      }

    for (const Common::ParametersGroup group : applicationGroup.Groups)
      {
        Log("parsing group ", group.Name);

        if (group.Name == "endpoint")
          {
            EndpointDescription endpoint = GetEndpointDescription(group);
            data.Application.DiscoveryUrls.push_back(endpoint.EndpointUrl);
            data.Endpoints.push_back(endpoint);
          }

        else
          {
            Log("Unknown group in the applications parameters: ", group.Name);
          }
      }

    for (EndpointDescription & endpoint : data.Endpoints)
      {
        endpoint.Server = data.Application;
      }

    return data;
  }

  Common::ParametersGroup ApplicationToParametersGroup(const ApplicationData & app) const
  {
    Common::ParametersGroup result("application");
    result.Parameters.push_back(Common::Parameter("discovery_profile", app.Application.DiscoveryProfileUri));
    result.Parameters.push_back(Common::Parameter("uri", app.Application.ApplicationUri));
    result.Parameters.push_back(Common::Parameter("gateway_server_uri", app.Application.GatewayServerUri));
    result.Parameters.push_back(Common::Parameter("product_uri", app.Application.ProductUri));
    result.Parameters.push_back(Common::Parameter("name", app.Application.ApplicationName.Text));
    result.Parameters.push_back(Common::Parameter("type", GetApplicationType(app.Application.ApplicationType)));

    for (const EndpointDescription & endpoint : app.Endpoints)
      {
        result.Groups.push_back(GetEndpointDescription(endpoint));
      }

    return result;
  }

private:
  template <typename T, typename... Args>
  void Log(T && msg, Args... args) const
  {
    LOG_DEBUG(Logger, msg, args...);
    /*
    if (!Debug)
      {
        return;
      }

    std::clog << msg;
    Log(args...);
    */
  }

  /*
  void Log() const
  {
    if (!Debug)
      {
        return;
      }

    std::clog << std::endl;
  }
  */

private:
  Common::Logger::SharedPtr Logger;
};
}

std::vector<OpcUa::Server::ApplicationData> OpcUa::ParseEndpointsParameters(const std::vector<Common::ParametersGroup> & applicationGroups, const Common::Logger::SharedPtr & logger)
{
  OpcUaParameters parser(logger);
  const std::vector<OpcUa::Server::ApplicationData> & data = parser.GetApplications(applicationGroups);
  return data;
}

std::vector<Common::ParametersGroup> OpcUa::CreateCommonParameters(const std::vector<Server::ApplicationData> & endpoints, const Common::Logger::SharedPtr & logger)
{
  OpcUaParameters parser(logger);
  const std::vector<Common::ParametersGroup> & result = parser.GetAddonParameters(endpoints);
  return result;
}
