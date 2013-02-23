/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary attributes.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "common.h"

#include <opc/ua/extension_identifiers.h>
#include <opc/ua/message_identifiers.h>
#include <opc/ua/binary/stream.h>
#include <opc/ua/binary/types.h>
#include <opc/ua/binary/variant.h>
#include <opc/ua/binary/data_value.h>

#include <algorithm>
#include <stdexcept>

//-------------------------------------------------------
// Serialization
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Variant_NUL)
{

  using namespace OpcUa::Binary;

  Variant var;

  GetStream() << var << flush;

  const std::vector<char> expectedData = {
  0
  };

  ASSERT_EQ(expectedData.size(), RawSize(var));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinarySerialization, Variant_BOOLEAN)
{

  using namespace OpcUa::Binary;

  Variant var;
  var.Type = VariantType::BOOLEAN;
  var.Value.Boolean = std::vector<bool>{true};

  GetStream() << var << flush;

  uint8_t encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN);
  const std::vector<char> expectedData = {
  encodingMask,
  1
  };

  ASSERT_EQ(expectedData.size(), RawSize(var));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinarySerialization, Variant_BOOLEAN_Array)
{

  using namespace OpcUa::Binary;

  Variant var;
  var.Type = VariantType::BOOLEAN;
  var.Value.Boolean = std::vector<bool>{true, true};

  GetStream() << var << flush;

  uint8_t encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_ARRAY_MASK;
  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  1,
  1
  };

  ASSERT_EQ(expectedData.size(), RawSize(var));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}


TEST_F(OpcUaBinarySerialization, Variant_BOOLEAN_DIMENSIONS)
{

  using namespace OpcUa::Binary;

  Variant var;
  var.Type = VariantType::BOOLEAN;
  var.Value.Boolean = std::vector<bool>{true};
  var.Dimensions.push_back(1);

  GetStream() << var << flush;

  uint8_t encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_DIMENSIONS_MASK;
  const std::vector<char> expectedData = {
  encodingMask,
  1,
  1,0,0,0,
  1,0,0,0
  };

  ASSERT_EQ(expectedData.size(), RawSize(var));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}


//-------------------------------------------------------
// Deserialization
//-------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Variant_NUL)
{
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type, VariantType::NUL);
  ASSERT_TRUE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

TEST_F(OpcUaBinaryDeserialization, Variant_BOOLEAN)
{
  using namespace OpcUa::Binary;

  uint8_t encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN);

  const std::vector<char> expectedData = {
  encodingMask,
  1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type, VariantType::BOOLEAN);
  ASSERT_EQ(var.Value.Boolean.size(), 1);
  ASSERT_EQ(var.Value.Boolean[0], true);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

TEST_F(OpcUaBinaryDeserialization, Variant_BOOLEAN_Array)
{
  using namespace OpcUa::Binary;

  uint8_t encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  1,
  1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type, VariantType::BOOLEAN);
  ASSERT_EQ(var.Value.Boolean.size(), 2);
  ASSERT_EQ(var.Value.Boolean[0], true);
  ASSERT_EQ(var.Value.Boolean[1], true);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
}


TEST_F(OpcUaBinaryDeserialization, Variant_BOOLEAN_DIMENSIONS)
{
  using namespace OpcUa::Binary;

  uint8_t encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  1,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type, VariantType::BOOLEAN);
  ASSERT_EQ(var.Value.Boolean.size(), 1);
  ASSERT_EQ(var.Value.Boolean[0], true);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

