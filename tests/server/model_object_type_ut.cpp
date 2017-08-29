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


class ModelObjectType : public Test
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

  OpcUa::NodeId CreateEmptyObjectType()
  {
    OpcUa::NodeManagementServices::SharedPtr nodes = Services->NodeManagement();
    OpcUa::AddNodesItem item;
    item.BrowseName = OpcUa::QualifiedName("empty_object_type");
    item.Class = OpcUa::NodeClass::ObjectType;
    item.ParentNodeId = OpcUa::ObjectId::BaseObjectType;
    item.ReferenceTypeId = OpcUa::ObjectId::HasSubtype;

    OpcUa::ObjectTypeAttributes attrs;
    attrs.Description = OpcUa::LocalizedText("empty_object_type");
    attrs.DisplayName = OpcUa::LocalizedText("empty_object_type");
    attrs.IsAbstract = false;
    item.Attributes = attrs;
    std::vector<OpcUa::AddNodesResult> result = nodes->AddNodes({item});
    return result[0].AddedNodeId;
  }

  OpcUa::NodeId CreateObjectTypeWithOneVariable()
  {
    const OpcUa::NodeId & objectId = CreateEmptyObjectType();
    OpcUa::AddNodesItem variable;
    variable.BrowseName = OpcUa::QualifiedName("new_variable1");
    variable.Class = OpcUa::NodeClass::Variable;
    variable.ParentNodeId = objectId;
    variable.ReferenceTypeId = OpcUa::ObjectId::HasProperty;
    OpcUa::VariableAttributes attrs;
    attrs.DisplayName = OpcUa::LocalizedText("new_variable");
    variable.Attributes = attrs;
    Services->NodeManagement()->AddNodes({variable});
    return objectId;
  }

  OpcUa::NodeId CreateObjectTypeWithOneUntypedObject()
  {
    const OpcUa::NodeId & objectId = CreateEmptyObjectType();
    OpcUa::AddNodesItem object;
    object.BrowseName = OpcUa::QualifiedName("new_sub_object1");
    object.Class = OpcUa::NodeClass::Object;
    object.ParentNodeId = objectId;
    object.ReferenceTypeId = OpcUa::ObjectId::HasComponent;
    OpcUa::ObjectAttributes attrs;
    attrs.DisplayName = OpcUa::LocalizedText("new_sub_object");
    object.Attributes = attrs;
    Services->NodeManagement()->AddNodes({object});
    return objectId;
  }

  OpcUa::NodeId CreateObjectTypeWithOneTypedObject()
  {
    const OpcUa::NodeId & resultTypeId = CreateEmptyObjectType();
    const OpcUa::NodeId & objectTypeWithVar = CreateObjectTypeWithOneVariable();
    OpcUa::AddNodesItem object;
    object.BrowseName = OpcUa::QualifiedName("new_sub_object1");
    object.Class = OpcUa::NodeClass::Object;
    object.ParentNodeId = resultTypeId;
    object.ReferenceTypeId = OpcUa::ObjectId::HasComponent;
    object.TypeDefinition = objectTypeWithVar;
    OpcUa::ObjectAttributes attrs;
    attrs.DisplayName = OpcUa::LocalizedText("new_sub_object");
    object.Attributes = attrs;
    Services->NodeManagement()->AddNodes({object});
    return resultTypeId;
  }

protected:
  Common::Logger::SharedPtr Logger;
  Common::AddonsManager::UniquePtr Addons;
  OpcUa::Services::SharedPtr Services;
};


TEST_F(ModelObjectType, ServerAccessObjectTypes)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::ObjectType baseObjectType = server.GetObjectType(OpcUa::ObjectId::BaseObjectType);
  ASSERT_EQ(baseObjectType.GetId(), OpcUa::ObjectId::BaseObjectType);
  ASSERT_EQ(baseObjectType.GetDisplayName(), OpcUa::LocalizedText(OpcUa::Names::BaseObjectType));
  ASSERT_EQ(baseObjectType.GetBrowseName(), OpcUa::QualifiedName(OpcUa::Names::BaseObjectType));
  ASSERT_EQ(baseObjectType.IsAbstract(), false);
}

TEST_F(ModelObjectType, ObjectTypeAllowsAccessToSubtypes)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::ObjectType baseObjectType = server.GetObjectType(OpcUa::ObjectId::BaseObjectType);
  std::vector<OpcUa::Model::ObjectType> subTypes = baseObjectType.SubTypes();
  ASSERT_FALSE(subTypes.empty());
}

TEST_F(ModelObjectType, ObjectTypeAllowsAccessToVariables)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::ObjectType serverType = server.GetObjectType(OpcUa::ObjectId::ServerType);
  std::vector<OpcUa::Model::Variable> variables = serverType.Variables();
  ASSERT_FALSE(variables.empty());
}

TEST_F(ModelObjectType, ObjectTypeAllowsAccessToObjects)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::ObjectType serverType = server.GetObjectType(OpcUa::ObjectId::ServerType);
  std::vector<OpcUa::Model::Object> objects = serverType.Objects();
  ASSERT_FALSE(objects.empty());
}
