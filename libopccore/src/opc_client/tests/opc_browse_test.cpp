/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of OpcBrowse inteerfaces
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "property_tree_builder.h"


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

#include <opc/common/exception.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/opc_client/server_browser.h>
#include <opc/opc_client/server.h>

#include <tr1/functional>

class OpcBrowseTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(OpcBrowseTestCase);
  CPPUNIT_TEST(TestBrowsingAllElementsForRoot);
  CPPUNIT_TEST(TestBrowsingAllElementsOfSubGroup1);
  CPPUNIT_TEST(TestBrowsingGroupsForRoot);
  CPPUNIT_TEST(TestBrowsingAllGroupsOfSubGroup1);
  CPPUNIT_TEST(TestBrowsingItemsForRoot);
  CPPUNIT_TEST(TestBrowsingItemsForGroup1);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

protected:
  void TestBrowsingAllElementsForRoot();
  void TestBrowsingAllElementsOfSubGroup1();
  void TestBrowsingGroupsForRoot();
  void TestBrowsingAllGroupsOfSubGroup1();
  void TestBrowsingItemsForRoot();
  void TestBrowsingItemsForGroup1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( OpcBrowseTestCase );

void OpcBrowseTestCase::setUp()
{
}

void OpcBrowseTestCase::tearDown()
{
}

std::ostream& operator<< (std::ostream& out, const OpcClient::BrowseElement& el)
{
  out << "OpcClient::BrowseElement:" << std::endl;
  out << "  Name: " << el.Name << std::endl;
  out << "  ID: " << el.ID << std::endl;
  out << "  IsItem: " << el.IsItem << std::endl;
  out << "  HasChildren: " << el.HasChildren << std::endl;
  return out;
}

std::ostream& operator<< (std::ostream& out, const std::vector<OpcClient::BrowseElement>& elements)
{
  for (std::vector<OpcClient::BrowseElement>::const_iterator it = elements.begin(); it != elements.end(); ++it)
  {
    out << *it;
  }
  return out;
}


inline OpcClient::BrowseElement GetElement(
  const std::string& name,
  const std::string& id,
  bool hasChildren,
  bool isItem)
{
  OpcClient::BrowseElement element;
  element.Name = name;
  element.ID = id;
  element.HasChildren = hasChildren;
  element.IsItem = isItem;
  //<Property> Properties;
  return element;
}

// TODO Add Propertites to the result Group
inline OpcClient::BrowseElement GetGroupElement(
  const std::string& name,
  const std::string& id,
  bool hasChildren)
{
  return GetElement(name, id, hasChildren, false);
}

// TODO Add Propertites to the result Item
inline OpcClient::BrowseElement GetItemElement(
  const std::string& name,
  const std::string& id)
{
  return GetElement(name, id, false, true);
}


bool Compare (const OpcClient::Property& el1, const OpcClient::Property& el2)
{
  // TODO add comparing of properties.
  return el1.ID == el2.ID &&
  el1.ItemID == el2.ItemID;
}

bool CompareProperties(const std::vector<OpcClient::Property>& el1, const std::vector<OpcClient::Property>& el2)
{
  if(el1.size() != el2.size())
  {
    return false;
  }
  typedef std::vector<OpcClient::Property> ElementVector;
  for (ElementVector::const_iterator it = el1.begin(); it != el1.end(); ++it)
  {
    const OpcClient::Property& el = *it;
    if (std::find_if(el2.begin(), el2.end(), std::tr1::bind(&Compare, std::tr1::placeholders::_1, std::tr1::cref(el))) == el2.end())
    {
      return false;
    }
  }
  return true;
}


bool CompareElements (const OpcClient::BrowseElement& el1, const OpcClient::BrowseElement& el2)
{
  return el1.Name == el2.Name &&
  el1.ID == el2.ID &&
  el1.HasChildren == el2.HasChildren &&
  el1.IsItem == el2.IsItem &&
  CompareProperties(el1.Properties, el2.Properties);
}

// Compare lists of browsed elements and expected elements
void CheckBrowsedElements(const std::vector<OpcClient::BrowseElement>& el1, const std::vector<OpcClient::BrowseElement>& el2)
{
  CPPUNIT_ASSERT(el1.size() == el2.size());
  typedef std::vector<OpcClient::BrowseElement> ElementVector;
  for (ElementVector::const_iterator it = el1.begin(); it != el1.end(); ++it)
  {
    const OpcClient::BrowseElement& el = *it;
    if (std::find_if(el2.begin(), el2.end(), 
      std::tr1::bind(
        &CompareElements, 
        std::tr1::placeholders::_1, 
        std::tr1::cref(el))) 
      == el2.end())
    {
      std::cout << std::endl << "Element \"" << it->ID << "\" not found";
      std::cout << "Browsed elements:" << std::endl << el1 << std::endl;
      CPPUNIT_ASSERT(false);
    }
  }
}

