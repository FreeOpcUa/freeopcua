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
      SoapService(int port)
        : Service(SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE | SOAP_XML_INDENT)
        , Port(port)
      {
        Service.soap->accept_timeout = Timeout;
        Service.soap->bind_flags |= SO_REUSEADDR;
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
          Service.soap_close_socket();
          Thread->Join();
          Thread.reset();
        }
      }

    private:
      void Run()
      {
        Service.run(Port);
      }

      static void ServiceProc(SelfType* service)
      {
        service->Run();
      }

    private:
      ServiceType Service;
      const int Port;
      Common::Thread::UniquePtr Thread;
    };

  } // namespace Impl
} // namespace OpcUa
