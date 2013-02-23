
#ifndef CPP_UNIT_EXAMPLETESTCASE_H
#define CPP_UNIT_EXAMPLETESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class NamedObjectTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(NamedObjectTestCase);
  CPPUNIT_TEST(TestNamedObject);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestNamedObject();
};


#endif
