/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Common test utls.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opcua_tests_common_utils_h
#define opcua_tests_common_utils_h

#include <opc/ua/server/server.h>

namespace OpcUa
{

  inline std::string GetTcpServerAddonPath()
  {
    if (const char* path = getenv("TCP_ADDON_PATH"))
    {
      return path;
    }
    return std::string();
  }

  inline std::string GetEndpointsAddonPath()
  {
    if (const char* path = getenv("ENDPOINTS_ADDON_PATH"))
    {
      return path;
    }
    return std::string();
  }


   class IncomingConnectionProcessorMock : public OpcUa::Server::IncomingConnectionProcessor
   {
   public:
     MOCK_METHOD1(Process, void (std::shared_ptr<OpcUa::IOChannel>));
     MOCK_METHOD1(StopProcessing, void (std::shared_ptr<OpcUa::IOChannel> clientChannel));
   };

  class EchoProcessor : public OpcUa::Server::IncomingConnectionProcessor
  {
  public:
    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      char data[4] = {0};
      clientChannel->Receive(data, 4);
      clientChannel->Send(data, 4);
    }
    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }
  };

}

#endif //  opcua_tests_common_utils_h

