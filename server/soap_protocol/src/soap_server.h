/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include <opc/common/interface.h>
#include <opc/common/thread.h>

#include <memory>
#include <stdsoap2.h>
#include <vector>

namespace OpcUa
{
  namespace Soap
  {

    class Service : private Common::Interface
    {
    public:
      virtual int Dispatch(soap* s) = 0;
    };

    class Server : private Common::Interface
    {
    public:
      explicit Server(unsigned port, bool debug)
        : Stopped(true)
        , Debug(debug)
        , Port(port)
        , S(0)
      {
      }

      virtual ~Server()
      {
        Stop();
      }

      void AddService(std::unique_ptr<Service> service)
      {
        Services.push_back(std::shared_ptr<Service>(std::move(service)));
      }

      void Run()
      {
        Stopped = false;
        ServerThread.reset(new Common::Thread(std::bind(ThreadProc, this)));
      }

      void Stop()
      {
        Stopped = true;
        if (S)
        {
          shutdown(S->master, SHUT_RDWR);
        }
        if (ServerThread)
        {
          ServerThread->Join();
          ServerThread.reset();
        }
      }

    private:
      static void ThreadProc(Server* server);

      void ProcessRequest(soap* s);

      bool IsDebug() const
      {
        return Debug;
      }

    private:
      volatile bool Stopped;
      bool Debug;
      unsigned Port;
      soap* S;
      std::unique_ptr<Common::Thread> ServerThread;
      std::vector<std::shared_ptr<Service>> Services;
    };

  } // namespace Soap
} // namespace OpcUa
