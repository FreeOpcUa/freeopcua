#include <algorithm>
#include <opc/common/addons_core/config_file.h>
#include <opc/ua/server/standard_namespace.h>


#include <opc/ua/node.h>

#include <opc/ua/opcuaserver.h>

#include <iostream>

namespace OpcUa
{
  void OPCUAServer::SetConfigFile(const std::string path)
  {
    config_path = path;
  }
/*
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

      if (loadCppAddressSpace)
      {

        Common::ModuleConfiguration mod8;
        mod8.ID = "standard_namespace";
        mod8.Path = "libstandard_ns_addon.so";
        mod8.Dependencies.push_back("services_registry");
        modules.push_back(mod8);

      }

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
*/
  OPCUAServer::OPCUAServer()
  {
    //addons = Common::CreateAddonsManager();
  }

  void OPCUAServer::Start()
  {
    UaServer::ServicesRegistryFactory RegistryFactory;
    Common::Addon::SharedPtr tmp1 = RegistryFactory.CreateAddon();
    Registry = std::dynamic_pointer_cast<UaServer::ServicesRegistryAddon>(tmp1);
    //UaServer::ServicesRegistryAddon::SharedPtr Registry = UaServer::CreateServiceRegistry();
    UaServer::TcpServerFactory TcpServFactory;
    Common::Addon::SharedPtr TcpServer1 = TcpServFactory.CreateAddon();
    TcpServer = std::dynamic_pointer_cast<UaServer::TcpServerAddon>(TcpServer1);
    //UaServer::TcpServerAddon::SharedPtr TcpServer = std::dynamic_pointer_cast<UaServer::TcpServerAddon>(TcpServFactory.CreateAddon());
      //UaServer::TcpServerAddon::SharedPtr TcpServer = UaServer::CreateTcpServer();
      EndpointsServices = UaServer::CreateEndpointsServices(Registry);
     
      std::vector<ApplicationDescription> Applications;
      ApplicationDescription appdesc;
      appdesc.Name = LocalizedText(name);
      appdesc.URI = uri;
      appdesc.Type = ApplicationType::SERVER;
      Applications.push_back(appdesc);
      std::vector<EndpointDescription> Endpoints;
      EndpointDescription ed;
      ed.EndpointURL = endpoint;
      ed.SecurityMode = MessageSecurityMode::MSM_NONE;
      ed.SecurityPolicyURI = "http://opcfoundation.org/UA/SecurityPolicy#None";
      ed.TransportProfileURI = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
      Endpoints.push_back(ed);
      

    UaServer::AddressSpaceAddonFactory AddressSpaceFactory;
    Common::Addon::SharedPtr tmp2 = AddressSpaceFactory.CreateAddon();
    AddressSpace = std::dynamic_pointer_cast<UaServer::AddressSpace>(tmp2);
    Registry->RegisterViewServices(AddressSpace);
    Registry->RegisterAttributeServices(AddressSpace);
    Registry->RegisterNodeManagementServices(AddressSpace);
    Registry->RegisterSubscriptionServices(AddressSpace);

    UaServer::CreateStandardNamespace(Registry->GetServer()->NodeManagement());

      //UaServer::AddressSpace::SharedPtr AddressSpace = UaServer::CreateEndpointsServices(Registry);
      //conf.Applications.push_back();
      //conf.Endoints.push_back();
    Protocol = UaServer::CreateOpcUaProtocol(Registry, TcpServer, Endpoints);
    Server = Registry->GetServer(); //Not necessary when we have a link to Registry anyway

 
  }
  /*
  void OPCUAServer::Start()
  {
    const Common::ModulesConfiguration modules = GetConfig(); 

    //std::vector<Common::AddonInformation> infos; 
    std::transform(modules.begin(), modules.end(), std::back_inserter(infos), std::bind(&Common::GetAddonInfomation, std::placeholders::_1));

      for (const Common::AddonInformation& config : infos)
      {
        addons->Register(config);
      }
      addons->Start();

    //const Common::AddonsManager& addons = application->GetAddonsManager();
    OpcUa::UaServer::ServicesRegistryAddon::SharedPtr registry = addons->GetAddon<OpcUa::UaServer::ServicesRegistryAddon>(OpcUa::UaServer::ServicesRegistryAddonID);
    Server = registry->GetServer();
    Node root = GetRootNode();
    try
    {
      Node serverarray = root.GetChild(std::vector<std::string>({"0:Objects", "0:Server", "0:ServerArray"}));
      serverarray.SetValue(endpoint); //writing endpoint is wrong it should be urn of server....
    }
    catch (NodeNotFoundException ex)
    {
      std::cout << "Error could not get serverArray node" << std::endl;
    }

    //OpcUa::UaServer::TcpServerAddon::SharedPtr tcpserv = addons.GetAddon<OpcUa::UaServer::TcpServerAddon>(OpcUa::UaServer::TcpServerAddonID);
    //tcpserv->;
    //tcpserv->operator=
  }
  */
  
  Node OPCUAServer::GetNode(NodeID nodeid)
  {
    return Node(Server, nodeid);
  }

  void OPCUAServer::Stop()
  {
    //addons->Stop();

    std::cout << "Stopping opcua server application" << std::endl;
  }

  Node OPCUAServer::GetRootNode()
  {
    return Node(Server, OpcUa::ObjectID::RootFolder);
  }

  Node OPCUAServer::GetObjectsNode()
  {
    return Node(Server, OpcUa::ObjectID::ObjectsFolder);
  }
}
