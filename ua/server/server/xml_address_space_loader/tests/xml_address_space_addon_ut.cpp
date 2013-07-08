/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "../src/xml_address_space_loader.h"

#include <opc/ua/object_ids.h>
#include <opc/ua/attribute_ids.h>
#include <opc/ua/status_codes.h>
#include <opc/ua/server/addons/address_space_registry.h>

#include <src/server/address_space/src/address_space_addon.h>

#include <functional>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// TODO Add tests for several appearing nodes in xml.
// TODO Add tests for all node classes and for invalid classe names.

using namespace testing;
using namespace OpcUa;
using namespace OpcUa::Internal;

class XmlSpaceAddon : public testing::Test
{
protected:
  virtual void SetUp()
  {
    Addons = Common::CreateAddonsManager();
    Addons->Register(CreateAddressSpaceAddonConfig());
    Addons->Register(CreateXmlAddressSpaceAddonConfig());
    Addons->Start();
  }

  virtual void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  Common::AddonConfiguration CreateAddressSpaceAddonConfig()
  {
    Common::AddonConfiguration config;
    config.ID = OpcUa::Server::AddressSpaceRegistryAddonID;
    config.Factory.reset(new OpcUa::Internal::AddressSpaceAddonFactory());
    return config;
  }

  Common::AddonConfiguration CreateXmlAddressSpaceAddonConfig()
  {
    Common::AddonConfiguration tcpConfig;
    tcpConfig.ID = OpcUa::Server::XmlAddressSpaceAddonID;
    tcpConfig.Factory.reset(new OpcUa::Internal::XmlAddressSpaceAddonFactory());
    return tcpConfig;
  }

protected:
  Common::AddonsManager::SharedPtr Addons;
};

TEST_F(XmlSpaceAddon, CanBeLoaded)
{
  ASSERT_TRUE(static_cast<bool>(Addons->GetAddon(OpcUa::Server::XmlAddressSpaceAddonID)));
}