/*
  enum BrowseFilter
  {
    OPC_BROWSE_FILTER_ALL      = 1,
    OPC_BROWSE_FILTER_BRANCHES = 2,
    OPC_BROWSE_FILTER_ITEMS    = 3,
  };
*/

/////////////////////////////////////////////////////////////////////////////////
// Browsing all SubGroups and all sub items of root group
// It should be returned 7 elements: 4 sub groups and 3 items
////////////////////////////////////////////////////////////////////////////////

inline std::vector<OpcClient::BrowseElement> GetAllElementsForRoot()
{
  std::vector<OpcClient::BrowseElement> result;
  result.push_back(GetGroupElement("Group1", "/Group1", true));
  result.push_back(GetGroupElement("Group2", "/Group2", false));
  result.push_back(GetGroupElement("Group3", "/Group3", false));
  result.push_back(GetGroupElement("Group4", "/Group4", false));

  result.push_back(GetItemElement("Item1", "/Item1"));
  result.push_back(GetItemElement("Item2", "/Item2"));
  result.push_back(GetItemElement("Item3", "/Item3"));
  return result;
}

void OpcBrowseTestCase::TestBrowsingAllElementsForRoot()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();
  const std::unique_ptr<OpcClient::ServerBrowser> browser(OpcClient::CreateServerBrowser());
  CPPUNIT_ASSERT(browser.get());
  const std::vector<OpcClient::BrowseElement>& browsedElements = browser->Browse(
    "",
    OpcClient::OPC_BROWSE_FILTER_ALL,
    "",
    "",
    true,
    false,
    std::vector<Common::ObjectID>());

  if (browsedElements.size() != 7)
  {
    std::cout << std::endl << "Count of browsed elements: " << browsedElements.size() << std::endl;
    // It should be returned 7 elements: 4 groups and 3 items
    CPPUNIT_ASSERT(browsedElements.size() == 7);
  }
  CheckBrowsedElements(browsedElements, GetAllElementsForRoot());
  StopTests();
}

/////////////////////////////////////////////////////////////////////////////////
// all SubGroups and all sub items for group "/Group1"
// It should be returned 7 elements: 4 sub groups and 3 items
////////////////////////////////////////////////////////////////////////////////

inline std::vector<OpcClient::BrowseElement> GetAllElementsForSubGroup1()
{
  std::vector<OpcClient::BrowseElement> result;
  result.push_back(GetGroupElement("Group1", "/Group1/Group1", false));
  result.push_back(GetGroupElement("Group2", "/Group1/Group2", false));
  result.push_back(GetGroupElement("Group3", "/Group1/Group3", false));
  result.push_back(GetGroupElement("Group4", "/Group1/Group4", false));

  result.push_back(GetItemElement("Item1", "/Group1/Item1"));
  result.push_back(GetItemElement("Item2", "/Group1/Item2"));
  result.push_back(GetItemElement("Item3", "/Group1/Item3"));
  return result;
}


// Test browsing elements of group /Group1
void OpcBrowseTestCase::TestBrowsingAllElementsOfSubGroup1()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();
  const std::unique_ptr<OpcClient::ServerBrowser> browser(OpcClient::CreateServerBrowser());
  CPPUNIT_ASSERT(browser.get());
  const std::vector<OpcClient::BrowseElement>& browsedElements = browser->Browse(
    "/Group1",
    OpcClient::OPC_BROWSE_FILTER_ALL,
    "",
    "",
    true,
    false,
    std::vector<Common::ObjectID>());

  // It should be returned 7: 4 subgroups and 3 items
  if (browsedElements.size() != 7)
  {
    std::cout << std::endl << "Count of browsed elements: " << browsedElements.size() << std::endl;
  }
  CPPUNIT_ASSERT(browsedElements.size() == 7);
  CheckBrowsedElements(browsedElements, GetAllElementsForSubGroup1());
  StopTests();
}

/////////////////////////////////////////////////////////////////////////////////
// test of browsing Groups at root group
/////////////////////////////////////////////////////////////////////////////////

inline std::vector<OpcClient::BrowseElement> GetAllGroupsForRoot()
{
  std::vector<OpcClient::BrowseElement> result;
  result.push_back(GetGroupElement("Group1", "/Group1", true));
  result.push_back(GetGroupElement("Group2", "/Group2", false));
  result.push_back(GetGroupElement("Group3", "/Group3", false));
  result.push_back(GetGroupElement("Group4", "/Group4", false));
  return result;
}

