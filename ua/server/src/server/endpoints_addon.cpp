/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/endpoints.h>

#include <opccore/common/addons_core/addon_manager.h>
#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/server/addons/tcp_server_addon.h>

#include <iostream>

namespace
{

  using namespace OpcUa::Server;

  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      if (!clientChannel)
      {
        std::cerr << "Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      OpcUa::Binary::IOStream stream(clientChannel);
      std::clog << "Hello client!" << std::endl;
      Hello(stream);
      std::cout << "Creating secure channel with client." << std::endl;
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }

  private:
    void Hello(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;
      std::cout << "Reading header." << std::endl;
      Header hdr;
      stream >> hdr;
      if (hdr.Type != MT_HELLO || hdr.Chunk != CHT_SINGLE)
      {
        std::cout << "Invalid header received." << std::endl;
        return;
      }

      std::cout << "Reading hello message." << std::endl;
      OpcUa::Binary::Hello hello;
      stream >> hello;

      OpcUa::Binary::Acknowledge ack;
      ack.ReceiveBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.SendBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxMessageSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxChunkCount = 1;

      OpcUa::Binary::Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      std::cout << "Sending answer to client." << std::endl;
      stream << ackHeader << ack << flush; 
    }
  };


  class Endpoints : public EndpointsAddon
  {
  public:
    Endpoints()
    {
    }

    std::vector<Endpoint> GetEndpoints() const
    {
      return std::vector<Endpoint>();
    }

    std::shared_ptr<IncomingConnectionProcessor> GetProcessor() const
    {
      return std::shared_ptr<IncomingConnectionProcessor>(new OpcTcp());
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons)
    {
    }

    virtual void Stop()
    {
    }

  private:
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::Endpoints());
}

