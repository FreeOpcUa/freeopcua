/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Manager of Property tree
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef CPP_UNIT_EXAMPLETESTCASE_H
#define CPP_UNIT_EXAMPLETESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class GroupTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(GroupTestCase);
  CPPUNIT_TEST(TestGroup);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestGroup();
};


#endif