void OpcBrowseTestCase::TestBrowsingGroupsForRoot()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();
  const std::unique_ptr<OpcClient::ServerBrowser> browser(OpcClient::CreateServerBrowser());
  CPPUNIT_ASSERT(browser.get());
  const std::vector<OpcClient::BrowseElement>& browsedElements = browser->Browse(
    "",
    OpcClient::OPC_BROWSE_FILTER_BRANCHES,
    "",
    "",
    true,
    false,
    std::vector<Common::ObjectID>());

  if (browsedElements.size() != 4)
  {
    std::cout << std::endl << "Count of browsed elements: " << browsedElements.size() << std::endl;
    // It should be returned 4 groups
    CPPUNIT_ASSERT(browsedElements.size() == 4);
  }
  CheckBrowsedElements(browsedElements, GetAllGroupsForRoot());
  StopTests();
}

/////////////////////////////////////////////////////////////////////////////////
// test of browsing Groups at group "/Group1"
/////////////////////////////////////////////////////////////////////////////////

inline std::vector<OpcClient::BrowseElement> GetAllGroupsForSubGroup1()
{
  std::vector<OpcClient::BrowseElement> result;
  result.push_back(GetGroupElement("Group1", "/Group1/Group1", false));
  result.push_back(GetGroupElement("Group2", "/Group1/Group2", false));
  result.push_back(GetGroupElement("Group3", "/Group1/Group3", false));
  result.push_back(GetGroupElement("Group4", "/Group1/Group4", false));
  return result;
}


// Test browsing elements of group /Group1
void OpcBrowseTestCase::TestBrowsingAllGroupsOfSubGroup1()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();
  const std::unique_ptr<OpcClient::ServerBrowser> browser(OpcClient::CreateServerBrowser());
  CPPUNIT_ASSERT(browser.get());
  const std::vector<OpcClient::BrowseElement>& browsedElements = browser->Browse(
    "/Group1",
    OpcClient::OPC_BROWSE_FILTER_BRANCHES,
    "",
    "",
    true,
    false,
    std::vector<Common::ObjectID>());

  // It should be returned 4 subgroups
  if (browsedElements.size() != 4)
  {
    std::cout << std::endl << "Count of browsed elements: " << browsedElements.size() << std::endl;
  }
  CPPUNIT_ASSERT(browsedElements.size() == 4);
  CheckBrowsedElements(browsedElements, GetAllGroupsForSubGroup1());
  StopTests();
}

/////////////////////////////////////////////////////////////////////////////////
// test of browsing Items at group "/Group1"
/////////////////////////////////////////////////////////////////////////////////

inline std::vector<OpcClient::BrowseElement> GetAllItemsForRoot()
{
  std::vector<OpcClient::BrowseElement> result;
  result.push_back(GetItemElement("Item1", "/Item1"));
  result.push_back(GetItemElement("Item2", "/Item2"));
  result.push_back(GetItemElement("Item3", "/Item3"));
  return result;
}

void OpcBrowseTestCase::TestBrowsingItemsForRoot()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();
  const std::unique_ptr<OpcClient::ServerBrowser> browser(OpcClient::CreateServerBrowser());
  CPPUNIT_ASSERT(browser.get());
  const std::vector<OpcClient::BrowseElement>& browsedElements = browser->Browse(
    "",
    OpcClient::OPC_BROWSE_FILTER_ITEMS,
    "",
    "",
    true,
    false,
    std::vector<Common::ObjectID>());

  if (browsedElements.size() != 3)
  {
    std::cout << std::endl << "Count of browsed elements: " << browsedElements.size() << std::endl;
    // It should be returned 3 items
    CPPUNIT_ASSERT(browsedElements.size() == 3);
  }
  CheckBrowsedElements(browsedElements, GetAllItemsForRoot());
  StopTests();
}

/////////////////////////////////////////////////////////////////////////////////
// test of browsing items at group "/Group1"
/////////////////////////////////////////////////////////////////////////////////

inline std::vector<OpcClient::BrowseElement> GetAllItemsForGroup1()
{
  std::vector<OpcClient::BrowseElement> result;
  result.push_back(GetItemElement("Item1", "/Group1/Item1"));
  result.push_back(GetItemElement("Item2", "/Group1/Item2"));
  result.push_back(GetItemElement("Item3", "/Group1/Item3"));
  return result;
}

void OpcBrowseTestCase::TestBrowsingItemsForGroup1()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();
  const std::unique_ptr<OpcClient::ServerBrowser> browser(OpcClient::CreateServerBrowser());
  CPPUNIT_ASSERT(browser.get());
  const std::vector<OpcClient::BrowseElement>& browsedElements = browser->Browse(
    "/Group1",
    OpcClient::OPC_BROWSE_FILTER_ITEMS,
    "",
    "",
    true,
    false,
    std::vector<Common::ObjectID>());

  if (browsedElements.size() != 3)
  {
    std::cout << std::endl << "Count of browsed elements: " << browsedElements.size() << std::endl;
    // It should be returned 3 items
    CPPUNIT_ASSERT(browsedElements.size() == 3);
  }
  CheckBrowsedElements(browsedElements, GetAllItemsForGroup1());
  StopTests();
}
