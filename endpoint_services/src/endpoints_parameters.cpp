/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#include "endpoints_parameters.h"

#include <iostream>
#include <stdexcept>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::UaServer;

  class OpcUaParameters
  {
  public:
    OpcUaParameters(bool debug)
      : Debug(debug)
    {
    }

    std::vector<ApplicationData> GetApplications(const std::vector<Common::ParametersGroup>& applicationGroups)
    {
      std::vector<ApplicationData> applications;
      for (const Common::ParametersGroup subGroup : applicationGroups)
      {
        std::cout << "Found group : "<< subGroup.Name << std::endl; 
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

  private:
    UserIdentifyTokenType GetTokenType(const std::string& typeName)
    {
      if (typeName == "anonymous")
      {
        return UserIdentifyTokenType::ANONYMOUS;
      }
      else if (typeName == "user_name")
      {
        return UserIdentifyTokenType::USERNAME;
      }
      else if (typeName == "certificate")
      {
        return UserIdentifyTokenType::CERTIFICATE;
      }
      else if (typeName == "issued_token")
      {
        return UserIdentifyTokenType::ISSUED_TOKEN;
      }
      throw std::logic_error("Unknown token type '" + typeName + "'");
    }

    ApplicationType GetApplicationType(const std::string& typeName)
    {
      if (typeName == "server")
      {
        return ApplicationType::SERVER;
      }
      else if (typeName == "client")
      {
        return ApplicationType::CLIENT;
      }
      else if (typeName == "client_and_server")
      {
        return ApplicationType::CLIENT_AND_SERVER;
      }
      else if (typeName == "discovery_server")
      {
        return ApplicationType::DISCOVERY_SERVER;
      }
      throw std::logic_error("Invalid name of type application type: " + typeName);
    }

    MessageSecurityMode GetSecurityMode(const std::string& modeName)
    {
      if (modeName == "none")
      {
        return MessageSecurityMode::MSM_NONE;
      }
      else if (modeName == "sign")
      {
        return MessageSecurityMode::MSM_SIGN;
      }
      else if (modeName == "sign_encrypt")
      {
        return MessageSecurityMode::MSM_SIGN_AND_ENCRYPT;
      }
      throw std::logic_error("Unknown security mode name: " + modeName);
    }

    UserTokenPolicy GetUserTokenPolicy(const std::vector<Common::Parameter>& params)
    {
      Log("Parsing user token policy.");
      UserTokenPolicy tokenPolicy;
      for (const Common::Parameter& param : params)
      {
        if (param.Name == "id")
        {
          tokenPolicy.PolicyID = param.Value;//"Anonymous";
        }
        else if (param.Name == "type")
        {
          tokenPolicy.TokenType = GetTokenType(param.Value);
        }
        else if (param.Name == "uri")
        {
          tokenPolicy.SecurityPolicyURI = param.Value; //"http://opcfoundation.org/UA/SecurityPolicy#None";
        }
        else
        {
          Log("Unknown policy token field", param.Name, param.Value);
        }
      }
      return tokenPolicy;
    }

    EndpointDescription GetEndpointDescription(const Common::ParametersGroup& group)
    {
      Log("Parsing endpoint parameters.");
      EndpointDescription endpoint;
      for (const Common::Parameter param : group.Parameters)
      {
        std::cout << "Param is: " << param.Name << param.Value << std::endl;
        if (param.Name == "security_mode")
        {
          endpoint.SecurityMode = GetSecurityMode(param.Value);
        }
        else if (param.Name == "security_policy_uri")
        {
          endpoint.SecurityPolicyURI = param.Value;
        }
        else if (param.Name == "transport_profile_uri")
        {
          endpoint.TransportProfileURI = param.Value;//"http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
        }
        else if (param.Name == "url")
        {
          endpoint.EndpointURL = param.Value;
        }
        else
        {
          Log("Unknown endpoint parameter: ", param.Name, "=", param.Value);
        }
      }

      for (const Common::ParametersGroup subGroup : group.Groups)
      {
        if (subGroup.Name == "user_token_policy")
        {
          const UserTokenPolicy tokenPolicy = GetUserTokenPolicy(subGroup.Parameters);
          endpoint.UserIdentifyTokens.push_back(tokenPolicy);
        }
        else
        {
          Log("Unknown group of endpoint parameters: ", subGroup.Name);
        }
      }
      return endpoint;
    }

    ApplicationData GetApplicationData(const Common::ParametersGroup& applicationGroup)
    {
      Log("Parsing application parameters.");
      ApplicationData data;
      for (const Common::Parameter param : applicationGroup.Parameters)
      {
        std::cout << "Parsing app paramter " << param.Name << param.Value << std::endl;
        if (param.Name == "uri")
        {
          data.Application.URI = param.Value;
          data.Application.ProductURI = param.Value;
        }
        else if (param.Name == "name")
        {
          data.Application.Name.Encoding = HAS_TEXT;
          data.Application.Name.Text = param.Value;
        }
        else if (param.Name == "type")
        {
          data.Application.Type = GetApplicationType(param.Value);
        }
        else
        {
          Log("Unknown application parameter: ", param.Name, "=", param.Value);
        }
      }

      for (const Common::ParametersGroup group : applicationGroup.Groups)
      {
        std::cout << "parsing group " << group.Name << std::endl;
        if (group.Name == "endpoint")
        {
          EndpointDescription endpoint = GetEndpointDescription(group);
          data.Application.DiscoveryURLs.push_back(endpoint.EndpointURL);
          data.Endpoints.push_back(endpoint);
        }
        else
        {
          Log("Unknown group in the applications parameters: ", group.Name);
        }
      }

      for (EndpointDescription& endpoint : data.Endpoints)
      {
        endpoint.ServerDescription = data.Application;
      }

      return data;
    }

  private:
    template <typename T, typename... Args>
    void Log(T&& msg, Args... args) const
    {
      if (!Debug)
      {
        return;
      }
      std::clog << msg;
      Log(args...);
    }

    void Log() const
    {
      if (!Debug)
      {
        return;
      }
      std::clog << std::endl;
    }

  private:
    const bool Debug;
  };
}

std::vector<OpcUa::UaServer::ApplicationData> OpcUa::ParseEndpointsParameters(const std::vector<Common::ParametersGroup>& applicationGroups, bool debug)
{
  OpcUaParameters parser(debug);
  const std::vector<OpcUa::UaServer::ApplicationData>& data = parser.GetApplications(applicationGroups);
  return data;
}
