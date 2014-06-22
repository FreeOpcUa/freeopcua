/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/interface.h>
#include <opc/ua/protocol/channel.h>

#include <memory>

namespace OpcUa
{
  namespace UaServer
  {

    class IncomingConnectionProcessor : private Common::Interface
    {
    public:
      virtual void Process(IOChannel::SharedPtr clientChannel) = 0;
      virtual void StopProcessing(IOChannel::SharedPtr clientChannel) = 0;
    };

    class ConnectionListener : private Common::Interface
    {
    public:
      virtual void Start(std::shared_ptr<IncomingConnectionProcessor> connectionProcssor) = 0;
      virtual void Stop() = 0;
    };

  } // namespace UaServer
}  // namespace OpcUA
