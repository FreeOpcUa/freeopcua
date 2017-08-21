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

#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/status_codes.h>

#include <opc/ua/server/address_space.h>
#include <opc/ua/server/standard_address_space.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

class AddressSpace : public Test
{
protected:
  virtual void SetUp()
  {
    spdlog::drop_all();
    Logger = spdlog::stderr_color_mt("test");
    Logger->set_level(spdlog::level::info);
    NameSpace = OpcUa::Server::CreateAddressSpace(Logger);
    OpcUa::Server::FillStandardNamespace(*NameSpace, Logger);
  }

  virtual void TearDown()
  {
    NameSpace.reset();
  }

  OpcUa::NodeId CreateValue()
  {
    OpcUa::AddNodesItem item;
    item.Attributes = OpcUa::VariableAttributes();
    item.BrowseName = OpcUa::QualifiedName("value");
    item.Class = OpcUa::NodeClass::Variable;
    item.ParentNodeId = OpcUa::ObjectId::RootFolder;
    std::vector<OpcUa::AddNodesResult> newNodesResult = NameSpace->AddNodes({item});
    return newNodesResult[0].AddedNodeId;
  }

protected:
  OpcUa::Server::AddressSpace::UniquePtr NameSpace;
  Common::Logger::SharedPtr Logger;
};

TEST_F(AddressSpace, GeneratesNodeIdIfPassNull)
{
  OpcUa::AddNodesItem newNode;
  newNode.BrowseName.Name = "newNode";
  newNode.Attributes = OpcUa::ObjectAttributes();
  std::vector<OpcUa::AddNodesResult> results = NameSpace->AddNodes({newNode});
  ASSERT_EQ(results.size(), 1);
  const OpcUa::AddNodesResult & result = results[0];
  EXPECT_EQ(result.Status, OpcUa::StatusCode::Good);
  EXPECT_NE(result.AddedNodeId, OpcUa::NodeId(OpcUa::ObjectId::Null));
}

TEST_F(AddressSpace, GeneratesIfNodeIdDuplicated)
{
  OpcUa::AddNodesItem newNode;
  newNode.BrowseName.Name = "newNode";
  newNode.RequestedNewNodeId = OpcUa::ObjectId::RootFolder;
  newNode.Attributes = OpcUa::ObjectAttributes();
  std::vector<OpcUa::AddNodesResult> results = NameSpace->AddNodes({newNode});
  ASSERT_EQ(results.size(), 1);
  const OpcUa::AddNodesResult & result = results[0];
  EXPECT_EQ(result.Status, OpcUa::StatusCode::BadNodeIdExists);
  EXPECT_EQ(result.AddedNodeId, OpcUa::ObjectId::Null);
}

TEST_F(AddressSpace, ReadAttributes)
{
  OpcUa::ReadParameters readParams;
  OpcUa::ReadValueId value = ToReadValueId(OpcUa::ObjectId::RootFolder, OpcUa::AttributeId::BrowseName);
  readParams.AttributesToRead.push_back(value);
  std::vector<OpcUa::DataValue> results = NameSpace->Read(readParams);
  ASSERT_EQ(results.size(), 1);
  const OpcUa::DataValue & result = results[0];
  ASSERT_NE(result.Encoding | OpcUa::DATA_VALUE, 0);
  EXPECT_EQ(result.Value, OpcUa::QualifiedName(OpcUa::Names::Root));
}

TEST_F(AddressSpace, CallsDataChangeCallbackOnWrite)
{
  OpcUa::NodeId valueId = CreateValue();
  OpcUa::NodeId callbackId;
  OpcUa::AttributeId callbackAttr;
  OpcUa::DataValue callbackValue;
  bool callbackCalled = false;
  unsigned callbackHandle = NameSpace->AddDataChangeCallback(valueId, OpcUa::AttributeId::Value, [&](const OpcUa::NodeId & id, OpcUa::AttributeId attr, const OpcUa::DataValue & value)
  {
    callbackId = id;
    callbackAttr = attr;
    callbackValue = value;
    callbackCalled = true;
  });

  EXPECT_NE(callbackHandle, 0);

  OpcUa::WriteValue value;
  value.AttributeId = OpcUa::AttributeId::Value;
  value.NodeId = valueId;
  value.Value = 10;
  std::vector<OpcUa::StatusCode> result = NameSpace->Write({value});
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], OpcUa::StatusCode::Good);
  EXPECT_EQ(callbackId, valueId);
  EXPECT_EQ(callbackValue, 10);
  EXPECT_EQ(callbackAttr, OpcUa::AttributeId::Value);

  callbackCalled = false;
  NameSpace->DeleteDataChangeCallback(callbackHandle);
  result = NameSpace->Write({value});
  ASSERT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], OpcUa::StatusCode::Good);
  ASSERT_FALSE(callbackCalled);
}

TEST_F(AddressSpace, ValueCallbackIsCalled)
{
  OpcUa::NodeId valueId = CreateValue();
  OpcUa::StatusCode code = NameSpace->SetValueCallback(valueId, OpcUa::AttributeId::Value, []()
  {
    return OpcUa::DataValue(10);
  });

  ASSERT_EQ(code, OpcUa::StatusCode::Good);

  OpcUa::ReadParameters readParams;
  readParams.AttributesToRead.push_back(OpcUa::ToReadValueId(valueId, OpcUa::AttributeId::Value));
  std::vector<OpcUa::DataValue> result = NameSpace->Read(readParams);
  ASSERT_EQ(result.size(), 1);
  EXPECT_TRUE(result[0].Encoding & OpcUa::DATA_VALUE);
  EXPECT_EQ(result[0].Value, 10);
}
