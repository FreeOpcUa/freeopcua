/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary attributes.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>

#include <algorithm>
#include <stdexcept>

//-------------------------------------------------------
// Serialization
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Variant_NUL)
{

  using namespace OpcUa;
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

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  Variant var = true;

  GetStream() << var << flush;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN);
  const std::vector<char> expectedData = {
  encodingMask,
  1
  };

  ASSERT_EQ(expectedData.size(), RawSize(var));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinarySerialization, Variant_BOOLEAN_Array)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  Variant var = std::vector<bool>{true, true};

  GetStream() << var << flush;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_ARRAY_MASK;
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

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  Variant var = true;
  var.Dimensions.push_back(1);

  GetStream() << var << flush;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_DIMENSIONS_MASK;
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
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::NUL);
  ASSERT_TRUE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

TEST_F(OpcUaBinaryDeserialization, Variant_BOOLEAN)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN);

  const std::vector<char> expectedData = {
  encodingMask,
  1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BOOLEAN);
  ASSERT_TRUE(var.As<bool>());
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

TEST_F(OpcUaBinaryDeserialization, Variant_BOOLEAN_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  1,
  1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BOOLEAN);
  std::vector<bool> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<bool>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], true);
  ASSERT_EQ(vals[1], true);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
}


TEST_F(OpcUaBinaryDeserialization, Variant_BOOLEAN_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  1,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BOOLEAN);
  ASSERT_TRUE(var.As<bool>());
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}
///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_BYTE)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::BYTE);

  const std::vector<char> expectedData = {
    encodingMask,
    (char)200
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BYTE);
  ASSERT_EQ(var.As<uint8_t>(), 200);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

TEST_F(OpcUaBinaryDeserialization, Variant_BYTE_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BYTE) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  (char)200,
  (char)201
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BYTE);
  std::vector<uint8_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<uint8_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 200);
  ASSERT_EQ(vals[1], 201);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_BYTE_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BYTE) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  (char)200,
  2,0,0,0,
  2,0,0,0,
  3,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BYTE);
  ASSERT_EQ(var.As<uint8_t>(), 200);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 2);
  ASSERT_EQ(var.Dimensions[0], 2);
  ASSERT_EQ(var.Dimensions[1], 3);
}
///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_SBYTE)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::SBYTE);

  const std::vector<char> expectedData = {
    encodingMask,
    (char)-5
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::SBYTE);
  ASSERT_EQ(var.As<int8_t>(), -5);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

