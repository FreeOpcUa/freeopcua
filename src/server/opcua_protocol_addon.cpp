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

#include <opc/common/logger.h>
#include <opc/common/uri_facade.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/input_from_buffer.h>
#include <opc/ua/protocol/utils.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>

#include <stdexcept>


namespace
{

using namespace OpcUa;
using namespace OpcUa::Binary;
using namespace OpcUa::Server;

class OpcTcp : public OpcUa::Server::IncomingConnectionProcessor
{
public:
  OpcTcp(OpcUa::Services::SharedPtr services, const Common::Logger::SharedPtr & logger)
    : Server(services)
    , Logger(logger)
  {
  }

  virtual void Process(OpcUa::IOChannel::SharedPtr clientChannel)
  {
    if (!clientChannel)
      {
        LOG_WARN(Logger, "opc_tcp_processor| empty channel passed to endpoints opc binary protocol processor");

        return;
      }

    LOG_DEBUG(Logger, "opc_tcp_processor| Hello client!");

    std::shared_ptr<OpcTcpMessages> messageProcessor = std::make_shared<OpcTcpMessages>(Server, clientChannel, Logger);

    for (;;)
      {
        ProcessData(*clientChannel, *messageProcessor);
      }
  }

  virtual void StopProcessing(OpcUa::IOChannel::SharedPtr clientChannel)
  {
  }

private:
  void ProcessData(OpcUa::IOChannel & clientChannel, OpcUa::Server::OpcTcpMessages & messageProcessor)
  {
    using namespace OpcUa::Binary;

    IStreamBinary iStream(clientChannel);
    ProcessChunk(iStream, messageProcessor);
  }

  // TODO implement collecting full message from chunks before processing.
  void ProcessChunk(IStreamBinary & iStream, OpcTcpMessages & messageProcessor)
  {
    LOG_DEBUG(Logger, "opc_tcp_processor| processing new chunk");

    Header hdr;
    // Receive message header.
    iStream >> hdr;

    // Receive full message.
    std::vector<char> buffer(hdr.MessageSize());
    OpcUa::Binary::RawBuffer buf(&buffer[0], buffer.size());
    iStream >> buf;

    LOG_DEBUG(Logger, "opc_tcp_processor| received message:\n{}", ToHexDump(buffer));

    // restrict server size code only with current message.
    OpcUa::InputFromBuffer messageChannel(&buffer[0], buffer.size());
    IStreamBinary messageStream(messageChannel);
    messageProcessor.ProcessMessage(hdr.Type, messageStream);

    if (messageChannel.GetRemainSize())
      {
        LOG_ERROR(Logger, "opc_tcp_processor| message has not been processed completely");
      }
  }

private:
  OpcUa::Services::SharedPtr Server;
  Common::Logger::SharedPtr Logger;
};

class OpcUaProtocol : public OpcUa::Server::OpcUaProtocol
{
public:
  DEFINE_CLASS_POINTERS(OpcUaProtocol)

public:
  OpcUaProtocol(OpcUa::Server::TcpServer & tcpServer, const Common::Logger::SharedPtr & logger)
    : TcpAddon(tcpServer)
    , Logger(logger)
  {
  }

  virtual void StartEndpoints(const std::vector<EndpointDescription> & endpoints, OpcUa::Services::SharedPtr server) override
  {
    for (const EndpointDescription endpoint : endpoints)
      {
        const Common::Uri uri(endpoint.EndpointUrl);

        if (uri.Scheme() == "opc.tcp")
          {
            std::shared_ptr<IncomingConnectionProcessor> processor(new OpcTcp(server, Logger));
            TcpParameters tcpParams;
            tcpParams.Port = uri.Port();

            LOG_INFO(Logger, "opc_tcp_processor| start to listen on port {}", tcpParams.Port);

            TcpAddon.Listen(tcpParams, processor);
            Ports.push_back(tcpParams);
          }
      }
  }

  virtual void StopEndpoints() override
  {
    for (const TcpParameters & params : Ports)
      {
        TcpAddon.StopListen(params);
      }
  }

private:
  OpcUa::Server::TcpServer & TcpAddon;
  std::vector<TcpParameters> Ports;
  Common::Logger::SharedPtr Logger;
};


class OpcUaProtocolAddon : public Common::Addon
{
public:
  OpcUaProtocolAddon()
  {
  }

public: // Common::Addon
  virtual void Initialize(Common::AddonsManager & addons, const Common::AddonParameters & params) override;
  virtual void Stop() override;

private:
  void ApplyAddonParameters(const Common::AddonParameters & params);
  // not used
  // void PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const;

private:
  OpcUa::Server::ServicesRegistry::SharedPtr InternalServer;
  OpcUa::Server::TcpServer::SharedPtr TcpServer;
  OpcUa::Server::OpcUaProtocol::SharedPtr Protocol;
  Common::Logger::SharedPtr Logger;
};

void OpcUaProtocolAddon::Initialize(Common::AddonsManager & addons, const Common::AddonParameters & params)
{
  Logger = addons.GetLogger();
  ApplyAddonParameters(params);
  const std::vector<OpcUa::Server::ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Logger);

  for (OpcUa::Server::ApplicationData d : applications)
    {
      LOG_INFO(Logger, "endpoint is: {}", d.Endpoints.front().EndpointUrl);
    }

  std::vector<OpcUa::ApplicationDescription> applicationDescriptions;
  std::vector<OpcUa::EndpointDescription> endpointDescriptions;

  for (const OpcUa::Server::ApplicationData application : applications)
    {
      applicationDescriptions.push_back(application.Application);
      endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
    }

  OpcUa::Server::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::Server::EndpointsRegistry>(OpcUa::Server::EndpointsRegistryAddonId);

  if (!endpointsAddon)
    {
      LOG_ERROR(Logger, "cannot store endpoints information, endpoints service addon has not been registered");
      return;
    }

  endpointsAddon->AddEndpoints(endpointDescriptions);
  endpointsAddon->AddApplications(applicationDescriptions);

  InternalServer = addons.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);

  TcpServer = OpcUa::Server::CreateTcpServer(Logger);
  Protocol.reset(new OpcUaProtocol(*TcpServer, Logger));
  Protocol->StartEndpoints(endpointDescriptions, InternalServer->GetServer());
}

void OpcUaProtocolAddon::Stop()
{
  Protocol.reset();
  TcpServer.reset();
  InternalServer.reset();
}

void OpcUaProtocolAddon::ApplyAddonParameters(const Common::AddonParameters & params)
{
  /*
  for (const Common::Parameter parameter : params.Parameters)
    {
      if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
        {
          Debug = true;
          std::cout << "Enabled debug mode in the binary protocol addon." << std::endl;
        }
    }
   */
}

// not used
//void OpcUaProtocolAddon::PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const
//{
//  OpcUa::Server::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::Server::EndpointsRegistry>(OpcUa::Server::EndpointsRegistryAddonId);
//  if (!endpointsAddon)
//  {
//    std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
//    return;
//  }
//  endpointsAddon->AddEndpoints(endpoints);
//  endpointsAddon->AddApplications(applications);
//}

} // namespace


namespace OpcUa
{
namespace Server
{
Common::Addon::UniquePtr OpcUaProtocolAddonFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new ::OpcUaProtocolAddon());
}


OpcUaProtocol::UniquePtr CreateOpcUaProtocol(TcpServer & tcpServer, const Common::Logger::SharedPtr & logger)
{
  return OpcUaProtocol::UniquePtr(new ::OpcUaProtocol(tcpServer, logger));
}

}
}
