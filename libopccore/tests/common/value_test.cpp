/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of Common::Value
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/value.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

class ValueTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ValueTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION( ValueTestCase );

void ValueTestCase::Test()
{
  Common::Value value;
  std::vector<Common::Value> values;
}
