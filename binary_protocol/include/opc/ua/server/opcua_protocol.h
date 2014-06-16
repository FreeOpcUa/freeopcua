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

#include <opc/common/interface.h>
#include <opc/ua/server/services_registry.h>
#include <opc/ua/server/tcp_server.h>


namespace OpcUa
{
  namespace UaServer
  {

    class OpcUaProtocol : public Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(OpcUaProtocol);

      virtual void StartEndpoints(const std::vector<EndpointDescription>& Endpoints, Remote::Server::SharedPtr server) = 0;
      virtual void StopEndpoints() = 0;
    };

    OpcUaProtocol::UniquePtr CreateOpcUaProtocol(TcpServer::SharedPtr tcpServer, bool debug = false);

  } // namespace UaServer
} // nmespace OpcUa
