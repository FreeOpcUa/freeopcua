/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/server/addons/builtin_server.h>
#include <opc/ua/server/builtin_server.h>

#include <opc/ua/connection_listener.h>

#include <opc/common/addons_core/addon.h>
#include <opc/common/thread.h>
#include <opc/ua/client/binary_server.h>
#include <opc/ua/protocol/binary/secure_connection.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>


namespace OpcUa
{
  namespace Impl
  {
    class BufferedInput;

    class BuiltinServerAddon
      : public Common::Addon
      , public UaServer::TcpServer
      , public UaServer::BuiltinServer
      , private Common::ThreadObserver
    {
    public:
      BuiltinServerAddon();
      ~BuiltinServerAddon();

      virtual std::shared_ptr<OpcUa::Remote::Server> GetServer() const override;

    public: // Common::Addon
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    public: // TcpServer
      virtual void Listen(const OpcUa::UaServer::TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor);
      virtual void StopListen(const OpcUa::UaServer::TcpParameters& params);

    private:
      virtual void OnSuccess();
      virtual void OnError(const std::exception& exc);

    private:
      std::shared_ptr<BufferedInput> ClientInput;
      std::shared_ptr<BufferedInput> ServerInput;

      std::shared_ptr<OpcUa::IOChannel> ClientChannel;
      std::shared_ptr<OpcUa::IOChannel> ServerChannel;
      std::unique_ptr<Common::Thread> Thread;
      bool Debug;
    };

  } // namespace Impl
} // namespace OpcUa