TEST_F(OpcUaBinaryDeserialization, Variant_SBYTE_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::SBYTE) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  -2,
  -3
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::SBYTE);
  std::vector<int8_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<int8_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], -2);
  ASSERT_EQ(vals[1], -3);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_SBYTE_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::SBYTE) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  -5,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::SBYTE);
  ASSERT_EQ(var.As<int8_t>(), -5);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_INT16)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::INT16);

  const std::vector<char> expectedData = {
    encodingMask,
    0x00, 0x20 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT16);
  ASSERT_EQ(var.As<int16_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_INT16_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::INT16) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0x00, 0x20, // 0x2000
  0x00, 0x20  // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT16);
  std::vector<int16_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<int16_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 8192);
  ASSERT_EQ(vals[1], 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_INT16_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::INT16) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0x00, 0x20, // 0x2000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT16);
  ASSERT_EQ(var.As<int16_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_UINT16)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::UINT16);

  const std::vector<char> expectedData = {
    encodingMask,
    0x00, 0x20 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT16);
  ASSERT_EQ(var.As<uint16_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_UINT16_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::UINT16) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0x00, 0x20, // 0x2000
  0x00, 0x20  // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT16);
  std::vector<uint16_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<uint16_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 8192);
  ASSERT_EQ(vals[1], 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_UINT16_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::UINT16) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0x00, 0x20, // 0x2000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT16);
  ASSERT_EQ(var.As<uint16_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_INT32)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::INT32);

  const std::vector<char> expectedData = {
    encodingMask,
    0x00,0x20,0,0 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT32);
  ASSERT_EQ(var.As<int32_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_INT32_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::INT32) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0x00, 0x20, 0, 0, // 0x2000
  0x00, 0x20, 0, 0  // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT32);
  std::vector<int32_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<int32_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 8192);
  ASSERT_EQ(vals[1], 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_INT32_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::INT32) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0x00, 0x20, 0, 0,// 0x2000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT32);
  ASSERT_EQ(var.As<int32_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_UINT32)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::UINT32);

  const std::vector<char> expectedData = {
    encodingMask,
    0x00, 0x20,0,0 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT32);
  ASSERT_EQ(var.As<uint32_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_UINT32_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::UINT32) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0x00, 0x20,0,0, // 0x2000
  0x00, 0x20,0,0  // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT32);
  std::vector<uint32_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<uint32_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 8192);
  ASSERT_EQ(vals[1], 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_UINT32_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::UINT32) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0x00, 0x20,0,0, // 0x2000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT32);
  ASSERT_EQ(var.As<uint32_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_INT64)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::INT64);

  const std::vector<char> expectedData = {
    encodingMask,
    0x00,0x20,0,0, 0,0,0,0 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT64);
  ASSERT_EQ(var.As<int64_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_INT64_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::INT64) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0x00,0x20,0,0, 0,0,0,0, // 0x2000
  0x00,0x20,0,0, 0,0,0,0  // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT64);
  std::vector<int64_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<int64_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 8192);
  ASSERT_EQ(vals[1], 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_INT64_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::INT64) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0x00,0x20,0,0, 0,0,0,0,// 0x2000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::INT64);
  ASSERT_EQ(var.As<int64_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_UINT64)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::UINT64);

  const std::vector<char> expectedData = {
    encodingMask,
    0x00,0x20,0,0, 0,0,0,0 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT64);
  ASSERT_EQ(var.As<uint64_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_UINT64_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::UINT64) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0x00,0x20,0,0, 0,0,0,0, // 0x2000
  0x00,0x20,0,0, 0,0,0,0 // 0x2000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT64);
  std::vector<uint64_t> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<uint64_t>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 8192);
  ASSERT_EQ(vals[1], 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_UINT64_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::UINT64) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0x00,0x20,0,0, 0,0,0,0, // 0x2000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::UINT64);
  ASSERT_EQ(var.As<uint64_t>(), 8192);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_FLOAT)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::FLOAT);

  const std::vector<char> expectedData = {
    encodingMask,
    //(char)0xC0, (char)0xD0, 0, 0 // -6.5
    0, 0, (char)0xD0, (char)0xC0 // -6.5
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::FLOAT);
  ASSERT_EQ(var.As<float>(), -6.5);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_FLOAT_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::FLOAT) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0, 0, (char)0xD0, (char)0xC0, // -6.5
  0, 0, (char)0xD0, (char)0xC0 // -6.5
  //(char)0xC0, (char)0xD0, 0, 0, // -6.5
  //(char)0xC0, (char)0xD0, 0, 0 // -6.5
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::FLOAT);
  std::vector<float> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<float>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], -6.5);
  ASSERT_EQ(vals[1], -6.5);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_FLOAT_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::FLOAT) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0, 0, (char)0xD0, (char)0xC0, // -6.5
  //(char)0xC0, (char)0xD0, 0, 0, // -6.5
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::FLOAT);
  ASSERT_EQ(var.As<float>(), -6.5);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_DOUBLE)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::DOUBLE);

  const std::vector<char> expectedData = {
    encodingMask,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41 // 1200000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::DOUBLE);
  ASSERT_EQ(var.As<double>(), 1200000);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_DOUBLE_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::DOUBLE) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41, // 1200000
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41  // 1200000
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::DOUBLE);
  std::vector<double> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<double>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 1200000);
  ASSERT_EQ(vals[1], 1200000);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_DOUBLE_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::DOUBLE) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41, // 1200000
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::DOUBLE);
  ASSERT_EQ(var.As<double>(), 1200000);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

