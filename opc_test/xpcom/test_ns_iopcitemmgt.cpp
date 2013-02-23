/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Tests of nsIOPCItemMgt interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include "test_ns_iopcitemmgt.h"

#include "array.h"

#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <nsISupportsPrimitives.h>
#include <nsStringAPI.h>
#include <xpcom/mozopc.h>
#include <xpcom/src/ns_array.h>


namespace
{
  void Check_ValidateItem(nsIOPCGroupInterfacesFactory& factory, const nsString& itemID, bool isValidItemID, nsIOPCItemMgt& itemMgt)
  {
    nsCOMPtr<nsIOPCItemDef> itemDef;
    CPPUNIT_ASSERT(factory.CreateItemDef(getter_AddRefs(itemDef)) == NS_OK);
    CPPUNIT_ASSERT(itemDef);
    PRUnichar*  id = ToNewUnicode(itemID);
    CPPUNIT_ASSERT(itemDef->SetItemID(id) == NS_OK);
    nsCOMPtr<nsISupports> itemDefSupports = do_QueryInterface(itemDef);
    const std::vector<nsCOMPtr<nsISupports> > vec(1, itemDefSupports);
    nsCOMPtr<nsStlArray> itemDefs(new nsStlArray(vec));
    nsCOMPtr<nsIArray> resultArray;
    CPPUNIT_ASSERT(itemMgt.ValidateItems(itemDefs.get(), getter_AddRefs(resultArray)) == NS_OK);
    CPPUNIT_ASSERT(resultArray);
    PRUint32 resultSize = 0;
    CPPUNIT_ASSERT(resultArray->GetLength(&resultSize) == NS_OK);
    CPPUNIT_ASSERT(resultSize == 1);
    nsCOMPtr<nsIOPCItemResult> itemResults;
    CPPUNIT_ASSERT(resultArray->QueryElementAt(0, NS_GET_IID(nsIOPCItemResult), getter_AddRefs(itemResults)) == NS_OK);
    CPPUNIT_ASSERT(itemResults);

    PRUint32 errorCode = ~PRUint32();
    CPPUNIT_ASSERT(itemResults->GetError(&errorCode) == NS_OK);
 
    if (isValidItemID)
    {
      CPPUNIT_ASSERT(errorCode == 0);
    }
    else
    {
      CPPUNIT_ASSERT(errorCode != 0);
    }
  }

  PRUint32 Check_AddItem(nsIOPCGroupInterfacesFactory& factory, const nsString& itemID, bool isValidItemID, nsIOPCItemMgt& itemMgt)
  {
    nsCOMPtr<nsIOPCItemDef> itemDef;
    CPPUNIT_ASSERT(factory.CreateItemDef(getter_AddRefs(itemDef)) == NS_OK);
    CPPUNIT_ASSERT(itemDef);
//    const PRUnichar accessPath[] = 'A', 'c', 'c', 'e', 's', 's', 'P', 'a', 't', 'h', 0};
//    CPPUNIT_ASSERT(itemDef->SetAccessPath(accessPath) == NS_OK);
    const PRUnichar* id = ToNewUnicode(itemID); 
    CPPUNIT_ASSERT(itemDef->SetItemID(id) == NS_OK);
    CPPUNIT_ASSERT(itemDef->SetIsActive(PR_TRUE) == NS_OK);
    CPPUNIT_ASSERT(itemDef->SetClientHandle(1) == NS_OK);
    nsCOMPtr<nsISupports> itemDefSupports = do_QueryInterface(itemDef);
    const std::vector<nsCOMPtr<nsISupports> > vec(1, itemDefSupports);
    nsCOMPtr<nsStlArray> itemDefs(new nsStlArray(vec));
    nsCOMPtr<nsIArray> resultArray;
    CPPUNIT_ASSERT(itemMgt.AddItems(itemDefs.get(), getter_AddRefs(resultArray)) == NS_OK);
    CPPUNIT_ASSERT(resultArray);
    PRUint32 resultSize = 0;
    CPPUNIT_ASSERT(resultArray->GetLength(&resultSize) == NS_OK);
    CPPUNIT_ASSERT(resultSize == 1);
    nsCOMPtr<nsIOPCItemResult> itemResults;
    CPPUNIT_ASSERT(resultArray->QueryElementAt(0, NS_GET_IID(nsIOPCItemResult), getter_AddRefs(itemResults)) == NS_OK);
    CPPUNIT_ASSERT(itemResults);

    PRUint32 errorCode = ~PRUint32();
    CPPUNIT_ASSERT(itemResults->GetError(&errorCode) == NS_OK);
    if (isValidItemID)
    {
      CPPUNIT_ASSERT(errorCode == 0);
      PRUint16 dataType = 0;
      CPPUNIT_ASSERT(itemResults->GetDataType(&dataType) == NS_OK);

      PRUint32 serverHandle = 0;
      CPPUNIT_ASSERT(itemResults->GetServerHandle(&serverHandle) == NS_OK);
      CPPUNIT_ASSERT(serverHandle != 0);
      return serverHandle;
    }
    else
    {
      CPPUNIT_ASSERT(errorCode);
    }
    return ~PRUint32();
  }

