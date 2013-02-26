/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef _OPC_UA_BINARY_SERVER_H
#define _OPC_UA_BINARY_SERVER_H

#include <opc/ua/channel.h>

#include <memory>

namespace OpcUa
{

  class IncomingConnectionProcessor : private Interface
  {
  public:
    virtual void Process(std::unique_ptr<IOChannel> clientChannel) = 0;
  };

  class Server : private Interface
  {
  public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
  };

  std::unique_ptr<Server> CreateServer(unsigned short port, std::unique_ptr<IncomingConnectionProcessor> processor);

}

#endif // _OPC_UA_BINARY_SERVER_H

