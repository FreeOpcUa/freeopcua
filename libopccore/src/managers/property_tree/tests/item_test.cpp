#include "item_test.h"
#include <cppunit/config/SourcePrefix.h>
#include <memory>
#include <sstream>

CPPUNIT_TEST_SUITE_REGISTRATION( ItemTestCase );

void ItemTestCase::setUp()
{
  ItemPtr = Gefest::Item::Create();
}

void ItemTestCase::tearDown()
{
  ItemPtr = Gefest::Item::SharedPtr();
}

void ItemTestCase::TestItemValue()
{
  CPPUNIT_ASSERT(ItemPtr->GetSelfPtr().get() == ItemPtr.get());
  const char* itemName = "item";
  ItemPtr->SetName(itemName);
  const Common::Value value1("value1");
  const std::string value1_name = "name1";

  // SetValue GetValue
  ItemPtr->SetValue(value1);
  CPPUNIT_ASSERT(ItemPtr->GetValue().Data == value1.Data && ItemPtr->GetValue().Time == value1.Time);

  // AddProperty
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->AddProperty(Gefest::Property(value1_name)));
  CPPUNIT_ASSERT_THROW(ItemPtr->AddProperty(Gefest::Property(value1_name)), Common::Error);
  // GetProperty
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->GetProperty(value1_name));
  CPPUNIT_ASSERT_THROW(ItemPtr->GetProperty("wrong_name"), Common::Error);

  // SetPropertyValue
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->SetPropertyValue(value1_name, value1));
  {
    // GetPtopery 
    const Gefest::Property prop(value1_name, value1);
    const Gefest::Property& prop1 = ItemPtr->GetProperty(value1_name);
    CPPUNIT_ASSERT(prop1.Name == prop.Name && prop1.Value.Data == prop.Value.Data && prop1.Value.Time == prop.Value.Time);
    CPPUNIT_ASSERT_THROW(ItemPtr->GetProperty("unknown_property"), Common::Error);
  }

  const Common::Value value2("value2");
  const std::string value2_name = "name2";
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->AddProperty(Gefest::Property(value2_name)));
  CPPUNIT_ASSERT_THROW(ItemPtr->AddProperty(Gefest::Property(value2_name)), Common::Error);
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->GetProperty(value2_name));

  // SetPropertyValue
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->SetPropertyValue(value2_name, value1));
  {
    // GetPtopery 
    const Gefest::Property prop(value2_name, value1);
    const Gefest::Property& prop2 = ItemPtr->GetProperty(value2_name);
    CPPUNIT_ASSERT(prop2.Name == prop.Name && prop2.Value.Data == prop.Value.Data && prop2.Value.Time == prop.Value.Time);
    CPPUNIT_ASSERT_THROW(ItemPtr->GetProperty("unknown_property"), Common::Error);
  }

  {
    // GetPropertiesIDs
    CPPUNIT_ASSERT(ItemPtr->GetPropertiesIDs().size() == 2);
    // GetPropertiesNames
    CPPUNIT_ASSERT(ItemPtr->GetPropertiesNames().size() == 2);
    const std::vector<std::string>& names = ItemPtr->GetPropertiesNames();
    CPPUNIT_ASSERT( std::find(names.begin(), names.end(), value1_name) != names.end());
    CPPUNIT_ASSERT( std::find(names.begin(), names.end(), value2_name) != names.end());
  }

  // RemoveProperty
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->RemoveProperty(value1_name));
  CPPUNIT_ASSERT_THROW(ItemPtr->GetProperty(value1_name), Common::Error);
  CPPUNIT_ASSERT_NO_THROW(ItemPtr->RemoveProperty(value2_name));
  CPPUNIT_ASSERT_THROW(ItemPtr->GetProperty(value2_name), Common::Error);

  CPPUNIT_ASSERT(ItemPtr->GetPropertiesNames().size() == 0);
  CPPUNIT_ASSERT(ItemPtr->GetPropertiesIDs().size() == 0);
}

