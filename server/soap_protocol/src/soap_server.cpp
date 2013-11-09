/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "soap_server.h"

namespace OpcUa
{
  namespace Soap
  {

    void Server::ThreadProc(Server* server)
    {
      if (server->IsDebug()) std::clog << "SOAP: Starting server thread." << std::endl;
      soap s;
      soap_init(&s);

      const int backlog = 100;
      SOAP_SOCKET serverSock = soap_bind(&s, nullptr, server->Port, backlog);
      if (!soap_valid_socket(serverSock))
      {
        soap_stream_fault(&s, std::cerr);
        return;
      }

      server->S = &s;
      if (server->IsDebug()) std::clog << "SOAP: Created server socket." << std::endl;

      while(!server->Stopped)
      {
        SOAP_SOCKET clientSock = soap_accept(&s);
        if (clientSock < 0)
        {
          soap_stream_fault(&s, std::cerr);
          return;
        }
        soap* clientSoap = soap_copy(&s);
        server->ProcessRequest(clientSoap);
      }

      soap_end(&s);
      soap_done(&s);
      soap_free(&s);
    }

    void Server::ProcessRequest(soap* s)
    {
      int error = soap_begin_serve(s);
      if (error == SOAP_OK)
      {
        for (const auto& service : Services)
        {
          error = service->Dispatch(s);
          if (error != SOAP_NO_METHOD)
          {
            break;
          }
        }
      }
      soap_destroy(s);
      soap_end(s);
      soap_done(s);
      free(s);
    }

  } // namespace Soap
} // namespace OpcUa
