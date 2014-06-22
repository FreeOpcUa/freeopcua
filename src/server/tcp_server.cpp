/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/thread.h>
#include <opc/ua/server/tcp_server.h>
#include <opc/ua/socket_channel.h>

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace
{
  using namespace OpcUa;
  using namespace OpcUa::UaServer;

 
  class SocketHolder
  {
  public:
    explicit SocketHolder(int socket)
      : Socket(socket)
    {
    }

    ~SocketHolder()
    {
      if (close(Socket) < 0)
      {
        std::cerr << "Unable to close server socket." << strerror(errno) << std::endl;
      }
    }

    bool operator < (int sock) const
    {
      return Socket < sock;
    }

  private:
    int Socket;
  };


  class Client : public Common::ThreadObserver
  {
  public:
    Client(std::shared_ptr<IOChannel> channel, std::shared_ptr<IncomingConnectionProcessor> processor, std::function<void()> onFinish)
      : Channel(channel)
      , Processor(processor)
      , OnFinish(onFinish)
    {
      std::clog << "Starting new client thread." << std::endl;
      std::function<void()> func = std::bind(&Client::Run, std::ref(*this));
      ClientThread.reset(new Common::Thread(func));
    }

    ~Client()
    {
      ClientThread->Join();
      ClientThread.reset();
      std::clog << "Client thread stopped." << std::endl;
    }

  protected:
    virtual void OnSuccess()
    {
      std::cerr << "Server thread was exited successfully." << std::endl;
    }

    virtual void OnError(const std::exception& exc)
    {
      std::cerr << "Server thread has exited with error:" << exc.what() << std::endl;
    }   

  private:
    void Run()
    {
      try
      {
        std::cout << "start process" << std::endl;
        Processor->Process(Channel); 
      }
      catch (const std::exception& exc)
      {
        std::cerr << "unable to process client connection. " << exc.what() << std::endl;
      }
      std::thread t(OnFinish);
      t.detach();
    }

  private:
    std::shared_ptr<IOChannel> Channel;
    std::shared_ptr<IncomingConnectionProcessor> Processor;
    std::function<void()> OnFinish;
    std::unique_ptr<Common::Thread> ClientThread;
  };


  class TcpServerConnection : private Common::ThreadObserver
  {
  public:
    TcpServerConnection(const TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor)
      : Port(params.Port)
      , Stopped(true)
      , Socket(-1)
      , Processor(processor)
    {
    }

    virtual ~TcpServerConnection()
    {
      try
      {
        Stop();
      }
      catch (const std::exception& exc)
      {
        std::cerr << "Error unable to stop server. " << exc.what() << std::endl;
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
        std::clog << "Shutting down opc ua binary server" << std::endl;
        Stopped = true;
        shutdown(Socket, SHUT_RDWR);
        ServerThread->Join();
        ServerThread.reset();
      }
    }

  protected:
    virtual void OnSuccess()
    {
      std::cerr << "Server thread was exited successfully." << std::endl;
    }

    virtual void OnError(const std::exception& exc)
    {
      std::cerr << "Server thread has exited with error:" << exc.what() << std::endl;
      //throw 20;
    }   

  private:
    void StartNewThread()
    {
      std::clog << "Starting new server thread." << std::endl;
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
        throw std::logic_error(std::string("Unable to create server socket. ") + strerror(errno));
      }

      SocketHolder holder(Socket);
      std::cout << "Listening on: " << Port << std::endl;

      sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(Port);
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      if (bind(Socket, (sockaddr*)&addr, sizeof(addr)) < 0)
      {
        if (Stopped)
        {
          return;
        }
        throw std::logic_error(std::string("Unable bind socket. ") + strerror(errno));
      }

      const unsigned ServerQueueSize = 5;
      listen (Socket, ServerQueueSize);
 
     
      while (!Stopped)
      {
        int clientSocket = accept(Socket, NULL, NULL);
        if (Stopped)
        {
          return;
        }

        if (clientSocket < 0)
        {
          throw std::logic_error(std::string("Unable to accept client connection. ") + strerror(errno));
        }

        std::unique_lock<std::mutex> lock(ClientsMutex);
        std::shared_ptr<IOChannel> clientChannel(new SocketChannel(clientSocket));
        std::shared_ptr<Client> clientThread(new Client(clientChannel, Processor, std::bind(&TcpServerConnection::Erase, std::ref(*this), clientSocket)));
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
    std::shared_ptr<IncomingConnectionProcessor> Processor;
    volatile bool Stopped;
    volatile int Socket;
    std::unique_ptr<Common::Thread> ServerThread;
    std::mutex ClientsMutex;
    std::map<int, std::shared_ptr<Client>> ClientThreads;
  };

  class TcpServer : public OpcUa::UaServer::TcpServer
  {
  public:
    DEFINE_CLASS_POINTERS(TcpServer);

    void Listen(const OpcUa::UaServer::TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor) override
    {
      if (Servers.find(params.Port) != std::end(Servers))
      {
        // TODO add portnumber into message.
        throw std::logic_error("Server already started at this port.");
      }

      std::shared_ptr<TcpServerConnection> listener(new TcpServerConnection(params, processor));
      listener->Start();
      Servers.insert(std::make_pair(params.Port, listener));
    }

    void StopListen(const OpcUa::UaServer::TcpParameters& params) override
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
      catch (const std::exception& exc)
      {
        // TODO add port number to the message
        std::clog << "Stopping TcpServerAddon failed with error: " << exc.what() << std::endl;
      }
    }

  private:
    typedef std::map<unsigned short, std::shared_ptr<TcpServerConnection>> ServersMap;
    ServersMap Servers;
  };

} // namespace

std::unique_ptr<OpcUa::UaServer::TcpServer> OpcUa::UaServer::CreateTcpServer()
{
  return std::unique_ptr<OpcUa::UaServer::TcpServer>(new ::TcpServer());
}
