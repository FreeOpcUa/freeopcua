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

#include "opc_tcp_processor.h"

#include <opc/ua/server/opc_tcp_async.h>

#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/channel.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/input_from_buffer.h>

#include <array>
#include <boost/asio.hpp>
#include <iostream>
#include <set>



namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

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


  class OpcTcpConnection;

  class OpcTcpServer : public OpcUa::Server::AsyncOpcTcp
  {
  public:
    DEFINE_CLASS_POINTERS(OpcTcpServer);

  public:
    OpcTcpServer(const AsyncOpcTcp::Parameters& params, Services::SharedPtr server, boost::asio::io_service& ioService);

    virtual void Listen() override;
    virtual void Shutdown() override;

  private:
    void Accept();

  private:// OpcTcpClient interface;
    friend class OpcTcpConnection;
    void RemoveClient(std::shared_ptr<OpcTcpConnection> client);

  private:
    Parameters Params;
    Services::SharedPtr Server;
    std::set<std::shared_ptr<OpcTcpConnection>> Clients;

    tcp::socket socket;
    tcp::acceptor acceptor;
  };


  class OpcTcpConnection : public std::enable_shared_from_this<OpcTcpConnection>, private OpcUa::OutputChannel
  {
  public:
    DEFINE_CLASS_POINTERS(OpcTcpConnection);

  public:
    OpcTcpConnection(tcp::socket socket, OpcTcpServer& tcpServer, Services::SharedPtr uaServer, bool debug);
    ~OpcTcpConnection();

    void Start();

    virtual void Stop()
    {
      Socket.close();
    }


  private:
    void ReadNextData();
    void ProcessHeader(const boost::system::error_code& error, std::size_t bytes_transferred);
    void ProcessMessage(OpcUa::Binary::MessageType type, const boost::system::error_code& error, std::size_t bytesTransferred);
    void GoodBye();

    std::size_t GetHeaderSize() const;

  private:
    virtual void Send(const char* message, std::size_t size);
    void FillResponseHeader(const RequestHeader& requestHeader, ResponseHeader& responseHeader) const;

  private:
    tcp::socket Socket;
    OpcTcpServer& TcpServer;
    Server::OpcTcpMessages MessageProcessor;
    OStreamBinary OStream;
    const bool Debug = false;
    std::vector<char> Buffer;
  };

  OpcTcpConnection::OpcTcpConnection(tcp::socket socket, OpcTcpServer& tcpServer, Services::SharedPtr uaServer, bool debug)
    : Socket(std::move(socket))
    , TcpServer(tcpServer)
    , MessageProcessor(uaServer, *this, debug)
    , OStream(*this)
    , Debug(debug)
    , Buffer(8192)
  {
  }

  OpcTcpConnection::~OpcTcpConnection()
  {
  }

  void OpcTcpConnection::Start()
  {
    ReadNextData();
  }

  void OpcTcpConnection::ReadNextData()
  {
    async_read(Socket, buffer(Buffer), transfer_exactly(GetHeaderSize()),
      [this](const boost::system::error_code& error, std::size_t bytes_transferred)
      {
        try
        {
          ProcessHeader(error, bytes_transferred);
        }
        catch (const std::exception& exc)
        {
          std::cerr << "opc_tcp_async| Failed to process message header: " << exc.what() << std::endl;
        }
      }
    );
  }

  std::size_t OpcTcpConnection::GetHeaderSize() const
  {
    return OpcUa::Binary::RawSize(OpcUa::Binary::Header());
  }

  void OpcTcpConnection::ProcessHeader(const boost::system::error_code& error, std::size_t bytes_transferred)
  {
    if (error)
    {
      std::cerr << "opc_tcp_async| Error during receiving message header: " << error.message() << std::endl;
      GoodBye();
      return;
    }

    if (Debug) std::cout << "opc_tcp_async| Received message header with size " << bytes_transferred << std::endl;

    OpcUa::InputFromBuffer messageChannel(&Buffer[0], bytes_transferred);
    IStreamBinary messageStream(messageChannel);
    OpcUa::Binary::Header header;
    messageStream >> header;

    const std::size_t messageSize = header.Size - GetHeaderSize();

    if (Debug)
    {
      std::cout << "opc_tcp_async| Message type: " << header.Type << std::endl;
      std::cout << "opc_tcp_async| Chunk type: " << header.Chunk << std::endl;
      std::cout << "opc_tcp_async| MessageSize: " << header.Size << std::endl;
      std::cout << "opc_tcp_async| Waiting " << messageSize << " bytes from client." << std::endl;
    }

    async_read(Socket, buffer(Buffer), transfer_exactly(messageSize),
        [this, header](const boost::system::error_code& error, std::size_t bytesTransferred)
        {
          if (error)
          {
            if (Debug) std::cerr << "opc_tcp_async| Error during receiving message body." << std::endl;
            return;
          }
          ProcessMessage(header.Type, error, bytesTransferred);
        }
    );

  }

  void OpcTcpConnection::ProcessMessage(OpcUa::Binary::MessageType type, const boost::system::error_code& error, std::size_t bytesTransferred)
  {
    if (error)
    {
      std::cerr << "opc_tcp_async| Error during receiving message body: " << error.message() << std::endl;
      GoodBye();
      return;
    }

    if (Debug)
    {
      if (Debug) std::cout << "opc_tcp_async| Received " << bytesTransferred << " bytes from client:" << std::endl;
      PrintBlob(Buffer, bytesTransferred);
    }

    // restrict server size code only with current message.
    OpcUa::InputFromBuffer messageChannel(&Buffer[0], bytesTransferred);
    IStreamBinary messageStream(messageChannel);

    bool cont = true;

    try
    {
      cont = MessageProcessor.ProcessMessage(type, messageStream);
    }
    catch(const std::exception& exc)
    {
      std::cerr << "opc_tcp_async| Failed to process message. " << exc.what() << std::endl;
      GoodBye();
      return;
    }

    if (messageChannel.GetRemainSize())
    {
      std::cerr << "opc_tcp_async| ERROR!!! Message from client has been processed partially." << std::endl;
    }

    if ( ! cont )
    {
      GoodBye();
      return;
    }

    ReadNextData();
  }


  void OpcTcpConnection::GoodBye()
  {
    TcpServer.RemoveClient(shared_from_this());
    if (Debug) std::cout << "opc_tcp_async| Good bye." << std::endl;
  }

  void OpcTcpConnection::Send(const char* message, std::size_t size)
  {
    std::shared_ptr<std::vector<char>> data = std::make_shared<std::vector<char>>(message, message + size);

    if (Debug)
    {
      std::cout << "opc_tcp_async| Sending next data to the client:" << std::endl;
      PrintBlob(*data);
    }

    async_write(Socket, buffer(&(*data)[0], data->size()), [this, data](const boost::system::error_code & err, size_t bytes){
      if (err)
      {
        std::cerr << "opc_tcp_async| Failed to send data to the client. " << err.message() << std::endl;
        GoodBye();
        return;
      }

      if (Debug)
      {
        std::cout << "opc_tcp_async| Response sent to the client." << std::endl;
      }
    });
  }

  OpcTcpServer::OpcTcpServer(const AsyncOpcTcp::Parameters& params, Services::SharedPtr server, boost::asio::io_service& ioService)
    : Params(params)
    , Server(server)
    , socket(ioService)
    , acceptor(ioService)
  {
    tcp::endpoint ep;
    if (params.Host.empty() )
    {
      ep = tcp::endpoint( tcp::v4(), params.Port );
    }
    else if ( params.Host == "localhost" )
    {
      ep = tcp::endpoint( ip::address::from_string("127.0.0.1"), params.Port );
    }
    else
    {
      ep = tcp::endpoint( ip::address::from_string(params.Host), params.Port );
    }
    acceptor.open(ep.protocol());
    acceptor.bind(ep);
  }

  void OpcTcpServer::Listen()
  {
    std::clog << "opc_tcp_async| Running server." << std::endl;
    Accept();
  }

  void OpcTcpServer::Shutdown()
  {
    std::clog << "opc_tcp_async| Shutdowning server." << std::endl;
    Clients.clear();
    acceptor.close();
  }

  void OpcTcpServer::Accept()
  {
    try
    {
      std::cout << "opc_tcp_async| Waiting for client connection at: " << acceptor.local_endpoint().address() << ":" << acceptor.local_endpoint().port() <<  std::endl;
      acceptor.listen();
      acceptor.async_accept(socket, [this](boost::system::error_code errorCode){
        if (!errorCode)
        {
          std::cout << "opc_tcp_async| Accepted new client connection." << std::endl;
          std::shared_ptr<OpcTcpConnection> connection = std::make_shared<OpcTcpConnection>(std::move(socket), *this, Server, Params.DebugMode);
          Clients.insert(connection);
          connection->Start();
        }
        else
        {
          std::cout << "opc_tcp_async| Error during client connection: "<< errorCode.message() << std::endl;
        }
        Accept();
      });
    }
    catch (const std::exception& exc)
    {
      std::cout << "opc_tcp_async| Error accepting client connection: "<< exc.what() << std::endl;
    }
  }

  void OpcTcpServer::RemoveClient(OpcTcpConnection::SharedPtr client)
  {
    Clients.erase(client);
  }

} // namespace

OpcUa::Server::AsyncOpcTcp::UniquePtr OpcUa::Server::CreateAsyncOpcTcp(const OpcUa::Server::AsyncOpcTcp::Parameters& params, Services::SharedPtr server, boost::asio::io_service& io)
{
  return AsyncOpcTcp::UniquePtr(new OpcTcpServer(params, server, io));
}
