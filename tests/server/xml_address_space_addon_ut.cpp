/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "xml_address_space_loader.h"
#include "address_space_registry_test.h"
#include "services_registry_test.h"

#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/status_codes.h>
#include <opc/ua/server/addons/address_space.h>

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
    OpcUa::Test::RegisterAddressSpace(*Addons);
    OpcUa::Test::RegisterServicesRegistry(*Addons);
    Addons->Register(CreateXmlAddressSpaceAddonConfig());
    Addons->Start();
  }

  virtual void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  Common::AddonInformation CreateXmlAddressSpaceAddonConfig()
  {
    Common::AddonInformation config;
    config.ID = OpcUa::UaServer::XmlAddressSpaceAddonID;
    config.Factory.reset(new OpcUa::Internal::XmlAddressSpaceAddonFactory());
    config.Dependencies.push_back(OpcUa::UaServer::AddressSpaceRegistryAddonID);
    return config;
  }

protected:
  Common::AddonsManager::SharedPtr Addons;
};

TEST_F(XmlSpaceAddon, CanBeLoaded)
{
  ASSERT_TRUE(static_cast<bool>(Addons->GetAddon(OpcUa::UaServer::XmlAddressSpaceAddonID)));
}
