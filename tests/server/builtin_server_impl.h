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

#include "builtin_server_addon.h"
#include "builtin_server.h"

#include <opc/ua/connection_listener.h>

#include <opc/common/addons_core/addon.h>
#include <opc/common/thread.h>
#include <opc/ua/client/binary_client.h>
#include <src/server/opcua_protocol.h>

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
  , public Server::TcpServer
  , public Server::BuiltinServer
  , private Common::ThreadObserver
{
public:
  BuiltinServerAddon(const Common::Logger::SharedPtr & logger = nullptr);
  ~BuiltinServerAddon() override;

  OpcUa::Services::SharedPtr GetServices() const override;

public: // Common::Addon
  virtual void Initialize(Common::AddonsManager & addons, const Common::AddonParameters & params) override;
  virtual void Stop() override;

public: // TcpServer
  virtual void Listen(const OpcUa::Server::TcpParameters & params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor) override;
  virtual void StopListen(const OpcUa::Server::TcpParameters & params) override;

private:
  virtual void OnSuccess() override;
  virtual void OnError(const std::exception & exc) override;

private:
  std::shared_ptr<BufferedInput> ClientInput;
  std::shared_ptr<BufferedInput> ServerInput;

  std::shared_ptr<OpcUa::IOChannel> ClientChannel;
  std::shared_ptr<OpcUa::IOChannel> ServerChannel;
  std::unique_ptr<Common::Thread> Thread;

  OpcUa::Server::OpcUaProtocol::SharedPtr Protocol;
};

} // namespace Impl
} // namespace OpcUa
