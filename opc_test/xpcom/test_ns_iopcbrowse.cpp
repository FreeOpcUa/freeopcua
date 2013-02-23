#include "array.h"
#include "test_ns_iopcbrowse.h"
#include <cppunit/Message.h>
#include <cppunit/Exception.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>
#include <nsISupportsPrimitives.h>

#include <xpcom/mozopc.h>

#include <iostream>

namespace
{
  nsCOMPtr<nsIArray> CreateTestArray()
  {
    return nsCOMPtr<nsIArray>(new nsStlArray(std::vector<PRUint32>()));
  }

  inline bool HasMoreElements(nsISimpleEnumerator& en)
  {
    PRBool result = PR_FALSE;
    return en.HasMoreElements(&result) == NS_OK && result == PR_TRUE;
  }

  inline nsCOMPtr<nsIOPCBrowseElement> GetElement(nsISimpleEnumerator& en)
  {
    nsCOMPtr<nsISupports> element;
    CPPUNIT_ASSERT(en.GetNext(getter_AddRefs(element)) == NS_OK);
    nsCOMPtr<nsIOPCBrowseElement> result = do_QueryInterface(element);
    CPPUNIT_ASSERT(result.get());
    return result;
  }



  void CheckBrowsed(nsISimpleEnumerator& en, PRBool isItem, PRBool isBranch)
  {
    while(HasMoreElements(en))
    {
      nsCOMPtr<nsIOPCBrowseElement> element = GetElement(en);
      PRBool isBrowsedItem = PR_FALSE;
      CPPUNIT_ASSERT(element->GetIsItem(&isBrowsedItem) == NS_OK);
      if (isItem && !isBranch)
      {
        CPPUNIT_ASSERT_MESSAGE("It is browsed branch instead of item.", isBrowsedItem);
      }
      if (!isItem && isBranch)
      {
        CPPUNIT_ASSERT_MESSAGE("It is browsed item instead of branch.", !isBrowsedItem);
      }
    }
  }
  
}

void Test_nsIOPCBrowse(nsIOPCServer& server)
{
  //******************************************************
  // nsIOPCBrowse::Browse()
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "nsIOPCBrowse tests....";
  nsresult result = NS_OK;
  nsCOMPtr<nsIOPCBrowse> serverBrowse = do_QueryInterface(&server, &result);
  CPPUNIT_ASSERT(result == NS_OK);
  CPPUNIT_ASSERT(serverBrowse.get() != NULL);


  PRUnichar itemID[] = {0};
  PRUnichar nameFilter[] = {0};
  PRUnichar vendorFilter[] = {0};
  PRUint16 filter = nsIOPCBrowse::OPC_BROWSE_FILTER_ALL; //1
  nsCOMPtr<nsISimpleEnumerator> en;
  nsCOMPtr<nsIArray> array(CreateTestArray());
  // Invalid arguments
  CPPUNIT_ASSERT(serverBrowse->Browse(NULL,   filter, nameFilter, vendorFilter, true, true, array, getter_AddRefs(en)) == NS_ERROR_INVALID_ARG);
  CPPUNIT_ASSERT(en.get() == NULL);
  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, 0,      nameFilter, vendorFilter, true, true, array, getter_AddRefs(en)) == NS_ERROR_INVALID_ARG);
  CPPUNIT_ASSERT(en.get() == NULL);
  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, 4,      nameFilter, vendorFilter, true, true, array, getter_AddRefs(en)) == NS_ERROR_INVALID_ARG);
  CPPUNIT_ASSERT(en.get() == NULL);
  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, filter, NULL,       vendorFilter, true, true, array, getter_AddRefs(en)) == NS_ERROR_INVALID_ARG);
  CPPUNIT_ASSERT(en.get() == NULL);
  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, filter, nameFilter, NULL,         true, true, array, getter_AddRefs(en)) == NS_ERROR_INVALID_ARG);
  CPPUNIT_ASSERT(en.get() == NULL);
  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, filter, nameFilter, vendorFilter, true, true, array, NULL) == NS_ERROR_INVALID_POINTER);
  CPPUNIT_ASSERT(en.get() == NULL);

  // Right arguments
  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, filter, nameFilter, vendorFilter, true, true, array, getter_AddRefs(en)) == NS_OK);
  CPPUNIT_ASSERT(en.get());
  CheckBrowsed(*en, true, true);

  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, nsIOPCBrowse::OPC_BROWSE_FILTER_BRANCHES, nameFilter, vendorFilter, true, true, array, getter_AddRefs(en)) == NS_OK);
  CPPUNIT_ASSERT(en.get());
  CheckBrowsed(*en, false, true);

  CPPUNIT_ASSERT(serverBrowse->Browse(itemID, nsIOPCBrowse::OPC_BROWSE_FILTER_ITEMS, nameFilter, vendorFilter, true, true, array, getter_AddRefs(en)) == NS_OK);
  CPPUNIT_ASSERT(en.get());
  CheckBrowsed(*en, true, false);

  //******************************************************
  // nsIOPCBrowse::GetProperties()
  std::cout << "ok." << std::endl;
}

namespace
{
  void AddItemIDs(nsISimpleEnumerator& en, nsIOPCBrowse& browse, std::vector<nsString>& itemIDs)
  {
    while (HasMoreElements(en))
    {
      nsCOMPtr<nsIOPCBrowseElement> element = GetElement(en);
      CPPUNIT_ASSERT(element);
      PRUnichar* itemID = NULL;
      CPPUNIT_ASSERT(element->GetItemID(&itemID) == NS_OK);
      nsString itemIDString(itemID);
      NS_Free(itemID);
      PRBool isItem = PR_FALSE;
      CPPUNIT_ASSERT(element->GetIsItem(&isItem) == NS_OK);
      if (isItem == PR_TRUE)
      {
        itemIDs.push_back(itemIDString);
        continue;
      }

      nsCOMPtr<nsISimpleEnumerator> branchEnum;
      PRUnichar emptyString[] = { 0 };
      CPPUNIT_ASSERT(browse.Browse(itemIDString.get(), nsIOPCBrowse::OPC_BROWSE_FILTER_ALL, emptyString, emptyString, false, false, CreateTestArray(), getter_AddRefs(branchEnum)) == NS_OK);
      AddItemIDs(*branchEnum, browse, itemIDs);
    }
  }
}

std::vector<nsString> GetServerItemIDs(nsIOPCServer& server)
{
  nsCOMPtr<nsIOPCBrowse> browse = do_QueryInterface(&server);
  CPPUNIT_ASSERT(browse);

  PRUnichar emptyString[] = { 0 };
  nsCOMPtr<nsISimpleEnumerator> elements;
  CPPUNIT_ASSERT(browse->Browse(emptyString, nsIOPCBrowse::OPC_BROWSE_FILTER_ALL, emptyString, emptyString, false, false, CreateTestArray(), getter_AddRefs(elements)) == NS_OK);
  std::vector<nsString> result;
  AddItemIDs(*elements, *browse, result);
  return result;
}

