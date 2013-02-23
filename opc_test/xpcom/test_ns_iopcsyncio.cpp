/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of nsIOPCSyncIO interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "test_ns_iopcsyncio.h"

#include "array.h"

#include <cppunit/Message.h>
#include <cppunit/Exception.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>
#include <iostream>
#include <nsComponentManagerUtils.h>
#include <xpcom/mozopc.h>


namespace
{

  inline bool HasMoreElements(nsISimpleEnumerator& en)
  {
    PRBool hasMoreElements = PR_FALSE;
    CPPUNIT_ASSERT(en.HasMoreElements(&hasMoreElements) == NS_OK);
    return hasMoreElements == PR_TRUE;
  }

  inline nsCOMPtr<nsISupports> GetNextElement(nsISimpleEnumerator& enumerator)
  {
    nsCOMPtr<nsISupports> resultPtr;
    CPPUNIT_ASSERT(enumerator.GetNext(getter_AddRefs(resultPtr)) == NS_OK);
    return resultPtr;
  }

  std::vector<PRUint32> AddItems(nsISupports& supports, const std::vector<nsString>& itemIDs)
  {
    nsCOMPtr<nsIOPCGroupInterfacesFactory> factory = do_QueryInterface(&supports);
    nsCOMPtr<nsIOPCItemMgt> itemMgt = do_QueryInterface(&supports);
    CPPUNIT_ASSERT(itemMgt);
    std::vector<nsCOMPtr<nsISupports>> vec;
    nsCOMPtr<nsIOPCItemDef> itemDef;

    for (const nsString& itemID : itemIDs)
    {
      CPPUNIT_ASSERT(factory->CreateItemDef(getter_AddRefs(itemDef)) == NS_OK);
      CPPUNIT_ASSERT(itemDef);
      const PRUnichar* id = ToNewUnicode(itemID); 
      CPPUNIT_ASSERT(itemDef->SetItemID(id) == NS_OK);
      CPPUNIT_ASSERT(itemDef->SetIsActive(PR_TRUE) == NS_OK);
      CPPUNIT_ASSERT(itemDef->SetClientHandle(1) == NS_OK);
      nsCOMPtr<nsISupports> itemDefSupports = do_QueryInterface(itemDef);
      vec.push_back(itemDefSupports);
    }

    nsCOMPtr<nsStlArray> itemDefs(new nsStlArray(vec));
    nsCOMPtr<nsIArray> resultArray;
    CPPUNIT_ASSERT(itemMgt->AddItems(itemDefs.get(), getter_AddRefs(resultArray)) == NS_OK);
    CPPUNIT_ASSERT(resultArray);
    PRUint32 resultSize = 0;
    CPPUNIT_ASSERT(resultArray->GetLength(&resultSize) == NS_OK);
    CPPUNIT_ASSERT(resultSize == vec.size());

    std::vector<PRUint32> handles;
    for (PRUint32 i = 0; i < resultSize; ++i)
    {
      nsCOMPtr<nsIOPCItemResult> itemResults;
      CPPUNIT_ASSERT(resultArray->QueryElementAt(0, NS_GET_IID(nsIOPCItemResult), getter_AddRefs(itemResults)) == NS_OK);
      CPPUNIT_ASSERT(itemResults);

      PRUint32 errorCode = ~PRUint32();
      CPPUNIT_ASSERT(itemResults->GetError(&errorCode) == NS_OK);
      CPPUNIT_ASSERT(errorCode == 0);

      PRUint32 serverHandle = 0;
      CPPUNIT_ASSERT(itemResults->GetServerHandle(&serverHandle) == NS_OK);
      CPPUNIT_ASSERT(serverHandle != 0);
      handles.push_back(serverHandle);
    }
    return handles;
  } 

  nsCOMPtr<nsIArray> GetItemServerHandles(nsISupports& group, const std::vector<nsString>& itemIDs)
  {
    nsCOMPtr<nsIOPCItemMgt> itemMgt = do_QueryInterface(&group);
    CPPUNIT_ASSERT(itemMgt);
    nsCOMPtr<nsISimpleEnumerator> itemsEnumerator;
    CPPUNIT_ASSERT(itemMgt->CreateEnumerator(getter_AddRefs(itemsEnumerator)) == NS_OK);
    const auto& handles = AddItems(group, itemIDs);
    return nsCOMPtr<nsIArray>(new nsStlArray(handles));
  }

  nsCOMPtr<nsIArray> CreateIntVariants(PRUint32 count)
  {
    std::vector<nsCOMPtr<nsISupports> > valuesArray;
    while (count--)
    {
      nsCOMPtr<nsIWritableVariant> variant = do_CreateInstance(NS_VARIANT_CONTRACTID);
      variant->SetAsInt32(10);
      nsCOMPtr<nsISupports> supports = do_QueryInterface(variant);
      valuesArray.push_back(supports);
    }
    return nsCOMPtr<nsIArray>(new nsStlArray(valuesArray));
  }

}  //namespace
void Test_nsIOPCSyncIO(nsISupports& group, const std::vector<nsString>& itemIDs)
{
  std::cout << "---------------------------------------------------" << std::endl;
  std::cout << "nsIOPCSyncIO.... ";
  nsCOMPtr<nsIOPCSyncIO> syncIO = do_QueryInterface(&group);
  CPPUNIT_ASSERT(syncIO);
  nsCOMPtr<nsIArray> serverHandles = GetItemServerHandles(group, itemIDs);
  PRUint32 handlesLength = 0;
  CPPUNIT_ASSERT(serverHandles->GetLength(&handlesLength) == NS_OK);

  // Reading from cache
  PRBool fromCache = PR_TRUE;
  nsCOMPtr<nsIArray> itemsState;
  CPPUNIT_ASSERT(syncIO->Read(fromCache, serverHandles, getter_AddRefs(itemsState)) == NS_OK);
  PRUint32 cacheLength = 0;
  CPPUNIT_ASSERT(itemsState->GetLength(&cacheLength) == NS_OK);
  CPPUNIT_ASSERT(cacheLength == handlesLength);

  // reading from device
  PRBool fromDevice(PR_FALSE);  
  nsCOMPtr<nsIArray> deviceState;
  CPPUNIT_ASSERT(syncIO->Read(fromDevice, serverHandles, getter_AddRefs(deviceState)) == NS_OK);
  PRUint32 deviceLength = 0;
  CPPUNIT_ASSERT(deviceState->GetLength(&deviceLength) == NS_OK);
  CPPUNIT_ASSERT(deviceLength = handlesLength);

  // writing data to device
  nsCOMPtr<nsIArray> valuesArray = CreateIntVariants(handlesLength);
  nsCOMPtr<nsIArray> writeErrors;
  nsresult res = syncIO->Write(serverHandles, valuesArray, getter_AddRefs(writeErrors));
  if(res == NS_OK)
  {
    PRUint32 errorsLength = 0;
    CPPUNIT_ASSERT(writeErrors->GetLength(&errorsLength) == NS_OK);
    CPPUNIT_ASSERT(errorsLength == handlesLength);
  }

  std::cout << "ok" << std::endl;
}

