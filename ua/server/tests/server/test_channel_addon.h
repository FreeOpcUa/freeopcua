/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_test_channel_addon_h
#define opc_ua_test_channel_addon_h

#include <opc/ua/server/addons/tcp_server_addon.h>

namespace OpcUa
{
  namespace Tests
  {

    class TestNetworkAddon : public OpcUa::Server::TcpServerAddon
    {
    public:
      void Accept(std::shared_ptr<IOChannel> client); 
    };

  } // namespace Tests
} // namespace OpcUa

#endif // opc_ua_test_channel_addon_h