  void Check_SetActiveState(nsIOPCItemMgt& itemMgt, PRUint32 itemHandle, PRBool isActive)
  {
    std::vector<PRUint32> handles;
    handles.push_back(itemHandle);
    handles.push_back(-1);
    nsCOMPtr<nsIArray> handlesArray(new nsStlArray(handles));
    nsCOMPtr<nsIArray> result;
    nsresult res = itemMgt.SetActiveState(handlesArray.get(), isActive, getter_AddRefs(result));
    PRUint32 length = 0;
    CPPUNIT_ASSERT(result->GetLength(&length) == NS_OK);
    CPPUNIT_ASSERT(length == 2);

    nsCOMPtr<nsISupportsPRUint32> error1; 
    CPPUNIT_ASSERT(result->QueryElementAt(0, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error1)) == NS_OK); 
    PRUint32 error1Data = -1;
    CPPUNIT_ASSERT(error1->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error1Data == 0);

    nsCOMPtr<nsISupportsPRUint32> error2; 
    CPPUNIT_ASSERT(result->QueryElementAt(1, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error2)) == NS_OK); 
    PRUint32 error2Data = -1;
    CPPUNIT_ASSERT(error2->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error2Data != 0);

    CPPUNIT_ASSERT(res == NS_OK);
  }
 
  void Check_SetClientHandles(nsIOPCItemMgt& itemMgt, PRUint32 itemHandle)
  {
    std::vector<PRUint32> handles;
    handles.push_back(itemHandle);
    handles.push_back(-1);
    nsCOMPtr<nsIArray> handlesArray(new nsStlArray(handles));
    nsCOMPtr<nsIArray> result;

    std::vector<PRUint32> clientHandles;
    clientHandles.push_back(15);
    clientHandles.push_back(14);
    nsCOMPtr<nsIArray> clientHandlesArray(new nsStlArray(clientHandles));

    nsresult res = itemMgt.SetClientHandles(handlesArray.get(), clientHandlesArray, getter_AddRefs(result));
    PRUint32 length = 0;
    CPPUNIT_ASSERT(result->GetLength(&length) == NS_OK);
    CPPUNIT_ASSERT(length == 2);

    nsCOMPtr<nsISupportsPRUint32> error1; 
    CPPUNIT_ASSERT(result->QueryElementAt(0, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error1)) == NS_OK); 
    PRUint32 error1Data = -1;
    CPPUNIT_ASSERT(error1->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error1Data == 0);

    nsCOMPtr<nsISupportsPRUint32> error2; 
    CPPUNIT_ASSERT(result->QueryElementAt(1, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error2)) == NS_OK); 
    PRUint32 error2Data = -1;
    CPPUNIT_ASSERT(error2->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error2Data != 0);

    CPPUNIT_ASSERT(res == NS_OK);
  }

  void Check_SetDataTypes(nsIOPCItemMgt& itemMgt, PRUint32 itemHandle)
  {
    std::vector<PRUint32> handles;
    handles.push_back(itemHandle);
    handles.push_back(-1);
    nsCOMPtr<nsIArray> handlesArray(new nsStlArray(handles));
    nsCOMPtr<nsIArray> result;

    std::vector<PRUint32> dataTypes;
    dataTypes.push_back(nsIDataType::VTYPE_INT32);
    dataTypes.push_back(14);
    nsCOMPtr<nsIArray> clientHandlesArray(new nsStlArray(dataTypes));

    nsresult res = itemMgt.SetDataTypes(handlesArray.get(), clientHandlesArray, getter_AddRefs(result));
    PRUint32 length = 0;
    CPPUNIT_ASSERT(result->GetLength(&length) == NS_OK);
    CPPUNIT_ASSERT(length == 2);

    nsCOMPtr<nsISupportsPRUint32> error1; 
    CPPUNIT_ASSERT(result->QueryElementAt(0, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error1)) == NS_OK); 
    PRUint32 error1Data = -1;
    CPPUNIT_ASSERT(error1->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error1Data == 0);

    nsCOMPtr<nsISupportsPRUint32> error2; 
    CPPUNIT_ASSERT(result->QueryElementAt(1, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error2)) == NS_OK); 
    PRUint32 error2Data = -1;
    CPPUNIT_ASSERT(error2->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error2Data != 0);

    CPPUNIT_ASSERT(res == NS_OK);
  }

  void Check_RemoveItems(nsIOPCItemMgt& itemMgt, PRUint32 itemHandle)
  {
    std::vector<PRUint32> handles;
    handles.push_back(itemHandle);
    handles.push_back(-1);
    nsCOMPtr<nsIArray> handlesArray(new nsStlArray(handles));
    nsCOMPtr<nsIArray> result;

    nsresult res = itemMgt.RemoveItems(handlesArray.get(), getter_AddRefs(result));
    PRUint32 length = 0;
    CPPUNIT_ASSERT(result->GetLength(&length) == NS_OK);
    CPPUNIT_ASSERT(length == 2);

    nsCOMPtr<nsISupportsPRUint32> error1; 
    CPPUNIT_ASSERT(result->QueryElementAt(0, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error1)) == NS_OK); 
    PRUint32 error1Data = -1;
    CPPUNIT_ASSERT(error1->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error1Data == 0);

    nsCOMPtr<nsISupportsPRUint32> error2; 
    CPPUNIT_ASSERT(result->QueryElementAt(1, NS_GET_IID(nsISupportsPRUint32), getter_AddRefs(error2)) == NS_OK); 
    PRUint32 error2Data = -1;
    CPPUNIT_ASSERT(error2->GetData(&error1Data) == NS_OK);
    CPPUNIT_ASSERT(error2Data != 0);

    CPPUNIT_ASSERT(res == NS_OK);
  }

  inline bool HasMoreElements(nsISimpleEnumerator& en)
  {
    PRBool result = PR_FALSE;
    return en.HasMoreElements(&result) == NS_OK && result == PR_TRUE;
  }

  inline nsCOMPtr<nsIOPCItemAttributes> GetElement(nsISimpleEnumerator& en)
  {
    nsCOMPtr<nsISupports> element;
    CPPUNIT_ASSERT(en.GetNext(getter_AddRefs(element)) == NS_OK);
    nsCOMPtr<nsIOPCItemAttributes> result = do_QueryInterface(element);
    CPPUNIT_ASSERT(result.get());
    return result;
  }


  void Check_ItemsEnumerator(nsIOPCItemMgt& itemMgt)
  {
    nsCOMPtr<nsISimpleEnumerator> itemsEnum;
    CPPUNIT_ASSERT(itemMgt.CreateEnumerator(getter_AddRefs(itemsEnum)) == NS_OK);
    CPPUNIT_ASSERT(itemsEnum);
    while(HasMoreElements(*itemsEnum))
    {
      nsCOMPtr<nsIOPCItemAttributes> attributes = GetElement(*itemsEnum);
      PRBool isActive = PR_TRUE;
      CPPUNIT_ASSERT(attributes->GetIsActive(&isActive) == NS_OK);
      CPPUNIT_ASSERT(isActive == PR_FALSE);

      nsString itemID;
      CPPUNIT_ASSERT(attributes->GetItemID(itemID) == NS_OK);
      CPPUNIT_ASSERT(itemID.Length() != 0);

      PRUint32 serverHandle = 0;
      CPPUNIT_ASSERT(attributes->GetClientHandle(&serverHandle) == NS_OK);
      CPPUNIT_ASSERT(serverHandle != 0);

      PRUint32 clientHandle = 0;
      CPPUNIT_ASSERT(attributes->GetClientHandle(&clientHandle) == NS_OK);
      CPPUNIT_ASSERT(clientHandle == 15);

      PRUint32 dataType = 0;
      CPPUNIT_ASSERT(attributes->GetRequestedDataType(&dataType) == NS_OK);
      CPPUNIT_ASSERT(dataType == nsIDataType::VTYPE_INT32);

      PRUint32 accessRights = 0;
      CPPUNIT_ASSERT(attributes->GetAccessRights(&accessRights) == NS_OK);
      CPPUNIT_ASSERT(accessRights <= 3);

    }    
  }
}