OpcUa::Guid CreateTestGuid()
{
  OpcUa::Guid guid;
  guid.Data1 = 0x01020304;
  guid.Data2 = 0x0506;
  guid.Data3 = 0x0708;

  for (unsigned i = 0; i < 8; ++i)
  {
    guid.Data4[i] = i + 1;
  }
  return guid;
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_GUId)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::GUId);

  const std::vector<char> expectedData = {
    encodingMask,
    4,3,2,1, 6,5, 8,7, 1,2,3,4,5,6,7,8
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  OpcUa::Guid guid = CreateTestGuid();

  ASSERT_EQ(var.Type(), VariantType::GUId);
  ASSERT_EQ(var.As<Guid>(), guid);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_GUId_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::GUId) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  4,3,2,1, 6,5, 8,7, 1,2,3,4,5,6,7,8,
  4,3,2,1, 6,5, 8,7, 1,2,3,4,5,6,7,8
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  OpcUa::Guid guid = CreateTestGuid();

  ASSERT_EQ(var.Type(), VariantType::GUId);
  std::vector<Guid> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<Guid>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], guid);
  ASSERT_EQ(vals[1], guid);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_GUId_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::GUId) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  4,3,2,1, 6,5, 8,7, 1,2,3,4,5,6,7,8,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  OpcUa::Guid guid = CreateTestGuid();

  ASSERT_EQ(var.Type(), VariantType::GUId);
  ASSERT_EQ(var.As<Guid>(), guid);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_STRING)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::STRING);

  const std::vector<char> expectedData = {
    encodingMask,
    4,0,0,0, 'R','o','o','t'
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::STRING);
  ASSERT_EQ(var.As<std::string>(), std::string("Root"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_STRING_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::STRING) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  4,0,0,0, 'R','o','o','t',
  4,0,0,0, 'R','o','o','t'
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  OpcUa::Guid guid = CreateTestGuid();

  ASSERT_EQ(var.Type(), VariantType::STRING);
  std::vector<std::string> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<std::string>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], std::string("Root"));
  ASSERT_EQ(vals[1], std::string("Root"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_STRING_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::STRING) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  4,0,0,0, 'R','o','o','t',
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  OpcUa::Guid guid = CreateTestGuid();

  ASSERT_EQ(var.Type(), VariantType::STRING);
  ASSERT_EQ(var.As<std::string>(), std::string("Root"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_BYTE_STRING)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::BYTE_STRING);

  const std::vector<char> expectedData = {
    encodingMask,
    2,0,0,0, 1,2
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BYTE_STRING);
  ASSERT_EQ(var.As<ByteString>(), ByteString(std::vector<uint8_t>{1,2}));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_BYTE_STRING_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BYTE_STRING) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  2,0,0,0, 1,2,
  2,0,0,0, 1,2
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::BYTE_STRING);
  std::vector<ByteString> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<ByteString>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], ByteString(std::vector<uint8_t>{1,2}));
  ASSERT_EQ(vals[1], ByteString(std::vector<uint8_t>{1,2}));
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_BYTE_STRING_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BYTE_STRING) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0, 1,2,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  OpcUa::Guid guid = CreateTestGuid();

  ASSERT_EQ(var.Type(), VariantType::BYTE_STRING);
  ASSERT_EQ(var.As<ByteString>(), ByteString(std::vector<uint8_t>{1,2}));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_DATETIME)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::DATE_TIME);

  const std::vector<char> expectedData = {
    encodingMask,
    8, 7, 6, 5, 4, 3, 2, 1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::DATE_TIME);
  ASSERT_EQ(var.As<DateTime>(), 0x0102030405060708);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_DATE_TIME_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::DATE_TIME) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  8, 7, 6, 5, 4, 3, 2, 1,
  8, 7, 6, 5, 4, 3, 2, 1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::DATE_TIME);
  std::vector<DateTime> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<DateTime>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], 0x0102030405060708);
  ASSERT_EQ(vals[1], 0x0102030405060708);
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_DATE_TIME_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::DATE_TIME) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  8, 7, 6, 5, 4, 3, 2, 1,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::DATE_TIME);
  ASSERT_EQ(var.As<DateTime>(), 0x0102030405060708);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_LOCALIZED_TEXT)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::LOCALIZED_TEXT);

  const std::vector<char> expectedData = {
    encodingMask,
    2, 4,0,0,0,'t','e','x','t'
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(var.As<LocalizedText>(), LocalizedText("text"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_LOCALIZED_TEXT_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::LOCALIZED_TEXT) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  2, 4,0,0,0,'t','e','x','t',
  2, 4,0,0,0,'t','e','x','t'
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::LOCALIZED_TEXT);
  std::vector<LocalizedText> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<LocalizedText>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], LocalizedText("text"));
  ASSERT_EQ(vals[1], LocalizedText("text"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_LOCALIZED_TEXT_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::LOCALIZED_TEXT) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2, 4,0,0,0,'t','e','x','t',
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(var.As<LocalizedText>(), LocalizedText("text"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_QUALIFIED_NAME)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::QUALIFIED_NAME);

  const std::vector<char> expectedData = {
    encodingMask,
    1,0,  4,0,0,0,  'n','a','m','e'
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::QUALIFIED_NAME);
  ASSERT_EQ(var.As<QualifiedName>(), QualifiedName(1, "name"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_QUALIFIED_NAME_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::QUALIFIED_NAME) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  1,0,  4,0,0,0,  'n','a','m','e',
  1,0,  4,0,0,0,  'n','a','m','e'
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::QUALIFIED_NAME);
  std::vector<QualifiedName> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<QualifiedName>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], QualifiedName(1, "name"));
  ASSERT_EQ(vals[1], QualifiedName(1, "name"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}


TEST_F(OpcUaBinaryDeserialization, Variant_QUALIFIED_NAME_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::QUALIFIED_NAME) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  1,0,  4,0,0,0,  'n','a','m','e',
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::QUALIFIED_NAME);
  ASSERT_EQ(var.As<QualifiedName>(), QualifiedName(1, "name"));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------
