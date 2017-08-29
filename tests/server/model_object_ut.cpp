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


class ModelObject : public Test
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
    item.BrowseName = OpcUa::QualifiedName("object_type");
    item.Class = OpcUa::NodeClass::ObjectType;
    item.ParentNodeId = OpcUa::ObjectId::BaseObjectType;
    item.ReferenceTypeId = OpcUa::ObjectId::HasSubtype;

    OpcUa::ObjectTypeAttributes attrs;
    attrs.Description = OpcUa::LocalizedText("object_type");
    attrs.DisplayName = OpcUa::LocalizedText("object_type");
    attrs.IsAbstract = false;
    item.Attributes = attrs;
    std::vector<OpcUa::AddNodesResult> result = nodes->AddNodes({item});
    return result[0].AddedNodeId;
  }

  OpcUa::NodeId CreateObjectTypeWithOneVariable()
  {
    const OpcUa::NodeId & objectId = CreateEmptyObjectType();
    OpcUa::AddNodesItem variable;
    variable.BrowseName = OpcUa::QualifiedName("variable");
    variable.Class = OpcUa::NodeClass::Variable;
    variable.ParentNodeId = objectId;
    variable.ReferenceTypeId = OpcUa::ObjectId::HasProperty;
    OpcUa::VariableAttributes attrs;
    attrs.DisplayName = OpcUa::LocalizedText("variable");
    variable.Attributes = attrs;
    Services->NodeManagement()->AddNodes({variable});
    return objectId;
  }

  OpcUa::NodeId CreateObjectTypeWithOneUntypedObject()
  {
    const OpcUa::NodeId & objectId = CreateEmptyObjectType();
    OpcUa::AddNodesItem object;
    object.BrowseName = OpcUa::QualifiedName("sub_object");
    object.Class = OpcUa::NodeClass::Object;
    object.ParentNodeId = objectId;
    object.ReferenceTypeId = OpcUa::ObjectId::HasComponent;
    OpcUa::ObjectAttributes attrs;
    attrs.DisplayName = OpcUa::LocalizedText("sub_object");
    object.Attributes = attrs;
    Services->NodeManagement()->AddNodes({object});
    return objectId;
  }

  OpcUa::NodeId CreateObjectTypeWithOneTypedObject()
  {
    const OpcUa::NodeId & resultTypeId = CreateEmptyObjectType();
    const OpcUa::NodeId & objectTypeWithVar = CreateObjectTypeWithOneVariable();
    OpcUa::AddNodesItem object;
    object.BrowseName = OpcUa::QualifiedName("sub_object");
    object.Class = OpcUa::NodeClass::Object;
    object.ParentNodeId = resultTypeId;
    object.ReferenceTypeId = OpcUa::ObjectId::HasComponent;
    object.TypeDefinition = objectTypeWithVar;
    OpcUa::ObjectAttributes attrs;
    attrs.DisplayName = OpcUa::LocalizedText("sub_object");
    object.Attributes = attrs;
    Services->NodeManagement()->AddNodes({object});
    return resultTypeId;
  }

protected:
  Common::Logger::SharedPtr Logger;
  Common::AddonsManager::UniquePtr Addons;
  OpcUa::Services::SharedPtr Services;
};


TEST_F(ModelObject, ServerCanAccessToRootObject)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::Object rootObject = server.RootObject();

  ASSERT_EQ(rootObject.GetId(), OpcUa::ObjectId::RootFolder);
}

TEST_F(ModelObject, ObjectCanCreateVariable)
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
}

TEST_F(ModelObject, CanInstantiateEmptyObjectType)
{
  const OpcUa::NodeId & typeId = CreateEmptyObjectType();
  OpcUa::Model::ObjectType objectType(typeId, Services);
  OpcUa::Model::Object rootObject(OpcUa::ObjectId::RootFolder, Services);
  const char * objectDesc = "Empty object.";
  const OpcUa::QualifiedName browseName("empty_object");
  const OpcUa::NodeId objectId = rootObject.CreateObject(objectType, browseName, objectDesc).GetId();
  OpcUa::Model::Object object(objectId, Services);

  ASSERT_NE(object.GetId(), OpcUa::ObjectId::Null);
  ASSERT_EQ(object.GetBrowseName(), browseName) << "Real name: " << object.GetBrowseName().Name;

  std::vector<OpcUa::Model::Variable> variables = object.GetVariables();
  ASSERT_EQ(variables.size(), 0);
}

TEST_F(ModelObject, CanInstantiateObjectTypeWithOneVariable)
{
  const OpcUa::NodeId & typeId = CreateObjectTypeWithOneVariable();
  OpcUa::Model::ObjectType objectType(typeId, Services);
  OpcUa::Model::Object rootObject(OpcUa::ObjectId::RootFolder, Services);
  const char * objectDesc = "object_with_var.";
  const OpcUa::QualifiedName browseName("object_with_var");
  const OpcUa::NodeId objectId = rootObject.CreateObject(objectType, browseName, objectDesc).GetId();
  OpcUa::Model::Object object(objectId, Services);

  ASSERT_NE(object.GetId(), OpcUa::ObjectId::Null);
  ASSERT_EQ(object.GetBrowseName(), browseName) << "Real name: " << object.GetBrowseName().Name;

  std::vector<OpcUa::Model::Variable> variables = object.GetVariables();
  ASSERT_EQ(variables.size(), 1);
}

