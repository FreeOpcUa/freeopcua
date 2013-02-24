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

#include <opccore/managers/property_tree/item.h>

#include <cppunit/extensions/HelperMacros.h>
#include <boost/shared_ptr.hpp>


class ItemTestCase : public CPPUNIT_NS::TestFixture
{
public:
  CPPUNIT_TEST_SUITE(ItemTestCase);
  CPPUNIT_TEST(TestItemValue);
  CPPUNIT_TEST_SUITE_END();

public:
  virtual void setUp();
  virtual void tearDown();

protected:
  void TestItemValue();

private:
  Gefest::Item::SharedPtr ItemPtr;
};

#endif
