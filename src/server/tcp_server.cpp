/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifdef _WIN32
#include <windows.h>
#endif

#include "tcp_server.h"

#include "opcua_protocol.h"
#include "opc_tcp_processor.h"
#include <opc/ua/protocol/utils.h>

#include <opc/common/thread.h>
#include <opc/common/uri_facade.h>
#include <opc/ua/services/services.h>
#include <opc/ua/socket_channel.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/input_from_buffer.h>


#include <errno.h>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string.h>
#include <sys/types.h>


#ifdef _WIN32
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <sys/socket.h>
#endif

namespace
{
using namespace OpcUa;
using namespace OpcUa::Binary;
using namespace OpcUa::Server;


class SocketHolder
{
public:
  explicit SocketHolder(int socket, const Common::Logger::SharedPtr & logger)
    : Socket(socket)
    , Logger(logger)
  {
  }

  ~SocketHolder()
  {
    if (close(Socket) < 0)
      {
        LOG_ERROR(Logger, "unable to close server socket");
      }
  }

  bool operator < (int sock) const
  {
    return Socket < sock;
  }

private:
  int Socket;
  Common::Logger::SharedPtr Logger;
};


class Client : public Common::ThreadObserver
{
public:
  Client(std::shared_ptr<IOChannel> channel, std::shared_ptr<IncomingConnectionProcessor> processor, std::function<void()> onFinish, const Common::Logger::SharedPtr & logger)
    : Channel(channel)
    , Processor(processor)
    , OnFinish(onFinish)
    , Logger(logger)
  {
    LOG_INFO(Logger, "starting new client thread");
    std::function<void()> func = std::bind(&Client::Run, std::ref(*this));
    ClientThread.reset(new Common::Thread(func));
  }

  ~Client()
  {
    ClientThread->Join();
    ClientThread.reset();
    LOG_INFO(Logger, "client thread stopped");
  }

protected:
  virtual void OnSuccess()
  {
    LOG_INFO(Logger, "server thread exited successfully");
  }

  virtual void OnError(const std::exception & exc)
  {
    LOG_ERROR(Logger, "server thread terminated: {}", exc.what());
  }

private:
  void Run()
  {
    try
      {
        LOG_INFO(Logger, "start to process client connection");
        Processor->Process(Channel);
      }

    catch (const std::exception & exc)
      {
        LOG_ERROR(Logger, "unable to process client connection: {}", exc.what());
      }

    std::thread t(OnFinish);
    t.detach();
  }

private:
  std::shared_ptr<IOChannel> Channel;
  std::shared_ptr<IncomingConnectionProcessor> Processor;
  std::function<void()> OnFinish;
  std::unique_ptr<Common::Thread> ClientThread;
  Common::Logger::SharedPtr Logger;
};


class TcpServerConnection : private Common::ThreadObserver
{
public:
  TcpServerConnection(const TcpParameters & params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor, const Common::Logger::SharedPtr & logger)
    : Port(params.Port)
    , Stopped(true)
    , Socket(-1)
    , Processor(processor)
    , Logger(logger)
  {
  }

  virtual ~TcpServerConnection()
  {
    try
      {
        Stop();
      }

    catch (const std::exception & exc)
      {
        LOG_ERROR(Logger, "unable to stop server: ", exc.what());
      }
  }

  virtual void Start()
  {
    if (!ServerThread)
      {
        StartNewThread();
      }
  }

  virtual void Stop()
  {
    if (ServerThread)
      {
        LOG_INFO(Logger, "shutting down opc ua binary server");
        Stopped = true;
        shutdown(Socket, SHUT_RDWR);
        ServerThread->Join();
        ServerThread.reset();
      }
  }

protected:
  virtual void OnSuccess()
  {
    LOG_INFO(Logger, "server thread exited successfully");
  }

