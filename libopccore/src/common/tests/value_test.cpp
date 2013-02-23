/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of Common::Value
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

#include <opccore/common/value.h>

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