PRUint32 Test_nsIOPCItemMgt(nsISupports& supports, const std::vector<nsString>& itemIDs)
{
  
  std::cout << "----------------------------------------------" << std::endl;
  std::cout << "nsIOPCItemMgt tests...";

  nsresult res = NS_OK;
  nsCOMPtr<nsIOPCItemMgt> itemMgt = do_QueryInterface(&supports, &res);
  CPPUNIT_ASSERT(res == NS_OK);
  CPPUNIT_ASSERT(itemMgt);

  nsCOMPtr<nsIOPCGroupInterfacesFactory> factory = do_QueryInterface(&supports, &res);
  CPPUNIT_ASSERT(res == NS_OK);
  CPPUNIT_ASSERT(factory);

  if (itemIDs.empty())
  {
    std::cout << "WARNING: No items found at server. Items management not checked." << std::endl;
    return 0;
  }

  const nsString& invalidItemID = NS_LITERAL_STRING("ItemID"); 
  Check_ValidateItem(*factory, invalidItemID, false, *itemMgt);
  Check_ValidateItem(*factory, itemIDs[0], true, *itemMgt);
  PRUint32 itemHandle = Check_AddItem(*factory, itemIDs[0], true, *itemMgt);
  Check_AddItem(*factory, invalidItemID, false, *itemMgt);
  Check_SetActiveState(*itemMgt, itemHandle, false);
  Check_SetClientHandles(*itemMgt, itemHandle);
  Check_SetDataTypes(*itemMgt, itemHandle);
  Check_ItemsEnumerator(*itemMgt);

  std::cout << "ok." << std::endl;
  return itemHandle;
}

void Test_nsIOPCItemMgt_RemoveItems(nsISupports& supports, PRUint32 itemHandle)
{
  nsCOMPtr<nsIOPCItemMgt> itemMgt = do_QueryInterface(&supports);
  CPPUNIT_ASSERT(itemMgt);
  Check_RemoveItems(*itemMgt, itemHandle);
}

