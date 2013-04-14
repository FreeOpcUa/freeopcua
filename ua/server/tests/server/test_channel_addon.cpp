/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "test_channel_addon.h"

#include <thread>

namespace
{

  void Process(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor, std::shared_ptr<OpcUa::IOChannel> channel)
  {
    processor->Process(channel);
  }


  class TestNetworkAddon : public OpcUa::Tests::TestNetworkAddon
  {
  public:  // TcpServerAddon
    virtual void Listen(const OpcUa::Server::TcpParameters& params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor)
    {
      Processor = processor;
    }

    virtual void StopListen(const OpcUa::Server::TcpParameters& tcpParams)
    {
      Processor.reset();
    }

  public: // TestNetworkAddon
    void Accept(std::shared_ptr<OpcUa::IOChannel> client)
    {
      Thread.reset(new std::thread(std::bind(Process, Processor, client)));
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& addons)
    {
    }

    virtual void Stop()
    {
      if (Thread.get())
      {
        Thread->join();
      }
    }


  private:
    std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> Processor;
    std::unique_ptr<std::thread> Thread;
  };

}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::TestNetworkAddon());
}

