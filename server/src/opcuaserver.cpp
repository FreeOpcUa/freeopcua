#include <algorithm>
#include <opc/common/application.h>
#include <opc/common/addons_core/config_file.h>


#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/tcp_server.h>
#include <opc/ua/node.h>

#include <opc/ua/opcuaserver.h>


namespace OpcUa
{
  void OPCUAServer::SetConfigFile(const std::string path)
  {
    config_path = path;
  }

  Common::ModulesConfiguration const OPCUAServer::GetConfig()
  {
    if (config_path.length() > 0)
    {
      return Common::ParseConfiguration(config_path);
    }
    else
    {

      Common::ModulesConfiguration modules;

      Common::ModuleConfiguration mod1;
      mod1.ID = "services_registry";
      mod1.Path = "libservices_registry_addon.so";
      modules.push_back(mod1);

      Common::ModuleConfiguration mod2;
      mod2.ID = "tcp_server";
      mod2.Path = "libopcua_tcp_server.so";
      modules.push_back(mod2);

      Common::ModuleConfiguration mod3;
      mod3.ID = "endpoints_services";
      mod3.Path = "libendpoints_services_addon.so";
      mod3.Dependencies.push_back("services_registry");
      modules.push_back(mod3);

      Common::ModuleConfiguration mod4;
      mod4.ID = "address_space_registry";
      mod4.Path = "libaddress_space_addon.so";
      mod4.Dependencies.push_back("services_registry");
      modules.push_back(mod4);


      Common::ModuleConfiguration mod6;
      mod6.ID = "xml_address_space_loader";
      mod6.Path = "libxml_address_space_loader_addon.so";
      mod6.Dependencies.push_back("address_space_registry");
      for (std::string path : xml_address_spaces)
      {
        mod6.Parameters.Parameters.push_back(Common::Parameter("file_name", path));
      }
      modules.push_back(mod6);




      Common::ModuleConfiguration mod5;
      mod5.ID = "opcua_protocol";
      mod5.Path = "libopcua_protocol.so";
      mod5.Dependencies.push_back("tcp_server");
      mod5.Dependencies.push_back("services_registry");
      mod5.Dependencies.push_back("endpoints_services");
      mod5.Dependencies.push_back("xml_address_space_loader");

      mod5.Parameters.Parameters.push_back(Common::Parameter("debug", "1"));
      Common::ParametersGroup app("application");
      app.Parameters.push_back(Common::Parameter("name", name));
      app.Parameters.push_back(Common::Parameter("uri", uri));
      app.Parameters.push_back(Common::Parameter("type", "server"));
      Common::ParametersGroup ep("endpoint");
      ep.Parameters.push_back(Common::Parameter("url", endpoint));
      ep.Parameters.push_back(Common::Parameter("security_mode", security_mode));
      ep.Parameters.push_back(Common::Parameter("security_policy_uri", "http://opcfoundation.org/UA/SecurityPolicy#None"));
      ep.Parameters.push_back(Common::Parameter("transport_profile_uri", "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary"));
      //Common::ParametersGroup sec("user_token_policy");
      //sec.Parameters.push_back(Common::Parameter("id", "anonymous"));
      //sec.Parameters.push_back(Common::Parameter("type", "anonymous"));
      //sec.Parameters.push_back(Common::Parameter("uri", "http://opcfoundation.org/UA/SecurityPolicy#None"));
      //ep.Groups.push_back(sec);
      app.Groups.push_back(ep);
      mod5.Parameters.Groups.push_back(app);

      modules.push_back(mod5);
      return modules;
    }
  }

  OPCUAServer::OPCUAServer()
  {
    application = OpcUa::CreateApplication();
  }

  void OPCUAServer::Start()
  {
    const Common::ModulesConfiguration modules = GetConfig(); 

    //std::vector<Common::AddonInformation> infos; 
    std::transform(modules.begin(), modules.end(), std::back_inserter(infos), std::bind(&Common::GetAddonInfomation, std::placeholders::_1));


    application->Start(infos);

    const Common::AddonsManager& addons = application->GetAddonsManager();
    registry = addons.GetAddon<OpcUa::Server::ServicesRegistryAddon>(OpcUa::Server::ServicesRegistryAddonID);
    server = registry->GetComputer();
    //OpcUa::Server::TcpServerAddon::SharedPtr tcpserv = addons.GetAddon<OpcUa::Server::TcpServerAddon>(OpcUa::Server::TcpServerAddonID);
    //tcpserv->;
    //tcpserv->operator=
    



  }

  void OPCUAServer::Stop()
  {
    registry.reset(); //for some reason the pointers must be reset before stopping the application
    server.reset();
    std::cout << "Stopping Application" << std::endl;
    application->Stop();
  }

  Node OPCUAServer::GetRootNode()
  {
    return Node(server, OpcUa::ObjectID::RootFolder);
  }

  Node OPCUAServer::GetObjectsNode()
  {
    return Node(server, OpcUa::ObjectID::ObjectsFolder);
  }
}
