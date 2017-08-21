/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/


#include <opc/ua/model.h>

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/status_codes.h>
#include <opc/ua/services/services.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/server/standard_address_space.h>

#include "address_space_registry_test.h"
#include "services_registry_test.h"
#include "standard_namespace_test.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;


class ModelVariable : public Test
{
protected:
  virtual void SetUp()
  {
    spdlog::drop_all();
    Logger = spdlog::stderr_color_mt("test");
    Logger->set_level(spdlog::level::info);
    Addons = Common::CreateAddonsManager(Logger);

    OpcUa::Test::RegisterServicesRegistry(*Addons);
    OpcUa::Test::RegisterAddressSpace(*Addons);
    OpcUa::Test::RegisterStandardNamespace(*Addons);
    Addons->Start();

    OpcUa::Server::ServicesRegistry::SharedPtr addon = Addons->GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);
    Services = addon->GetServer();
  }

  virtual void TearDown()
  {
    Services.reset();
    Addons->Stop();
    Addons.reset();
  }

protected:
  Common::Logger::SharedPtr Logger;
  Common::AddonsManager::UniquePtr Addons;
  OpcUa::Services::SharedPtr Services;
};


TEST_F(ModelVariable, CanSetVariableValue_ByVariant)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::Object rootObject = server.RootObject();
  OpcUa::QualifiedName name("new_variable");
  OpcUa::Variant value = 8;
  OpcUa::Model::Variable variable = rootObject.CreateVariable(name, value);

  ASSERT_NE(variable.GetId(), OpcUa::ObjectId::Null);
  ASSERT_EQ(variable.GetBrowseName(), name);
  ASSERT_EQ(variable.GetDisplayName(), OpcUa::LocalizedText(name.Name));
  ASSERT_EQ(variable.GetValue(), value);

  variable.SetValue(10);
  OpcUa::DataValue data = variable.GetValue();
  ASSERT_TRUE(data.Encoding & (OpcUa::DATA_VALUE));
  ASSERT_TRUE(data.Encoding & (OpcUa::DATA_VALUE_SOURCE_TIMESTAMP));
  EXPECT_EQ(data.Value, 10);
  EXPECT_NE(data.SourceTimestamp, 0);
}

TEST_F(ModelVariable, CanSetVariableValue_DataValue)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::Object rootObject = server.RootObject();
  OpcUa::QualifiedName name("new_variable");
  OpcUa::Variant value = 8;
  OpcUa::Model::Variable variable = rootObject.CreateVariable(name, value);

  ASSERT_NE(variable.GetId(), OpcUa::ObjectId::Null);
  ASSERT_EQ(variable.GetBrowseName(), name);
  ASSERT_EQ(variable.GetDisplayName(), OpcUa::LocalizedText(name.Name));
  ASSERT_EQ(variable.GetValue(), value);

  OpcUa::DataValue data(10);
  data.SetSourceTimestamp(OpcUa::DateTime(12345));
  variable.SetValue(data);
  OpcUa::DataValue result = variable.GetValue();
  ASSERT_NE(result.Encoding & (OpcUa::DATA_VALUE), 0);
  ASSERT_NE(result.Encoding & (OpcUa::DATA_VALUE_SOURCE_TIMESTAMP), 0);
  EXPECT_EQ(result.Value, 10);
  EXPECT_EQ(result.SourceTimestamp, 12345);
}

TEST_F(ModelVariable, CanSetListSubVariables)
{
  OpcUa::Model::Variable variable(OpcUa::ObjectId::Server_ServerStatus_BuildInfo, Services);
  std::vector<OpcUa::Model::Variable> vars = variable.Variables();
  EXPECT_FALSE(vars.empty());
}
