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
#include <opc/ua/server/standard_namespace.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

class AddressSpace : public Test
{
protected:
  virtual void SetUp()
  {
    const bool debug = false;
    NameSpace = OpcUa::Server::CreateAddressSpace(debug);
    OpcUa::Server::FillStandardNamespace(*NameSpace, debug);
  }

  virtual void TearDown()
  {
    NameSpace.reset();
  }

protected:
  OpcUa::Server::AddressSpace::UniquePtr NameSpace;
};

TEST_F(AddressSpace, GeneratesNodeIDIfPassNull)
{
  OpcUa::AddNodesItem newNode;
  newNode.BrowseName.Name = "newNode";
  newNode.Attributes = OpcUa::ObjectAttributes();
  std::vector<OpcUa::AddNodesResult> results = NameSpace->AddNodes({newNode});
  ASSERT_EQ(results.size(), 1);
  const OpcUa::AddNodesResult& result = results[0];
  EXPECT_EQ(result.Status, OpcUa::StatusCode::Good);
  EXPECT_NE(result.AddedNodeID, OpcUa::NodeID(OpcUa::ObjectID::Null));
}

TEST_F(AddressSpace, GeneratesIfNodeIDDuplicated)
{
  OpcUa::AddNodesItem newNode;
  newNode.BrowseName.Name = "newNode";
  newNode.RequestedNewNodeID = OpcUa::ObjectID::RootFolder;
  newNode.Attributes = OpcUa::ObjectAttributes();
  std::vector<OpcUa::AddNodesResult> results = NameSpace->AddNodes({newNode});
  ASSERT_EQ(results.size(), 1);
  const OpcUa::AddNodesResult& result = results[0];
  EXPECT_EQ(result.Status, OpcUa::StatusCode::BadNodeIdExists);
  EXPECT_EQ(result.AddedNodeID, OpcUa::ObjectID::Null);
}

TEST_F(AddressSpace, ReadAttributes)
{
  OpcUa::ReadParameters readParams;
  OpcUa::AttributeValueID value(OpcUa::ObjectID::RootFolder, OpcUa::AttributeID::BROWSE_NAME);
  readParams.AttributesToRead.push_back(value);
  std::vector<OpcUa::DataValue> results = NameSpace->Read(readParams);
  ASSERT_EQ(results.size(), 1);
  const OpcUa::DataValue& result = results[0];
  ASSERT_TRUE(result.Encoding | OpcUa::DATA_VALUE);
  EXPECT_EQ(result.Value, OpcUa::QualifiedName(OpcUa::Names::Root));
}
