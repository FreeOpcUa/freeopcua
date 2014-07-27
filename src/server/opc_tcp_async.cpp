/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include <opc/ua/server/opc_tcp_async.h>

#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/channel.h>
#include <opc/ua/protocol/input_from_buffer.h>

#include <array>
#include <boost/asio.hpp>
#include <iostream>



namespace
{

  void PrintBlob(const std::vector<char>& buf, std::size_t size)
  {
    size = std::min(size, buf.size());
    unsigned pos = 0;
    std::cout << "Data size: " << size << std::endl;
    while (pos < size)
    {
      if (pos)
        printf((pos % 16 == 0) ? "\n" : " ");

      const char letter = buf[pos];
      printf("%02x", (unsigned)letter & 0x000000FF);

      if (letter > ' ')
        std::cout << "(" << letter << ")";
      else
        std::cout << "   ";

      ++pos;
    }

    std::cout << std::endl << std::flush;
  }

  inline void PrintBlob(const std::vector<char>& buf)
  {
    PrintBlob(buf, buf.size());
  }

  using namespace OpcUa;
  using namespace boost::asio;  
  using namespace boost::asio::ip;  

  typedef OpcUa::Binary::IOStream<OpcUa::IOChannel> IOStreamBinary;
  typedef OpcUa::Binary::IStream<OpcUa::InputChannel> IStreamBinary;
  typedef OpcUa::Binary::OStream<OpcUa::OutputChannel> OStreamBinary;


  class OpcTcpClient;

  class OpcTcpServer : public OpcUa::UaServer::AsyncOpcTcp
  {
  public:
    DEFINE_CLASS_POINTERS(OpcTcpServer);

  public:
    OpcTcpServer(const AsyncOpcTcp::Parameters& params, Remote::Server::SharedPtr server);

    virtual void Listen() override;
    virtual void Shutdown() override;

  private:
    void Accept();

  private:// OpcTcpClient interface;
    friend class OpcTcpClient;
    void RemoveClient(std::shared_ptr<OpcTcpClient> client);

  private:
    Parameters Params;
    Remote::Server::SharedPtr Server;
    std::vector<std::shared_ptr<OpcTcpClient>> Clients;

    io_service io;
    tcp::acceptor acceptor;
    tcp::socket socket;
  };


  class OpcTcpClient : public std::enable_shared_from_this<OpcTcpClient>, private OpcUa::OutputChannel
  {
  public:
    DEFINE_CLASS_POINTERS(OpcTcpClient);

  public:
    OpcTcpClient(tcp::socket socket, OpcTcpServer& server, bool debug);
    ~OpcTcpClient();

  private:
    void Start();
    void ProcessHeader(const boost::system::error_code& error, std::size_t bytes_transferred);
    void ProcessMessage(OpcUa::Binary::MessageType type, const boost::system::error_code& error, std::size_t bytes_transferred);
    void HelloClient(IStreamBinary& istream);
    void GoodBye();

    std::size_t GetHeaderSize() const;

  private:
    virtual void Send(const char* message, std::size_t size);

  private:
    tcp::socket Socket;
    OpcTcpServer& Server;
    OStreamBinary OStream;
    const bool Debug;
    std::vector<char> Buffer;
  };

  OpcTcpClient::OpcTcpClient(tcp::socket socket, OpcTcpServer& server, bool debug)
    : Socket(std::move(socket))
    , Server(server)
    , OStream(*this)
    , Debug(debug)
    , Buffer(8192)
  {
    Start();
  }

  OpcTcpClient::~OpcTcpClient()
  {
  }

  void OpcTcpClient::Start()
  {
    async_read(Socket, buffer(Buffer), transfer_exactly(GetHeaderSize()),
      [this](const boost::system::error_code& error, std::size_t bytes_transferred)
      {
        ProcessHeader(error, bytes_transferred);
      }
    );
  }

  std::size_t OpcTcpClient::GetHeaderSize() const
  {
    return OpcUa::Binary::RawSize(OpcUa::Binary::Header());
  }

