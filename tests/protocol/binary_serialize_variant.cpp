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
#include <opc/ua/protocol/node_classes.h>
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

  Variant var;
  var.Type = VariantType::BOOLEAN;
  var.Value.Boolean = std::vector<bool>{true};

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

  Variant var;
  var.Type = VariantType::BOOLEAN;
  var.Value.Boolean = std::vector<bool>{true, true};

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

  Variant var;
  var.Type = VariantType::BOOLEAN;
  var.Value.Boolean = std::vector<bool>{true};
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

  ASSERT_EQ(var.Type, VariantType::NUL);
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

  ASSERT_EQ(var.Type, VariantType::BOOLEAN);
  ASSERT_EQ(var.Value.Boolean.size(), 1);
  ASSERT_EQ(var.Value.Boolean[0], true);
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

  ASSERT_EQ(var.Type, VariantType::BOOLEAN);
  ASSERT_EQ(var.Value.Boolean.size(), 2);
  ASSERT_EQ(var.Value.Boolean[0], true);
  ASSERT_EQ(var.Value.Boolean[1], true);
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

  ASSERT_EQ(var.Type, VariantType::BOOLEAN);
  ASSERT_EQ(var.Value.Boolean.size(), 1);
  ASSERT_EQ(var.Value.Boolean[0], true);
  ASSERT_FALSE(var.IsNul());
  ASSERT_FALSE(var.IsArray());
}
TEST(Variant, InitializeNUL)
{
  const OpcUa::Variant var;
  ASSERT_EQ(var.Type, OpcUa::VariantType::NUL);
  ASSERT_FALSE(var.IsArray());
  ASSERT_TRUE(var.IsNul());
}

TEST(Variant, InitializeWithBoolValue)
{
  const OpcUa::Variant var(true);
  ASSERT_EQ(var.Type, OpcUa::VariantType::BOOLEAN);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, AssignBoolValue)
{
  OpcUa::Variant var;
  var = true;
  ASSERT_EQ(var.Type, OpcUa::VariantType::BOOLEAN);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

// TODO add tests for assign and initializing of variant with different types

TEST(Variant, InitializeWithBoolVector)
{
  const std::vector<bool> vec{true, false};
  const OpcUa::Variant var(vec);
  ASSERT_EQ(var.Type, OpcUa::VariantType::BOOLEAN);
  ASSERT_TRUE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, InitializeWithString)
{
  const OpcUa::Variant var(std::string("string"));
  ASSERT_EQ(var.Type, OpcUa::VariantType::STRING);
  ASSERT_FALSE(var.IsArray());
  ASSERT_FALSE(var.IsNul());
}

TEST(Variant, InitializeWithVariant)
{
  const OpcUa::Variant var((uint32_t)OpcUa::NodeClass::Object);
  ASSERT_EQ(var.Type, OpcUa::VariantType::UINT32);
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
      case VariantType::GUID:             var.Guids = value.Guids; break;
      case VariantType::BYTE_STRING:      var.ByteString = value.ByteString; break;
      case VariantType::XML_ELEMENT:      var.String = value.String; break;
      case VariantType::NODE_ID:          var.Node = value.Node; break;
      case VariantType::EXPANDED_NODE_ID: var.Node = value.Node; break;
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
