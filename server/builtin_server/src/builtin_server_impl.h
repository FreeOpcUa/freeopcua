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

#include <opc/ua/connection_listener.h>

#include <opc/common/addons_core/addon.h>
#include <opc/common/thread.h>
#include <opc/ua/client/binary_computer.h>
#include <opc/ua/protocol/binary/secure_connection.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>


namespace OpcUa
{
  namespace Impl
  {

    class BufferedInput : public OpcUa::InputChannel
    {
    public:
      explicit BufferedInput(bool debug);
      virtual std::size_t Receive(char* data, std::size_t size);
      void AddBuffer(const char* buf, std::size_t size);
      void Stop();

    private:
      void ThrowIfStopped();

    private:
      std::vector<char> Buffer;
      std::atomic<bool> Running;
      std::mutex BufferMutex;
      std::condition_variable DataReady;
      bool Debug;
    };


    class BuiltinComputerAddon
      : public ::OpcUa::Server::BuiltinServerAddon
      , private Common::ThreadObserver
    {
    public:
      BuiltinComputerAddon();
      ~BuiltinComputerAddon();

      virtual std::shared_ptr<OpcUa::Remote::Computer> GetComputer() const;

    public: // Common::Addon
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

      virtual void Listen(const OpcUa::Server::TcpParameters&, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor);
      virtual void StopListen(const OpcUa::Server::TcpParameters&);

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
