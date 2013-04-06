/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef CPP_UNIT_TREE_OBJECT_TEST_H__
#define CPP_UNIT_TREE_OBJECT_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class TreeObjectTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(TreeObjectTestCase);
  CPPUNIT_TEST(TestObject);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestObject();
};


#endif
