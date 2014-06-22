/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Addon for accessing opcua server inside process.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/server.h>
#include <opc/ua/server/addons/tcp_server.h>
#include <opc/ua/server/tcp_server.h>

namespace OpcUa
{
  namespace UaServer
  {

    class BuiltinServer : public Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(BuiltingServerFactory);

    public:
      virtual std::shared_ptr<OpcUa::Remote::Server> GetServer() const = 0;
    };


  } // namespace UaServer
} // namespace OpcUa