  virtual void OnError(const std::exception & exc)
  {
    LOG_ERROR(Logger, "server thread exited with error: {}", exc.what());
    //throw 20;
  }

private:
  void StartNewThread()
  {
    LOG_INFO(Logger, "starting new server thread");
    Stopped = false;
    std::function<void()> func = std::bind(&TcpServerConnection::Run, std::ref(*this));
    ServerThread.reset(new Common::Thread(func, this));
  }

  void Run()
  {
    Socket = socket(AF_INET, SOCK_STREAM, 0);

    if (Stopped)
      {
        return;
      }

    if (Socket  < 0)
      {
        throw std::logic_error(std::string("unable to create server socket: ") + strerror(errno));
      }

    SocketHolder holder(Socket, Logger);
    LOG_INFO(Logger, "listening on: {}", Port);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(Socket, (sockaddr *)&addr, sizeof(addr)) < 0)
      {
        if (Stopped)
          {
            return;
          }

        throw std::logic_error(std::string("unable bind socket: ") + strerror(errno));
      }

    const unsigned ServerQueueSize = 5;
    listen(Socket, ServerQueueSize);


    while (!Stopped)
      {
        int clientSocket = accept(Socket, NULL, NULL);

        if (Stopped)
          {
            return;
          }

        if (clientSocket < 0)
          {
            throw std::logic_error(std::string("unable to accept client connection: ") + strerror(errno));
          }

        std::unique_lock<std::mutex> lock(ClientsMutex);
        std::shared_ptr<IOChannel> clientChannel(new SocketChannel(clientSocket));
        std::shared_ptr<Client> clientThread(new Client(clientChannel, Processor, std::bind(&TcpServerConnection::Erase, std::ref(*this), clientSocket), Logger));
        ClientThreads.insert(std::make_pair(clientSocket, clientThread));
      }

    ClientThreads.clear();
  }

  void Erase(int client)
  {
    std::unique_lock<std::mutex> lock(ClientsMutex);

    if (!Stopped)
      {
        ClientThreads.erase(client);
      }
  }

private:
  const unsigned short Port;
  volatile bool Stopped;
  volatile int Socket;
  std::shared_ptr<IncomingConnectionProcessor> Processor;
  std::unique_ptr<Common::Thread> ServerThread;
  std::mutex ClientsMutex;
  std::map<int, std::shared_ptr<Client>> ClientThreads;
  Common::Logger::SharedPtr Logger;
};

class TcpServer : public OpcUa::Server::TcpServer
{
public:
  DEFINE_CLASS_POINTERS(TcpServer)

  TcpServer(const Common::Logger::SharedPtr & logger)
    : OpcUa::Server::TcpServer(logger)
  {}

  void Listen(const OpcUa::Server::TcpParameters & params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor) override
  {
    if (Servers.find(params.Port) != std::end(Servers))
      {
        // TODO add portnumber into message.
        throw std::logic_error("server on this port already exists");
      }

    std::shared_ptr<TcpServerConnection> listener(new TcpServerConnection(params, processor, Logger));
    listener->Start();
    Servers.insert(std::make_pair(params.Port, listener));
  }

  void StopListen(const OpcUa::Server::TcpParameters & params) override
  {
    ServersMap::iterator serverIt = Servers.find(params.Port);

    if (serverIt == std::end(Servers))
      {
        return;
      }

    try
      {
        serverIt->second->Stop();
        Servers.erase(serverIt);
      }

    catch (const std::exception & exc)
      {
        LOG_ERROR(Logger, "stopping TcpServer on port: {} failed: {}", params.Port, exc.what());
      }
  }

private:
  typedef std::map<unsigned short, std::shared_ptr<TcpServerConnection>> ServersMap;
  ServersMap Servers;
};

} // namespace

std::unique_ptr<OpcUa::Server::TcpServer> OpcUa::Server::CreateTcpServer(const Common::Logger::SharedPtr & logger)
{
  return std::unique_ptr<OpcUa::Server::TcpServer>(new ::TcpServer(logger));
}