  void OpcTcpClient::ProcessHeader(const boost::system::error_code& error, std::size_t bytes_transferred)
  {
    if (error)
    {
      std::cerr << "Error during receiving message header: " << error.message() << std::endl;
      GoodBye();
      return;
    }

    if (Debug) std::cout << "Received message header with size " << bytes_transferred << std::endl;

    OpcUa::InputFromBuffer messageChannel(&Buffer[0], bytes_transferred);
    IStreamBinary messageStream(messageChannel);
    OpcUa::Binary::Header header;
    messageStream >> header;

    if (Debug)
    {
      std::cout << "Message type: " << header.Type << std::endl;
      std::cout << "Chunk type: " << header.Chunk << std::endl;
      std::cout << "MessageSize: " << header.Size << std::endl;
    }

    async_read(Socket, buffer(Buffer), transfer_exactly(header.Size - GetHeaderSize()),
        [this, &header](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
          ProcessMessage(header.Type, error, bytes_transferred);
        }
    );

  }

  void OpcTcpClient::ProcessMessage(OpcUa::Binary::MessageType type, const boost::system::error_code& error, std::size_t bytes_transferred)
  {
    if (error)
    {
      std::cerr << "Error during receiving message body: " << error.message() << std::endl;
      GoodBye();
      return;
    }

    if (Debug)
    {
      std::cout << "Received new data from client." << std::endl;
      PrintBlob(Buffer, bytes_transferred);
    }

    // restrict server size code only with current message.
    OpcUa::InputFromBuffer messageChannel(&Buffer[0], bytes_transferred);
    IStreamBinary messageStream(messageChannel);

    switch (type)
    {
      case OpcUa::Binary::MessageType::MT_HELLO:
      {
        if (Debug) std::clog << "Accepted hello message." << std::endl;
        HelloClient(messageStream);
        break;
      }

      case OpcUa::Binary::MessageType::MT_SECURE_OPEN:
      {
        if (Debug) std::clog << "Opening securechannel." << std::endl;
        OpenChannel(messageStream, oStream);
        break;
      }
/*
      case OpcUa::Binary::MessageType::MT_SECURE_CLOSE:
      {
        if (Debug) std::clog << "Closing secure channel." << std::endl;
        CloseChannel(messageStream);
        return false;
      }

      case OpcUa::Binary::MessageType::MT_SECURE_MESSAGE:
      {
        ProcessMessage(messageStream, oStream);
        break;
      }

      case OpcUa::Binary::MessageType::MT_ACKNOWLEDGE:
      {
        if (Debug) std::clog << "Received acknowledge from client. This should not have happend..." << std::endl;
        throw std::logic_error("Thank to client about acknowledge.");
      }
      case OpcUa::Binary::MessageType::MT_ERROR:
      {
        if (Debug) std::clog << "There is an error happend in the client!" << std::endl;
        throw std::logic_error("It is very nice get to know server about error in the client.");
      }
*/
      default:
      {
        if (Debug) std::cerr << "Unknown message type '" << type << "' received!" << std::endl;
        GoodBye();
        return;
      }
    }

    if (messageChannel.GetRemainSize())
    {
      std::cerr << "ERROR!!! Message from client has been processed partially." << std::endl;
    }

    Start();
  }

  void OpcTcpClient::HelloClient(IStreamBinary& istream)
  {
    using namespace OpcUa::Binary;

    if (Debug) std::clog << "Reading hello message." << std::endl;
    Hello hello;
    istream >> hello;

    Acknowledge ack;
    ack.ReceiveBufferSize = hello.ReceiveBufferSize;
    ack.SendBufferSize = hello.SendBufferSize;
    ack.MaxMessageSize = hello.MaxMessageSize;
    ack.MaxChunkCount = 1;

    Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
    ackHeader.AddSize(RawSize(ack));
    if (Debug) std::clog << "Sending answer to client." << std::endl;

    OStream << ackHeader << ack << flush;
  }

