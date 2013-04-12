/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opc/ua/server/addons/endpoints.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/addons_core/dynamic_addon_factory.h>

#include <chrono>
#include <thread>

#include "common.h"

using namespace testing;

const unsigned short TestPort = 33455;



TEST(EndpointsAddon, CanBeLoadedLoaded)
{
  std::shared_ptr<Common::AddonsManager> addons = Common::CreateAddonsManager();
  addons->Register(OpcUa::Server::TcpServerAddonID, Common::CreateDynamicAddonFactory(OpcUa::GetTcpServerAddonPath().c_str()));
  addons->Register(OpcUa::Server::EndpointsAddonID, Common::CreateDynamicAddonFactory(OpcUa::GetEndpointsAddonPath().c_str()), std::vector<Common::AddonID>(1, OpcUa::Server::TcpServerAddonID));
  addons->Start();
  Common::Addon::UniquePtr addon;
  ASSERT_NO_THROW();
  ASSERT_TRUE(static_cast<bool>(addons->GetAddon(OpcUa::Server::EndpointsAddonID)));
  ASSERT_TRUE(static_cast<bool>(Common::GetAddon<OpcUa::Server::EndpointsAddon>(*addons, OpcUa::Server::EndpointsAddonID)));
  addons->Stop();
}

