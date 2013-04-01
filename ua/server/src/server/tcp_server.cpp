/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <internal/thread.h>

#include <opc/ua/server/tcp_server.h>
#include <opc/ua/socket_channel.h>

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>


namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Internal;
  using namespace OpcUa::Server;

 
  class SocketHolder
  {
  public:
    SocketHolder(int socket)
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

  private:
    int Socket;
  };



  class TcpServer
    : public ConnectionListener
    , private ThreadObserver
  {
  public:
    TcpServer(unsigned short port)
      : Port(port)
      , Stopped(true)
      , Socket(-1)
    {
    }

    virtual ~TcpServer()
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

    virtual void Start(std::unique_ptr<IncomingConnectionProcessor> connectionProcessor) 
    {
      Processor = std::move(connectionProcessor);
      StartNewThread();
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
      if (!Stopped)
      {
        StartNewThread();
      }
    }   

  private:
    void StartNewThread()
    {
      std::clog << "Starting new server thread." << std::endl;
      Stopped = false;
      std::function<void()> func = std::bind(&TcpServer::Run, std::ref(*this));
      ThreadObserver& observer = *this;
      ServerThread.reset(new Thread(func, observer));
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

        try
        {
          std::unique_ptr<SocketChannel> connection(new SocketChannel(clientSocket));
          Processor->Process(std::move(connection));
        }
        catch (const std::exception& exc)
        {
          std::cerr << "unable to process client connection. " << exc.what() << std::endl;
        }
      }
    }

  private:
    const unsigned short Port;
    std::unique_ptr<IncomingConnectionProcessor> Processor;
    volatile bool Stopped;
    volatile int Socket;
    std::unique_ptr<Thread> ServerThread;
  };
}

std::unique_ptr<OpcUa::Server::ConnectionListener> OpcUa::CreateTcpServer(unsigned short port)
{
  return std::unique_ptr<OpcUa::Server::ConnectionListener>(new TcpServer(port));
}

