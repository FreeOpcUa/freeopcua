/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "opcua_protocol.h"

#include "opc_tcp_processor.h"
#include "endpoints_parameters.h"
#include "tcp_server.h"
#include "utils.h"

#include <opc/common/uri_facade.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/input_from_buffer.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>

#include <stdexcept>


namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using namespace OpcUa::UaServer;

  class OpcTcp : public OpcUa::UaServer::IncomingConnectionProcessor
  {
  public:
    OpcTcp(OpcUa::Remote::Server::SharedPtr computer, bool debug)
      : Server(computer)
      , Debug(debug)
    {
    }

    virtual void Process(OpcUa::IOChannel::SharedPtr clientChannel)
    {
      if (!clientChannel)
      {
        if (Debug) std::cerr << "opc_tcp_processor| Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      if (Debug) std::clog << "opc_tcp_processor| Hello client!" << std::endl;

      std::auto_ptr<OpcTcpMessages> messageProcessor(new OpcTcpMessages(Server, *clientChannel, Debug));

      for(;;)
      {
        ProcessData(*clientChannel, *messageProcessor);
      }
    }

    virtual void StopProcessing(OpcUa::IOChannel::SharedPtr clientChannel)
    {
    }

  private:
    void ProcessData(OpcUa::IOChannel& clientChannel, OpcUa::UaServer::OpcTcpMessages& messageProcessor)
    {
      using namespace OpcUa::Binary;

      IStreamBinary iStream(clientChannel);
      ProcessChunk(iStream, messageProcessor);
    }

    // TODO implement collecting full message from chunks before processing.
    void ProcessChunk(IStreamBinary& iStream, OpcTcpMessages& messageProcessor)
    {
      if (Debug) std::cout << "opc_tcp_processor| Processing new chunk." << std::endl;
      Header hdr;
      // Receive message header.
      iStream >> hdr;

      // Receive full message.
      std::vector<char> buffer(hdr.MessageSize());
      OpcUa::Binary::RawBuffer buf(&buffer[0], buffer.size());
      iStream >> buf;
      if (Debug)
      {
        std::clog << "opc_tcp_processor| Received message." << std::endl;
        PrintBlob(buffer);
      }

      // restrict server size code only with current message.
      OpcUa::InputFromBuffer messageChannel(&buffer[0], buffer.size());
      IStreamBinary messageStream(messageChannel);
      messageProcessor.ProcessMessage(hdr.Type, messageStream);

      if (messageChannel.GetRemainSize())
      {
        std::cerr << "opc_tcp_processor| ERROR!!! Message from client has been processed partially." << std::endl;
      }
    }

  private:
    OpcUa::Remote::Server::SharedPtr Server;
    bool Debug;
  };

  class OpcUaProtocol : public OpcUa::UaServer::OpcUaProtocol
  {
  public:
    DEFINE_CLASS_POINTERS(OpcUaProtocol);

  public:
    OpcUaProtocol(OpcUa::UaServer::TcpServer& tcpServer, bool debug)
      : TcpAddon(tcpServer)
      , Debug(debug)
    {
    }

    virtual void StartEndpoints(const std::vector<EndpointDescription>& endpoints, OpcUa::Remote::Server::SharedPtr server) override
    {
      for (const EndpointDescription endpoint : endpoints)
      {
        const Common::Uri uri(endpoint.EndpointURL);
        if (uri.Scheme() == "opc.tcp")
        {
          std::shared_ptr<IncomingConnectionProcessor> processor(new OpcTcp(server, Debug));
          TcpParameters tcpParams;
          tcpParams.Port = uri.Port();
          if (Debug) std::clog << "opc_tcp_processor| Starting listen port " << tcpParams.Port << std::endl;
          TcpAddon.Listen(tcpParams, processor);
          Ports.push_back(tcpParams);
        }
      }
    }

    virtual void StopEndpoints() override
    {
      for (const TcpParameters& params : Ports)
      {
        TcpAddon.StopListen(params);
      }
    }

  private:
    OpcUa::UaServer::TcpServer& TcpAddon;
    std::vector<TcpParameters> Ports;
    bool Debug;
  };


  class OpcUaProtocolAddon : public Common::Addon
  {
  public:
    OpcUaProtocolAddon()
      : Debug(false)
    {
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params) override;
    virtual void Stop() override;

  private:
    void ApplyAddonParameters(const Common::AddonParameters& params);
    void PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const;

  private:
    OpcUa::UaServer::ServicesRegistry::SharedPtr InternalServer;
    OpcUa::UaServer::TcpServer::SharedPtr TcpServer;
    OpcUa::UaServer::OpcUaProtocol::SharedPtr Protocol;
    bool Debug;
  };

  void OpcUaProtocolAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
  {
    ApplyAddonParameters(params);
    const std::vector<OpcUa::UaServer::ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Debug);
    for (OpcUa::UaServer::ApplicationData d: applications) {
      std::cout << "Endpoint is: " << d.Endpoints.front().EndpointURL << std::endl;
    }

    std::vector<OpcUa::ApplicationDescription> applicationDescriptions;
    std::vector<OpcUa::EndpointDescription> endpointDescriptions;
    for (const OpcUa::UaServer::ApplicationData application : applications)
    {
      applicationDescriptions.push_back(application.Application);
      endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
    }
    OpcUa::UaServer::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::UaServer::EndpointsRegistry>(OpcUa::UaServer::EndpointsRegistryAddonID);
    if (!endpointsAddon)
    {
      std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
      return;
    }
    endpointsAddon->AddEndpoints(endpointDescriptions);
    endpointsAddon->AddApplications(applicationDescriptions);

    InternalServer = addons.GetAddon<OpcUa::UaServer::ServicesRegistry>(OpcUa::UaServer::ServicesRegistryAddonID);

    TcpServer = OpcUa::UaServer::CreateTcpServer();
    Protocol.reset(new OpcUaProtocol(*TcpServer, Debug));
    Protocol->StartEndpoints(endpointDescriptions, InternalServer->GetServer());
  }

  void OpcUaProtocolAddon::Stop()
  {
  }

  void OpcUaProtocolAddon::ApplyAddonParameters(const Common::AddonParameters& params)
  {
    for (const Common::Parameter parameter : params.Parameters)
    {
      if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
      {
        Debug = true;
        std::cout << "Enabled debug mode in the binary protocol addon." << std::endl;
      }
    }
  }

  void OpcUaProtocolAddon::PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const
  {
    OpcUa::UaServer::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::UaServer::EndpointsRegistry>(OpcUa::UaServer::EndpointsRegistryAddonID);
    if (!endpointsAddon)
    {
      std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
      return;
    }
    endpointsAddon->AddEndpoints(endpoints);
    endpointsAddon->AddApplications(applications);
  }

} // namespace


namespace OpcUa
{
  namespace UaServer
  {
    Common::Addon::UniquePtr OpcUaProtocolAddonFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new ::OpcUaProtocolAddon());
    }


    OpcUaProtocol::UniquePtr CreateOpcUaProtocol(TcpServer& tcpServer, bool debug)
    {
      return OpcUaProtocol::UniquePtr(new ::OpcUaProtocol(tcpServer, debug));
    }

  }
}