TEST_F(OpcUaBinaryDeserialization, Variant_NODE_Id)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<char>(VariantType::NODE_Id);

  const std::vector<char> expectedData = {
    encodingMask,
    1, 0, (char)0xC1, 0x1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::NODE_Id);
  ASSERT_EQ(var.As<NodeId>(), OpcUa::FourByteNodeId(449));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_NODE_Id_Array)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::NODE_Id) | HAS_ARRAY_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  2,0,0,0,
  1, 0, (char)0xC1, 0x1,
  1, 0, (char)0xC1, 0x1
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::NODE_Id);
  std::vector<NodeId> vals;
  ASSERT_NO_THROW(vals = var.As<std::vector<NodeId>>());
  ASSERT_EQ(vals.size(), 2);
  ASSERT_EQ(vals[0], OpcUa::FourByteNodeId(449));
  ASSERT_EQ(vals[1], OpcUa::FourByteNodeId(449));
  ASSERT_FALSE(var.IsNul());
  ASSERT_TRUE(var.IsArray());
  ASSERT_TRUE(var.Dimensions.empty());
}

TEST_F(OpcUaBinaryDeserialization, Variant_NODE_Id_DIMENSIONS)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::NODE_Id) | HAS_DIMENSIONS_MASK;

  const std::vector<char> expectedData = {
  encodingMask,
  1, 0, (char)0xC1, 0x1,
  1,0,0,0,
  1,0,0,0,
  };

  GetChannel().SetData(expectedData);

  Variant var;
  GetStream() >> var;

  ASSERT_EQ(var.Type(), VariantType::NODE_Id);
  ASSERT_EQ(var.As<NodeId>(), OpcUa::FourByteNodeId(449));
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
  ASSERT_EQ(var.Dimensions.size(), 1);
  ASSERT_EQ(var.Dimensions[0], 1);
}

///-----------------------------------------------------------------------------

TEST(Variant, InitializeNUL)
{
  const OpcUa::Variant var;
  ASSERT_EQ(var.Type(), OpcUa::VariantType::NUL);
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.IsNul());
}

