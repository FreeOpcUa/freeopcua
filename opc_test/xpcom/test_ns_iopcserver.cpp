#include "test_ns_iopcserver.h"

#include "test_ns_iopcbrowse.h"
#include "test_ns_iopcgroupinterfacesfactory.h"
#include "test_ns_iopcitemmgt.h"
#include "test_ns_iopcserverstatus.h"
#include "test_ns_iopcsyncio.h"
#include <cppunit/Message.h>
#include <cppunit/Exception.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>
#include <nsStringAPI.h>


#include <xpcom/mozopc.h>


namespace
{
  nsCOMPtr<nsISupports> AddGroup1(nsIOPCServer& server, const PRUnichar* groupName)
  {
    nsCOMPtr<nsISupports> group;
    CPPUNIT_ASSERT(server.AddGroup(groupName, PR_TRUE, 100, 0, 3, getter_AddRefs(group)) == NS_OK);
    CPPUNIT_ASSERT(group);
    
    nsCOMPtr<nsISupports> group1;
    CPPUNIT_ASSERT(server.AddGroup(groupName, PR_TRUE, 100, 0, 3, getter_AddRefs(group1)) != NS_OK);
    CPPUNIT_ASSERT(!group1);
    return group;
  }

  void CheckName(nsIOPCGroupStateMgt& groupStateMgt, const PRUnichar* name)
  {
    CPPUNIT_ASSERT(groupStateMgt.SetName(name) == NS_OK);
    PRUnichar* name1 = NULL;
    CPPUNIT_ASSERT(groupStateMgt.GetName(&name1) == NS_OK);
    nsLiteralString str(name);
    CPPUNIT_ASSERT(str == name1);
    NS_Free(name1);
    CPPUNIT_ASSERT(groupStateMgt.GetName(NULL) == NS_ERROR_INVALID_POINTER);
 }

  void CheckUpdateRate(nsIOPCGroupStateMgt& groupStateMgt)
  {
    PRUint32 tmp = 100;
    PRUint32 result = 0;
    CPPUNIT_ASSERT(groupStateMgt.SetUpdateRate(tmp) == NS_OK);
    CPPUNIT_ASSERT(groupStateMgt.GetUpdateRate(&result) == NS_OK);
    CPPUNIT_ASSERT(result == 100);
    CPPUNIT_ASSERT(groupStateMgt.GetUpdateRate(NULL) == NS_ERROR_INVALID_POINTER);
  }

  void CheckTimeBias(nsIOPCGroupStateMgt& groupStateMgt)
  {
    PRInt32 tmp = 100;
    PRInt32 result = 0;
    CPPUNIT_ASSERT(groupStateMgt.SetTimebias(tmp) == NS_OK);
    CPPUNIT_ASSERT(groupStateMgt.GetTimebias(&result) == NS_OK);
    CPPUNIT_ASSERT(result == 100);
    CPPUNIT_ASSERT(groupStateMgt.GetTimebias(NULL) == NS_ERROR_INVALID_POINTER);
  }

  void CheckDeadband(nsIOPCGroupStateMgt& groupStateMgt)
  {
    float tmp = 100;
    float result = 0;
    CPPUNIT_ASSERT(groupStateMgt.SetDeadband(tmp) == NS_OK);
    CPPUNIT_ASSERT(groupStateMgt.GetDeadband(&result) == NS_OK);
    CPPUNIT_ASSERT(result == 100);
    CPPUNIT_ASSERT(groupStateMgt.GetDeadband(NULL) == NS_ERROR_INVALID_POINTER);
  }

  void CheckActive(nsIOPCGroupStateMgt& groupStateMgt)
  {
    PRBool tmp = true;
    PRBool result = false;
    CPPUNIT_ASSERT(groupStateMgt.SetActive(tmp) == NS_OK);
    CPPUNIT_ASSERT(groupStateMgt.GetActive(&result) == NS_OK);
    CPPUNIT_ASSERT(result == true);
    CPPUNIT_ASSERT(groupStateMgt.GetActive(NULL) == NS_ERROR_INVALID_POINTER);
  }

  void CheckSetState(nsIOPCGroupStateMgt& groupStateMgt)
  {
    PRUint32 rate = 1;
    PRBool active = false;
    PRInt32 timeBias = 2;
    float deadband = 4;
    PRUint32 locale = 5;
    PRUint32 resultRate = 0;
    CPPUNIT_ASSERT(groupStateMgt.SetState(rate, active, timeBias, deadband, locale, &resultRate) == NS_OK);
    CPPUNIT_ASSERT(resultRate = rate);
    CPPUNIT_ASSERT(groupStateMgt.SetState(rate, active, timeBias, deadband, locale, NULL) == NS_ERROR_INVALID_POINTER);
 
    PRUint32 rate1 = 0;
    PRBool active1 = true;
    PRInt32 timeBias1 = 0;
    float deadband1 = 0;
    CPPUNIT_ASSERT(groupStateMgt.GetUpdateRate(&rate1) == NS_OK);
    CPPUNIT_ASSERT(rate = rate1);
   
    CPPUNIT_ASSERT(groupStateMgt.GetActive(&active1) == NS_OK);
    CPPUNIT_ASSERT(active = active1);
 
    CPPUNIT_ASSERT(groupStateMgt.GetTimebias(&timeBias1) == NS_OK);
    CPPUNIT_ASSERT(timeBias = timeBias1);
    
    CPPUNIT_ASSERT(groupStateMgt.GetDeadband(&deadband1) == NS_OK);
    CPPUNIT_ASSERT(deadband = deadband1);
  }

