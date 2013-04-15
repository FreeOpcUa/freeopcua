/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/common/addons_core/addon.h>
#include <opc/ua/client/binary_computer.h>
#include <opc/ua/server/addons/builtin_computer.h>
#include <opc/ua/server/addons/endpoints.h>
#include <opc/ua/server/server.h>

#include <internal/thread.h>

#include <thread>

namespace
{

  class ClientChannel : public OpcUa::IOChannel
  {
  public:
    virtual std::size_t Receive(char* data, std::size_t size)
    {
      return 0;
    }

    virtual void Send(const char* message, std::size_t size)
    {
    }
  };


  void Process(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor, std::shared_ptr<OpcUa::IOChannel> channel)
  {
    processor->Process(channel);
  }


  class BuiltinComputerAddon : public OpcUa::Server::BuiltinComputerAddon
  {
  public:
    virtual std::shared_ptr<OpcUa::Remote::Computer> GetComputer() const
    {
      return OpcUa::Remote::CreateBinaryComputer(Channel);
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& addons)
    {
      Channel = std::shared_ptr<OpcUa::IOChannel>(new ClientChannel());
      std::shared_ptr<OpcUa::Server::EndpointsAddon> endpoints = Common::GetAddon<OpcUa::Server::EndpointsAddon>(addons, OpcUa::Server::EndpointsAddonID);
      std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor = endpoints->GetProcessor();
      Thread.reset(new OpcUa::Internal::Thread(std::bind(Process, processor, Channel)));
    }

    virtual void Stop()
    {
      if (Thread.get())
      {
        Thread->Join();
        Thread.reset();
      }
    }

  private:
    std::shared_ptr<OpcUa::IOChannel> Channel;
    std::unique_ptr<OpcUa::Internal::Thread> Thread;
  };

}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::BuiltinComputerAddon());
}

