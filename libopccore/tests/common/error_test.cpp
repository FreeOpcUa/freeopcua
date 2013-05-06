/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of Common::Error
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/exception.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>
#include <iostream>

class ErrorTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ErrorTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION( ErrorTestCase );

void ErrorTestCase::Test()
{
  Common::Error(1, "test_file", 2, "test_message");
}
