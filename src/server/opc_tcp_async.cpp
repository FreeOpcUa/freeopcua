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

#include <opc/ua/protocol/utils.h>
#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/channel.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/input_from_buffer.h>

#include <array>
#include <boost/asio.hpp>
#include <future>
#include <iostream>
#include <set>



namespace
{

using namespace OpcUa;
using namespace OpcUa::Binary;
using namespace OpcUa;

using namespace boost::asio;
using namespace boost::asio::ip;


class OpcTcpConnection;

class OpcTcpServer : public OpcUa::Server::AsyncOpcTcp
{
public:
  DEFINE_CLASS_POINTERS(OpcTcpServer)

public:
  OpcTcpServer(const AsyncOpcTcp::Parameters & params, Services::SharedPtr server, boost::asio::io_service & ioService, const Common::Logger::SharedPtr & logger);

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
  Common::Logger::SharedPtr Logger;
  std::mutex Mutex;
  std::set<std::shared_ptr<OpcTcpConnection>> Clients;

  tcp::socket socket;
  tcp::acceptor acceptor;
};


class OpcTcpConnection : public std::enable_shared_from_this<OpcTcpConnection>, public OpcUa::OutputChannel
{
public:
  DEFINE_CLASS_POINTERS(OpcTcpConnection)

public:
  // Even if this is a public constructor do not use it - use OpcTcpConnection::create().
  // This constructor is needed for make_shared() which is needed
  // to be able to use instances of OpcTcpConnection as
  // OpcTcpConnection::SharedPtr and OpcUa::OutputChannel::SharedPtr
  // at the same time.
  OpcTcpConnection(tcp::socket socket, OpcTcpServer & tcpServer, const Common::Logger::SharedPtr & logger);
  static SharedPtr create(tcp::socket socket, OpcTcpServer & tcpServer, Services::SharedPtr uaServer, const Common::Logger::SharedPtr & logger);
  ~OpcTcpConnection();

  void Start();

  virtual void Stop()
  {
    Socket.close();

    /* queue a dummy operation to io_service to make sure we do not return
     * until all existing async io requests of this instance are actually
     * processed
     */
    typedef std::promise<void> Promise;
    Promise promise;
#if BOOST_VERSION < 107000
    Socket.get_io_service().post(bind(&Promise::set_value, &promise));
#else
    post(Socket.get_executor(), bind(&Promise::set_value, &promise));
#endif
    promise.get_future().wait();
  }


private:
  void ReadNextData();
  void ProcessHeader(const boost::system::error_code & error, std::size_t bytes_transferred);
  void ProcessMessage(OpcUa::Binary::MessageType type, const boost::system::error_code & error, std::size_t bytesTransferred);
  void GoodBye();