TEST(Variant, InitializeWithBoolValue)
{
  const OpcUa::Variant var(true);
  ASSERT_EQ(var.Type(), OpcUa::VariantType::BOOLEAN);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, AssignBoolValue)
{
  OpcUa::Variant var;
  var = true;
  ASSERT_EQ(var.Type(), OpcUa::VariantType::BOOLEAN);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

// TODO add tests for assign and initializing of variant with different types

TEST(Variant, InitializeWithBoolVector)
{
  const std::vector<bool> vec{true, false};
  const OpcUa::Variant var(vec);
  ASSERT_EQ(var.Type(), OpcUa::VariantType::BOOLEAN);
  ASSERT_TRUE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, InitializeWithString)
{
  const OpcUa::Variant var(std::string("string"));
  ASSERT_EQ(var.Type(), OpcUa::VariantType::STRING);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, InitializeWithVariant)
{
  const OpcUa::Variant var((uint32_t)OpcUa::NodeClass::Object);
  ASSERT_EQ(var.Type(), OpcUa::VariantType::UINT32);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, InitializeObjectId)
{
  const OpcUa::Variant var = OpcUa::ObjectId::RootFolder;
  ASSERT_EQ(var.Type(), OpcUa::VariantType::NODE_Id);
  ASSERT_EQ(var, OpcUa::ObjectId::RootFolder);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, InitializeMessageId)
{
  const OpcUa::Variant var = OpcUa::MessageId::READ_RESPONSE;
  ASSERT_EQ(var.Type(), OpcUa::VariantType::NODE_Id);
  ASSERT_EQ(var, OpcUa::MessageId::READ_RESPONSE);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

/*

TEST(Variant, InitializeWithBoolInitializerList)
{
  const OpcUa::Variant var{true};
  ASSERT_EQ(var.Type, OpcUa::VariantType::BOOLEAN);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}



      case VariantType::BOOLEAN:          var.Boolean = value.Boolean; break;
      case VariantType::SBYTE:            var.SByte = value.SByte; break;
      case VariantType::BYTE:             var.Byte = value.Byte; break;
      case VariantType::INT16:            var.Int16 = value.Int16; break;
      case VariantType::UINT16:           var.UInt16 = value.UInt16; break;
      case VariantType::INT32:            var.Int32 = value.Int32; break;
      case VariantType::UINT32:           var.UInt32 = value.UInt32; break;
      case VariantType::INT64:            var.Int64 = value.Int64; break;
      case VariantType::UINT64:           var.UInt64 = value.UInt64; break;
      case VariantType::FLOAT:            var.Float = value.Float; break;
      case VariantType::DOUBLE:           var.Double = value.Double; break;
      case VariantType::STRING:           var.String = value.String; break;
      case VariantType::DATE_TIME:        var.Time = value.Time; break;
      case VariantType::GUId:             var.Guids = value.Guids; break;
      case VariantType::BYTE_STRING:      var.ByteString = value.ByteString; break;
      case VariantType::XML_ELEMENT:      var.String = value.String; break;
      case VariantType::NODE_Id:          var.Node = value.Node; break;
      case VariantType::EXPANDED_NODE_Id: var.Node = value.Node; break;
      case VariantType::STATUS_CODE:      var.StatusCode = value.StatusCode; break;
      case VariantType::QUALIFIED_NAME:   var.Name = value.Name; break;
      case VariantType::LOCALIZED_TEXT:   var.Text = value.Text; break;
      case VariantType::DIAGNOSTIC_INFO:  var.Diagnostic = value.Diagnostic; break;
      case VariantType::VARIANT:          var.Variants = value.Variants; break;
      case VariantType::DATA_VALUE:       var.Value = value.Value; break;
 */

TEST(Variant, Comparable)
{
  ASSERT_EQ(OpcUa::Variant(), OpcUa::Variant());

  ASSERT_EQ(OpcUa::Variant(1), OpcUa::Variant(1));
  ASSERT_EQ(OpcUa::Variant(1), 1);
  ASSERT_NE(OpcUa::Variant(1), 2);

  ASSERT_EQ(OpcUa::Variant(true), OpcUa::Variant(true));
  ASSERT_EQ(OpcUa::Variant(true), true);

  ASSERT_NE(OpcUa::Variant(true), OpcUa::Variant(false));
  ASSERT_NE(OpcUa::Variant(true), false);
}
