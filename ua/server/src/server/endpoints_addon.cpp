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

namespace
{

  using namespace OpcUa::Server;

  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      OpcUa::Binary::IOStream stream(clientChannel);
      Hello(stream);
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }

  private:
    void Hello(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      Header hdr;
      stream >> hdr;
      if (hdr.Type != MT_HELLO || hdr.Chunk != CHT_SINGLE)
      {
        return;
      }

      OpcUa::Binary::Hello hello;
      stream >> hello;

      OpcUa::Binary::Acknowledge ack;
      ack.ReceiveBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.SendBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxMessageSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxChunkCount = 1;

      OpcUa::Binary::Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      stream << ackHeader << ack << flush; 
    }
  };


  class Endpoints : public EndpointsAddon
  {
  public:
    Endpoints()
       : OpcTcpProtocol(new OpcTcp())
    {
    }

    virtual std::vector<Endpoint> GetEndpoints() const
    {
      return std::vector<Endpoint>();
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons)
    {
      TcpAddon = Common::GetAddon<OpcUa::Server::TcpServerAddon>(addons, OpcUa::Server::TcpServerAddonID);
      OpcUa::Server::TcpParameters tcpParams;
      tcpParams.Port = 4841;
      TcpAddon->Listen(tcpParams, OpcTcpProtocol);
    }

    virtual void Stop()
    {
      TcpParameters tcpParams;
      tcpParams.Port = 4841;
      TcpAddon->StopListen(tcpParams);
      TcpAddon.reset();
    }

  private:
    std::shared_ptr<IncomingConnectionProcessor> OpcTcpProtocol;
    std::shared_ptr<TcpServerAddon> TcpAddon;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::Endpoints());
}

