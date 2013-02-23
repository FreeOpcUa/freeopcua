#include "test_ns_iopcserverstatus.h"
#include <cppunit/Message.h>
#include <cppunit/Exception.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Message.h>

#include <xpcom/mozopc.h>


void Test_nsIOPCServerStatus(nsIOPCServer& server)
{
  std::cout << "---------------------------------------" << std::endl;
  std::cout << "nsIOPCServerStatus tests....";
  nsCOMPtr<nsIOPCServerStatus> serverStatus;
  CPPUNIT_ASSERT(server.GetStatus(getter_AddRefs(serverStatus)) == NS_OK);
  CPPUNIT_ASSERT(serverStatus.get() != NULL);

  PRTime tmpTime = 0;
  CPPUNIT_ASSERT(serverStatus->GetFtStartTime(&tmpTime) == NS_OK);
  CPPUNIT_ASSERT(serverStatus->GetFtCurrentTime(&tmpTime) == NS_OK);
  CPPUNIT_ASSERT(serverStatus->GetFtLastUpdateTime(&tmpTime) == NS_OK);

  PRUint32 tmpInt = 0;
  CPPUNIT_ASSERT(serverStatus->GetDwServerState(&tmpInt) == NS_OK);
  CPPUNIT_ASSERT(serverStatus->GetDwGroupCount(&tmpInt) == NS_OK);
  CPPUNIT_ASSERT(serverStatus->GetDwBandWidth(&tmpInt) == NS_OK);

  CPPUNIT_ASSERT(serverStatus->GetWMajorVersion(&tmpInt) == NS_OK);
  CPPUNIT_ASSERT(serverStatus->GetWMinorVersion(&tmpInt) == NS_OK);
  CPPUNIT_ASSERT(serverStatus->GetWBuildNumber(&tmpInt) == NS_OK);
  PRUnichar* tmpString = NULL;
  CPPUNIT_ASSERT(serverStatus->GetSzVendorInfo(&tmpString) == NS_OK);
  CPPUNIT_ASSERT(tmpString != NULL);
  NS_Free(tmpString);
  std::cout << "ok" << std::endl;
}