  void CheckGetGroupByName(nsIOPCServer& server)
  {
    const PRUnichar name[] = {'g', '2', 0};
    nsCOMPtr<nsISupports> supports;
    CPPUNIT_ASSERT(server.GetGroupByName(name, getter_AddRefs(supports)) == NS_OK);
    nsCOMPtr<nsIOPCGroupStateMgt> groupState = do_QueryInterface(supports);
    CPPUNIT_ASSERT(groupState);
    PRUnichar* name1 = NULL;
    CPPUNIT_ASSERT(groupState->GetName(&name1) == NS_OK);
    CPPUNIT_ASSERT(name1);
    nsLiteralString str(name1);
    CPPUNIT_ASSERT(str == name);
    NS_Free(name1);
  }

  void CheckGroupEnumerator(nsIOPCServer& server, unsigned elementsCount)
  {
    nsCOMPtr<nsISupports> supports;
    const nsID& simpleEnumeratorIID = NS_GET_IID(nsISimpleEnumerator);
    CPPUNIT_ASSERT(server.CreateGroupEnumerator(PR_TRUE, &simpleEnumeratorIID, getter_AddRefs(supports)) == NS_OK);
    CPPUNIT_ASSERT(supports);
    nsCOMPtr<nsISimpleEnumerator> en = do_QueryInterface(supports);
    CPPUNIT_ASSERT(en);
    PRBool hasElements = PR_FALSE;

    while (en->HasMoreElements(&hasElements) == NS_OK && hasElements == PR_TRUE)
    {
      --elementsCount;
      CPPUNIT_ASSERT(en->GetNext(getter_AddRefs(supports)) == NS_OK);
      hasElements = PR_FALSE;
    }
    CPPUNIT_ASSERT(elementsCount == 0);
    CPPUNIT_ASSERT(en->GetNext(getter_AddRefs(supports)) == NS_ERROR_FAILURE);
  }

  void CheckRemoveGroup(nsIOPCServer& server, nsISupports* group)
  {
    PRBool result = PR_FALSE;
    CPPUNIT_ASSERT(server.RemoveGroup(group, &result) == NS_OK);
    CheckGroupEnumerator(server, 1);
    CPPUNIT_ASSERT(server.RemoveGroup(NULL, &result) == NS_ERROR_INVALID_POINTER);
  }

  void Test_Groups(nsIOPCServer& server, const std::vector<nsString>& itemIDs)
  {
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "nsIOPCGroupStateMgt tests....";
    PRUnichar groupName[] = {'g', '1', 0};
    nsCOMPtr<nsISupports> supports = AddGroup1(server, groupName);
    CPPUNIT_ASSERT(supports);
    nsCOMPtr<nsIOPCGroupStateMgt> groupStateMgt = do_QueryInterface(supports);
    CPPUNIT_ASSERT(groupStateMgt);
    CheckUpdateRate(*groupStateMgt);
    PRUnichar name1[] = {'g', '2', 0};
    CheckName(*groupStateMgt, name1);
    CheckTimeBias(*groupStateMgt); 
    CheckDeadband(*groupStateMgt); 
    CheckActive(*groupStateMgt);
    std:: cout << "ok." << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "nsIOPCServer tests.." << std::endl;
    CheckGetGroupByName(server); 
    nsCOMPtr<nsISupports> supports1 = AddGroup1(server, groupName);
    CPPUNIT_ASSERT(supports1);
    CheckGroupEnumerator(server, 2);
    CheckRemoveGroup(server, supports1);
    std::cout << "ok." << std::endl;
    
    // testing adding items and it management
    PRUint32 itemHandle = Test_nsIOPCItemMgt(*supports, itemIDs);
    if (itemHandle != 0)
    {
      // testing IO with items;
      Test_nsIOPCSyncIO(*supports, itemIDs);
      // testing removing items
      Test_nsIOPCItemMgt_RemoveItems(*supports, itemHandle);
    }
    Test_nsIOPCGroupInterfacesFactory(*supports);
    Test_nsIOPCSyncIO(*supports, itemIDs);
  }
}


void Test_nsIOPCServer(nsIOPCServer& server)
{
  Test_nsIOPCServerStatus(server);
  Test_nsIOPCBrowse(server);
  Test_Groups(server, GetServerItemIDs(server));
  std::cout << "------------------------------------------" << std::endl;
}

