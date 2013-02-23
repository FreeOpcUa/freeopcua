#include "named_object_test.h"
#include <cppunit/config/SourcePrefix.h>
#include <memory>
#include <opccore/managers/property_tree/named_object.h>

CPPUNIT_TEST_SUITE_REGISTRATION( NamedObjectTestCase );

namespace
{
  const char OBJECT_NAME[] = "object_name1";
}

void NamedObjectTestCase::TestNamedObject()
{
  Gefest::NamedObject object;
  object.SetName(OBJECT_NAME);
  CPPUNIT_ASSERT(object.GetName() == OBJECT_NAME);
  CPPUNIT_ASSERT(object.GetID() != Common::ObjectID());
}