TEST_F(ModelObject, CanInstantiateObjectTypeWithOneUntypedObject)
{
  const OpcUa::NodeId & typeId = CreateObjectTypeWithOneUntypedObject();
  OpcUa::Model::ObjectType objectType(typeId, Services);
  OpcUa::Model::Object rootObject(OpcUa::ObjectId::RootFolder, Services);
  const char * objectDesc = "object_with_var.";
  const OpcUa::QualifiedName browseName("object_with_var");
  const OpcUa::NodeId objectId = rootObject.CreateObject(objectType, browseName, objectDesc).GetId();
  OpcUa::Model::Object object(objectId, Services);

  ASSERT_NE(object.GetId(), OpcUa::ObjectId::Null);
  ASSERT_EQ(object.GetBrowseName(), browseName) << "Real name: " << object.GetBrowseName().Name;

  std::vector<OpcUa::Model::Object> objects = object.GetObjects();
  ASSERT_EQ(objects.size(), 1);
}

TEST_F(ModelObject, CanInstantiateObjectTypeWithOneTypedObject)
{
  // Type with one property - empty object with type that has a variable.
  // ObjectType1
  //   +-object - ObjectType2
  //
  // ObjectType2
  //   +-variable

  const OpcUa::NodeId & typeId = CreateObjectTypeWithOneTypedObject();
  OpcUa::Model::ObjectType objectType(typeId, Services);
  // we will create objects under root folder.
  OpcUa::Model::Object rootObject(OpcUa::ObjectId::RootFolder, Services);
  const char * objectDesc = "object_with_var.";
  const OpcUa::QualifiedName browseName("object_with_var");
  // Instantiate object type we have created first.
  // Get only id of that object.
  const OpcUa::NodeId objectId = rootObject.CreateObject(objectType, browseName, objectDesc).GetId();
  // This constructor will read all parameters of created object.
  // Restored object structure should be next:
  // Object1 - ObjectType1
  //   +-Object2 - ObjectType2
  //       +-variable
  OpcUa::Model::Object object(objectId, Services);

  ASSERT_EQ(object.GetId(), objectId);
  ASSERT_EQ(object.GetBrowseName(), browseName) << "Real name: " << object.GetBrowseName().Name;

  // Created object will have one sub object.
  std::vector<OpcUa::Model::Object> objects = object.GetObjects();
  ASSERT_EQ(objects.size(), 1);
  const OpcUa::Model::Object & subObject = objects[0];
  // Sub object in the source object type dedn't have any sub objects.
  // But it has a type definition which has one variable.
  // And new instantiated object have to restore full hierarchy.
  std::vector<OpcUa::Model::Variable> variables;
  ASSERT_NO_THROW(variables = subObject.GetVariables());
  ASSERT_EQ(variables.size(), 1);
}

OpcUa::RelativePathElement GetHierarchicalElement(const std::string & browseName)
{
  OpcUa::RelativePathElement element;
  element.ReferenceTypeId = OpcUa::ObjectId::HierarchicalReferences;
  element.IncludeSubtypes = true;
  element.TargetName.Name = browseName;
  return element;
}

TEST_F(ModelObject, CanAccessVariableByBrowsePath)
{
  OpcUa::Model::Server server(Services);
  OpcUa::Model::Object serverObject = server.GetObject(OpcUa::ObjectId::Server);
  OpcUa::RelativePath path;
  path.Elements.push_back(GetHierarchicalElement(OpcUa::Names::ServerStatus));
  path.Elements.push_back(GetHierarchicalElement(OpcUa::Names::BuildInfo));
  path.Elements.push_back(GetHierarchicalElement(OpcUa::Names::BuildNumber));

  OpcUa::Model::Variable buildNumber = serverObject.GetVariable(path);
  EXPECT_EQ(buildNumber.GetBrowseName(), OpcUa::QualifiedName(OpcUa::Names::BuildNumber));
}

TEST_F(ModelObject, CanAccessVariableByQualifiedName)
{
  std::cout << "1" << std::endl;
  OpcUa::Model::Server server(Services);
  std::cout << "2" << std::endl;
  OpcUa::Model::Object rootObject = server.RootObject();
  std::cout << "3" << std::endl;
  OpcUa::Model::ObjectType serverType = server.GetObjectType(OpcUa::ObjectId::ServerType);
  std::cout << "4" << std::endl;
  OpcUa::Model::Object serverObject = rootObject.CreateObject(serverType, OpcUa::QualifiedName("Server"));
  std::cout << "5" << std::endl;
  OpcUa::Model::Variable serverStatus = serverObject.GetVariable(OpcUa::QualifiedName(OpcUa::Names::ServerStatus));
  std::cout << "6" << std::endl;
  EXPECT_EQ(serverStatus.GetBrowseName(), OpcUa::QualifiedName(OpcUa::Names::ServerStatus));
  std::cout << "7" << std::endl;
}
