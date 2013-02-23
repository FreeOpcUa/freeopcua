/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of nsIOPCGroupInterfacesFactory interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "test_ns_iopcgroupinterfacesfactory.h"

#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <nsISupportsPrimitives.h>
#include <nsStringAPI.h>
#include <xpcom/mozopc.h>


namespace
{
  void CheckOPCItemDef(nsIOPCGroupInterfacesFactory& factory)
  {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "nsIOPCItemDef tests...";

    nsCOMPtr<nsIOPCItemDef> def;
    CPPUNIT_ASSERT(factory.CreateItemDef(getter_AddRefs(def)) == NS_OK);
    CPPUNIT_ASSERT(def);
 
    const PRUnichar accessPath[] = {'a', 0};
    CPPUNIT_ASSERT(def->SetAccessPath(accessPath) == NS_OK);
    PRUnichar* resultAccessPath;
    CPPUNIT_ASSERT(def->GetAccessPath(&resultAccessPath) == NS_OK);
    NS_Free(resultAccessPath);
 
    const PRUnichar itemID[] = {'i', 0};
    CPPUNIT_ASSERT(def->SetItemID(itemID) == NS_OK);
    PRUnichar* resultItemID = NULL;
    CPPUNIT_ASSERT(def->GetItemID(&resultItemID) == NS_OK);
    NS_Free(resultItemID);
 
    CPPUNIT_ASSERT(def->SetIsActive(PR_TRUE) == NS_OK);
    PRBool active = PR_FALSE;
    CPPUNIT_ASSERT(def->GetIsActive(&active) == NS_OK);
    CPPUNIT_ASSERT(active == PR_TRUE);
    CPPUNIT_ASSERT(def->GetIsActive(NULL) == NS_ERROR_INVALID_POINTER);
 
    CPPUNIT_ASSERT(def->SetClientHandle(10) == NS_OK);
    PRUint32 hClient = 0;
    CPPUNIT_ASSERT(def->GetClientHandle(&hClient) == NS_OK);
    CPPUNIT_ASSERT(hClient == 10);
    CPPUNIT_ASSERT(def->GetClientHandle(NULL) == NS_ERROR_INVALID_POINTER);
    std::cout << "ok." << std::endl;
  }

  void CheckOPCItemVQT(/*nsIOPCItemVQT& vqt*/)
  {
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "nsIOPCItemVQT tests...";

    std::cout << "TODO!!!" << std::endl;
  }
}


void Test_nsIOPCGroupInterfacesFactory(nsISupports& supports)
{
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "nsIOPCGroupInterfacesFactory tests...";
   
  nsresult res = NS_OK;
  nsCOMPtr<nsIOPCGroupInterfacesFactory> factory = do_QueryInterface(&supports, &res);
  CPPUNIT_ASSERT(res == NS_OK);
  CPPUNIT_ASSERT(factory);

  nsCOMPtr<nsIOPCItemDef> itemDef;
  CPPUNIT_ASSERT(factory->CreateItemDef(getter_AddRefs(itemDef)) == NS_OK);
  CPPUNIT_ASSERT(itemDef);

  // TODO Create nsIOPCItemVQT
  std::cout << "ok." << std::endl;
  CheckOPCItemDef(*factory);
  CheckOPCItemVQT(/*vqt*/);
}

