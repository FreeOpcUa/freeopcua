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
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/server/addons/tcp_server_addon.h>

#include <iostream>
#include <stdexcept>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using namespace OpcUa::Server;


  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    OpcTcp()
      : ChannelID(1)
      , TokenID(2)
    {
    }

    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      if (!clientChannel)
      {
        std::cerr << "Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      std::clog << "Hello client!" << std::endl;
      OpcUa::Binary::IOStream stream(clientChannel);
      while(ProcessChunk(stream));
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }

  private:
    bool ProcessChunk(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      Header hdr;
      stream >> hdr;
      switch (hdr.Type)
      {
        case MT_HELLO:
        {
          HelloClient(stream);
          break;
        }


        case MT_SECURE_OPEN:
        {
          OpenChannel(stream);
          break;
        }

        case MT_SECURE_CLOSE:
        {
          CloseChannel(stream);
          return false;
        }

        case MT_SECURE_MESSAGE:
        {
          break;
        }

        case MT_ACKNOWLEDGE:
        {
          throw std::logic_error("Thank to client about acknowledge.");
        }
        case MT_ERROR:
        {
          throw std::logic_error("It is very nice get to know server about error in the client.");
        }
        default:
        {
          throw std::logic_error("Invalid message type received.");
        }
      }
      return true;
    }

    void HelloClient(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      std::cout << "Reading hello message." << std::endl;
      Hello hello;
      stream >> hello;

      Acknowledge ack;
      ack.ReceiveBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.SendBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxMessageSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxChunkCount = 1;

      Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      std::cout << "Sending answer to client." << std::endl;
      stream << ackHeader << ack << flush; 
    }

    void OpenChannel(IOStream& stream)
    {
      std::cout << "Creating secure channel with client." << std::endl;
      uint32_t channelID = 0;
      stream >> channelID;
      AsymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      if (algorithmHeader.SecurityPolicyURI != "http://opcfoundation.org/UA/SecurityPolicy#None")
      {
        throw std::logic_error(std::string("Client want to create secure channel with of invalid policy '") + algorithmHeader.SecurityPolicyURI + std::string("'"));
      }

      SequenceHeader sequence;
      stream >> sequence;

      OpenSecureChannelRequest openChannel;
      stream >> openChannel;

      if (openChannel.RequestType != STR_ISSUE)
      {
        throw std::logic_error("Client have to create secure channel!");
      }

      if (openChannel.SecurityMode != MSM_NONE)
      {
        throw std::logic_error("Unsupported security mode.");
      }

      OpenSecureChannelResponse response;
      response.Header.InnerDiagnostics.push_back(DiagnosticInfo());
      response.ChannelSecurityToken.SecureChannelID = ChannelID;
      response.ChannelSecurityToken.TokenID = TokenID;
      response.ChannelSecurityToken.CreatedAt = OpcUa::CurrentDateTime();
      response.ChannelSecurityToken.RevisedLifetime = 3600;


      SecureHeader responseHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
      responseHeader.AddSize(RawSize(algorithmHeader));
      responseHeader.AddSize(RawSize(sequence));
      responseHeader.AddSize(RawSize(response));
      stream << responseHeader << algorithmHeader << sequence << response << flush;
    }

    void CloseChannel(IOStream& stream)
    {
      std::cout << "Closing secure channel with client." << std::endl;
      uint32_t channelID = 0;
      stream >> channelID;
     
      SymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      SequenceHeader sequence;
      stream >> sequence;

      CloseSecureChannelRequest request;
      stream >> request;
    }

  private:
    uint32_t ChannelID;
    uint32_t TokenID;
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