  std::size_t GetHeaderSize() const;

private:
  virtual void Send(const char * message, std::size_t size);
  void FillResponseHeader(const RequestHeader & requestHeader, ResponseHeader & responseHeader) const;

private:
  tcp::socket Socket;
  OpcTcpServer & TcpServer;
  Server::OpcTcpMessages::SharedPtr MessageProcessor;
  OStreamBinary OStream;
  Common::Logger::SharedPtr Logger;
  std::vector<char> Buffer;
};

OpcTcpConnection::OpcTcpConnection(tcp::socket socket, OpcTcpServer & tcpServer, const Common::Logger::SharedPtr & logger)
  : Socket(std::move(socket))
  , TcpServer(tcpServer)
  , OStream(*this)
  , Logger(logger)
  , Buffer(8192)
{
}

OpcTcpConnection::SharedPtr OpcTcpConnection::create(tcp::socket socket, OpcTcpServer & tcpServer, Services::SharedPtr uaServer, const Common::Logger::SharedPtr & logger)
{
  SharedPtr result = std::make_shared<OpcTcpConnection>(std::move(socket), tcpServer, logger);

  // you must not take a shared_ptr in a constructor
  // to give OpcTcpConnection as a shared_ptr to MessageProcessor
  // we have to add this helper function
  result->MessageProcessor = std::make_shared<Server::OpcTcpMessages>(uaServer, result, logger);
  return result;
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
  // do not lose reference to shared instance even if another
  // async operation decides to call GoodBye()
  OpcTcpConnection::SharedPtr self = shared_from_this();
  async_read(Socket, buffer(Buffer), transfer_exactly(GetHeaderSize()),
             [self](const boost::system::error_code & error, std::size_t bytes_transferred)
  {
    try
      {
        self->ProcessHeader(error, bytes_transferred);
      }

    catch (const std::exception & exc)
      {
        LOG_WARN(self->Logger, "opc_tcp_async         | failed to process message header: {}", exc.what());
      }
  }
            );
}

std::size_t OpcTcpConnection::GetHeaderSize() const
{
  return OpcUa::Binary::RawSize(OpcUa::Binary::Header());
}

void OpcTcpConnection::ProcessHeader(const boost::system::error_code & error, std::size_t bytes_transferred)
{
  if (error)
    {
      LOG_ERROR(Logger, "opc_tcp_async         | error receiving message header: {}", error.message());
      GoodBye();
      return;
    }

  LOG_DEBUG(Logger, "opc_tcp_async         | received message header with size: {}", bytes_transferred);

  OpcUa::InputFromBuffer messageChannel(&Buffer[0], bytes_transferred);
  IStreamBinary messageStream(messageChannel);
  OpcUa::Binary::Header header;
  messageStream >> header;

  const std::size_t messageSize = header.Size - GetHeaderSize();

  LOG_DEBUG(Logger, "opc_tcp_async         | received message: Type: {}, ChunkType: {}, Size: {}: DataSize: {}", header.Type, header.Chunk, header.Size, messageSize);

  // do not lose reference to shared instance even if another
  // async operation decides to call GoodBye()
  OpcTcpConnection::SharedPtr self = shared_from_this();
  async_read(Socket, buffer(Buffer), transfer_exactly(messageSize),
             [self, header](const boost::system::error_code & error, std::size_t bytesTransferred)
  {
    self->ProcessMessage(header.Type, error, bytesTransferred);
  }
            );

}

void OpcTcpConnection::ProcessMessage(OpcUa::Binary::MessageType type, const boost::system::error_code & error, std::size_t bytesTransferred)
{
  if (error)
    {
      LOG_ERROR(Logger, "opc_tcp_async         | error receiving message body: {}", error.message());
      GoodBye();
      return;
    }

  LOG_TRACE(Logger, "opc_tcp_async         | received message: {}", ToHexDump(Buffer, bytesTransferred));

  // restrict server size code only with current message.
  OpcUa::InputFromBuffer messageChannel(&Buffer[0], bytesTransferred);
  IStreamBinary messageStream(messageChannel);

  bool cont = true;

  try
    {
      cont = MessageProcessor->ProcessMessage(type, messageStream);
    }

  catch (const std::exception & exc)
    {
      LOG_ERROR(Logger, "opc_tcp_async         | failed to process message: {}", exc.what());
      GoodBye();
      return;
    }

  if (messageChannel.GetRemainSize())
    {
      std::cerr << "opc_tcp_async         | ERROR!!! Message from client has been processed partially." << std::endl;
    }

  if (!cont)
    {
      GoodBye();
      return;
    }

  ReadNextData();
}


void OpcTcpConnection::GoodBye()
{
  // reference to shared instance
  OpcTcpConnection::SharedPtr self = shared_from_this();
  TcpServer.RemoveClient(self);
  LOG_DEBUG(Logger, "opc_tcp_async         | good bye");
}

void OpcTcpConnection::Send(const char * message, std::size_t size)
{
  std::shared_ptr<std::vector<char>> data = std::make_shared<std::vector<char>>(message, message + size);

  LOG_TRACE(Logger, "opc_tcp_async         | send message: {}", ToHexDump(*data));

  // do not lose reference to shared instance even if another
  // async operation decides to call GoodBye()
  OpcTcpConnection::SharedPtr self = shared_from_this();
  async_write(Socket, buffer(&(*data)[0], data->size()), [self, data](const boost::system::error_code & err, size_t bytes)
  {
    if (err)
      {
        LOG_ERROR(self->Logger, "opc_tcp_async         | failed to send data: {}", err.message());
        self->GoodBye();
        return;
      }

    LOG_DEBUG(self->Logger, "opc_tcp_async         | response sent");
  });
}

OpcTcpServer::OpcTcpServer(const AsyncOpcTcp::Parameters & params, Services::SharedPtr server, boost::asio::io_service & ioService, const Common::Logger::SharedPtr & logger)
  : Params(params)
  , Server(server)
  , Logger(logger)
  , socket(ioService)
  , acceptor(ioService)
{
  tcp::endpoint ep;

  if (params.Host.empty())
    {
      ep = tcp::endpoint(tcp::v4(), params.Port);
    }

  else if (params.Host == "localhost")
    {
      ep = tcp::endpoint(ip::address::from_string("127.0.0.1"), params.Port);
    }

  else
    {
      ep = tcp::endpoint(ip::address::from_string(params.Host), params.Port);
    }

  acceptor.open(ep.protocol());
  acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor.bind(ep);
}

void OpcTcpServer::Listen()
{
  LOG_DEBUG(Logger, "opc_tcp_async         | running server");

  LOG_DEBUG(Logger, "opc_tcp_async         | waiting for client connection at: {}:{}", acceptor.local_endpoint().address(), acceptor.local_endpoint().port());
  acceptor.listen();

  Accept();
}

void OpcTcpServer::Shutdown()
{
  LOG_DEBUG(Logger, "opc_tcp_async         | shutting down server");
  acceptor.close();

  // Actively shutdown OpcTcpConnections to clear open async requests from worker
  // thread.
  // Warning: the Clients container may be modified by OpcTcpConnections::GoodBye
  // when calling Stop() which makes the iterator used in our for loop invalid.
  // So have a copy of this container to have a stable iterator.

  // guard copy operation
  typedef std::set<OpcTcpConnection::SharedPtr> OpcTcpConnectionSet;
  OpcTcpConnectionSet tmp;
  {
    std::unique_lock<std::mutex> lock(Mutex);
    tmp = OpcTcpConnectionSet(Clients);
  }

  // Unlock before client->Stop() because stop will interrupt all pending
  // async read/write operations, which may then call OpcTcpConnection::GoodBye(),
  // which needs access to OpcTcpServer::Clients. Otherwise we run into a deadlock
  // because stop waits for completion of pending operations.
  for (auto client : tmp)
    {
      client->Stop();
    }

  // clear possibly remaining Client's
  {
    std::unique_lock<std::mutex> lock(Mutex);
    Clients.clear();
  }

  /* queue a dummy operation to io_service to make sure we do not return
   * until all existing async io requests of this instance are actually
   * processed
   */
  typedef std::promise<void> Promise;
  Promise promise;
#if BOOST_VERSION < 107000
  acceptor.get_io_service().post(bind(&Promise::set_value, &promise));
#else
  post(acceptor.get_executor(), bind(&Promise::set_value, &promise));
#endif
  promise.get_future().wait();
}

void OpcTcpServer::Accept()
{
  try
    {
      acceptor.async_accept(socket, [this](boost::system::error_code errorCode)
      {
        if (!acceptor.is_open())
          {
            return;
          }

        if (!errorCode)
          {
            LOG_DEBUG(Logger, "opc_tcp_async         | accepted new client connection");
            OpcTcpConnection::SharedPtr connection = OpcTcpConnection::create(std::move(socket), *this, Server, Logger);
            {
              std::unique_lock<std::mutex> lock(Mutex);
              Clients.insert(connection);
            }
            connection->Start();
          }

        else
          {
            LOG_WARN(Logger, "opc_tcp_async         | error during client connection: {}", errorCode.message());
          }

        Accept();
      });
    }

  catch (const std::exception & exc)
    {
      LOG_WARN(Logger, "opc_tcp_async         | error accepting client connection: {}", exc.what());
    }
}

void OpcTcpServer::RemoveClient(OpcTcpConnection::SharedPtr client)
{
  std::unique_lock<std::mutex> lock(Mutex);
  Clients.erase(client);
}

} // namespace

OpcUa::Server::AsyncOpcTcp::UniquePtr OpcUa::Server::CreateAsyncOpcTcp(const OpcUa::Server::AsyncOpcTcp::Parameters & params, Services::SharedPtr server, boost::asio::io_service & io, const Common::Logger::SharedPtr & logger)
{
  return AsyncOpcTcp::UniquePtr(new OpcTcpServer(params, server, io, logger));
}
