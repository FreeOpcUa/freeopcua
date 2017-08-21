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

#include "opcua_protocol.h"
#include "tcp_server.h"

#include <opc/common/interface.h>
#include <opc/common/logger.h>
#include <opc/ua/server/services_registry.h>


namespace OpcUa
{
namespace Server
{

class OpcUaProtocol : public Common::Interface
{
public:
  DEFINE_CLASS_POINTERS(OpcUaProtocol)

  virtual void StartEndpoints(const std::vector<EndpointDescription> & Endpoints, Services::SharedPtr server) = 0;
  virtual void StopEndpoints() = 0;
};

OpcUaProtocol::UniquePtr CreateOpcUaProtocol(TcpServer & tcpServer, const Common::Logger::SharedPtr & logger);

} // namespace UaServer
} // nmespace OpcUa
