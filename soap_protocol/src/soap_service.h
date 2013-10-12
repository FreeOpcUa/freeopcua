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

#include "soapH.h"

#include <opc/common/thread.h>

namespace OpcUa
{
  namespace Impl
  {

    template <typename ServiceType>
    class SoapService
    {
    private:
      typedef SoapService<ServiceType> SelfType;

      const unsigned Timeout = 24 * 60 * 60;

    public:
      SoapService(int port, std::unique_ptr<ServiceType> service)
        : Service(std::move(service))
        , Port(port)
      {
        Service->accept_timeout = Timeout;
        Service->bind_flags |= SO_REUSEADDR;
      }

      void Start()
      {
        if (Thread)
        {
          return;
        }

        Thread = Common::Thread::Create(std::bind(&ServiceProc, this));
      }

      void Stop()
      {
        if (Thread)
        {
          Service->destroy();
          Thread->Join();
          Thread.reset();
        }
      }

    private:
      void Run()
      {
        Service->run(Port);
      }

      static void ServiceProc(SelfType* service)
      {
        service->Run();
      }

    private:
      std::unique_ptr<ServiceType> Service;
      const int Port;
      Common::Thread::UniquePtr Thread;
    };

  } // namespace Impl
} // namespace OpcUa