  void OpenChannel(IStreamBinary& istream)
  {
    uint32_t channelID = 0;
    istream >> channelID;
    OpcUa::Binary::AsymmetricAlgorithmHeader algorithmHeader;
    istream >> algorithmHeader;

    if (algorithmHeader.SecurityPolicyURI != "http://opcfoundation.org/UA/SecurityPolicy#None")
    {
      throw std::logic_error(std::string("Client want to create secure channel with unsupported policy '") + algorithmHeader.SecurityPolicyURI + std::string("'"));
    }

    OpcUa::Binary::SequenceHeader sequence;
    istream >> sequence;

    OpenSecureChannelRequest request;
    istream >> request;

    if (request.SecurityMode != MSM_NONE)
    {
      throw std::logic_error("Unsupported security mode.");
    }

    if (request.RequestType == STR_RENEW)
    {
      //FIXME:Should check that channel has been issued first
      ++TokenID;
    }

    OpenSecureChannelResponse response;
    FillResponseHeader(request.Header, response.Header);
    response.ChannelSecurityToken.SecureChannelID = ChannelID;
    response.ChannelSecurityToken.TokenID = TokenID;
    response.ChannelSecurityToken.CreatedAt = OpcUa::CurrentDateTime();
    response.ChannelSecurityToken.RevisedLifetime = request.RequestLifeTime;

    OpcUa::Binary::SecureHeader responseHeader(MT_SECURE_OPEN, CHT_SINGLE, ChannelID);
    responseHeader.AddSize(RawSize(algorithmHeader));
    responseHeader.AddSize(RawSize(sequence));
    responseHeader.AddSize(RawSize(response));
    OStream << responseHeader << algorithmHeader << sequence << response << flush;
  }

  void OpcTcpClient::GoodBye()
  {
    if (Debug) std::cout << "Good bye." << std::endl;
    Socket.close();
    Server.RemoveClient(shared_from_this());
  }

  void OpcTcpClient::Send(const char* message, std::size_t size)
  {
    std::shared_ptr<std::vector<char>> data = std::make_shared<std::vector<char>>(message, message + size);

    if (Debug)
    {
      std::cout << "Senfing to client the next data:" << std::endl;
      PrintBlob(*data);
    }

    async_write(Socket, buffer(&(*data)[0], data->size()), [this, data](const boost::system::error_code & err, size_t bytes){
      if (err)
      {
        std::cerr << "Failed to send data to the client. " << err.message() << std::endl;
        GoodBye();
        return;
      }

      if (Debug)
      {
        std::cout << "Response sent to the client." << std::endl;
      }
    });
  }

  OpcTcpServer::OpcTcpServer(const AsyncOpcTcp::Parameters& params, Remote::Server::SharedPtr server)
    : Params(params)
    , Server(server)
    , io(params.ThreadsNumber)
    , socket(io)
    , acceptor(io, tcp::endpoint(tcp::v4(), params.Port))
  {
  }

  void OpcTcpServer::Listen()
  {
    std::clog << "Running server." << std::endl;
    Accept();
    io.run();
    std::clog << "Server stopped." << std::endl;
  }

  void OpcTcpServer::Shutdown()
  {
    std::clog << "Shutdowning server." << std::endl;
    io.stop();
  }

  void OpcTcpServer::Accept()
  {
    acceptor.async_accept(socket, [this](boost::system::error_code errorCode){
      std::cout << "Accepted new client connection." << std::endl;
      if (!errorCode)
      {
        Clients.emplace_back(std::make_shared<OpcTcpClient>(std::move(socket), *this, Params.DebugMode));
        Accept();
      }
    });
  }

  void OpcTcpServer::RemoveClient(OpcTcpClient::SharedPtr client)
  {
    auto clientIt = std::find(Clients.begin(), Clients.end(), client);
    if (clientIt == Clients.end())
    {
      std::cout << "Internal error: failed to find client for removing." << std::endl;
      return;
    }

    if (Params.DebugMode) std::cout << "Client disconnected." << std::endl;
    Clients.erase(clientIt);
  }

} // namespace

OpcUa::UaServer::AsyncOpcTcp::UniquePtr OpcUa::UaServer::CreateAsyncOpcTcp(const OpcUa::UaServer::AsyncOpcTcp::Parameters& params, Remote::Server::SharedPtr server)
{
  return AsyncOpcTcp::UniquePtr(new OpcTcpServer(params, server));
}

