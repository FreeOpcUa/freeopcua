// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include "OPCServerFactory.h"
#include "OPCServer.h"

char* OPCServerFactory::loader_string="opc_server";
char* OPCServerFactory::class_name="@treww.org/opc_server:1";

// идентификатор класса
// {39460237-D584-4f06-89CA-367059AF7D97}
#define NS_OPC_SERVER_CID \
{ 0x39460237, 0xd584, 0x4f06, { 0x89, 0xca, 0x36, 0x70, 0x59, 0xaf, 0x7d, 0x97} };
const nsIID OPCServerFactory::kCID = NS_OPC_SERVER_CID;


/* Implementation file */
NS_IMPL_ISUPPORTS1(OPCServerFactory , nsIFactory)

OPCServerFactory ::OPCServerFactory ()
{
  /* member initializers and constructor code */
}

OPCServerFactory ::~OPCServerFactory ()
{
  /* destructor code */
}

/* void createInstance (in nsISupports aOuter, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP OPCServerFactory ::CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result)
{
  nsOPCServer* srv=new nsOPCServer();

  nsresult rv = srv->QueryInterface(nsIOPCServer::GetIID(), (void**)result);
	if (NS_FAILED(rv))
  {
    srv->Release();   
    *result=NULL;
    return rv;
  }

  return NS_OK;
}

/* void lockFactory (in PRBool lock); */
NS_IMETHODIMP OPCServerFactory ::LockFactory(PRBool lock)
{
  return NS_OK;
}

/* End of implementation class template. */
