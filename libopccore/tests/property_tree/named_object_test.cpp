/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "named_object_test.h"

#include <opc/managers/property_tree/named_object.h>

#include <cppunit/config/SourcePrefix.h>
#include <memory>

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

