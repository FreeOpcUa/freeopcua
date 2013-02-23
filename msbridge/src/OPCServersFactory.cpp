// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include "OPCServersFactory.h"
#include "OPCServers.h"

// идентификатор класса перечислителя ОРС серверов
// {E9203330-9C96-4c94-9C2B-7BA0C717CA09}
#define NS_OPC_CID \
{ 0xe9203330, 0x9c96, 0x4c94, { 0x9c, 0x2b, 0x7b, 0xa0, 0xc7, 0x17, 0xca, 0x9 } };
static const nsCID kOpcCID = NS_OPC_CID;

char*        OPCServersFactory::loader_string="moz_opc";
char*        OPCServersFactory::class_name=   "@treww.org/opc;1";
const nsIID  OPCServersFactory::kCID=         NS_OPC_CID;   


OPCServersFactory::OPCServersFactory()
{
	mRefCnt = 0;
}
OPCServersFactory::~OPCServersFactory()
{
}
NS_IMETHODIMP OPCServersFactory::QueryInterface(const nsIID &aIID,void **aResult)
{
	if (aResult == NULL) 
	{
		return NS_ERROR_NULL_POINTER;
	}
	*aResult = NULL;
	
  if(aIID.Equals(nsISupports::GetIID())) 
	{
		*aResult = (void *) (nsISupports*) this;
	}
  else if(aIID.Equals(nsIFactory::GetIID())) 
	{
		*aResult = (void *) (nsIFactory*) this;
	}

	if (*aResult == NULL) 
	{
		return NS_ERROR_NO_INTERFACE;
	}
	AddRef();
	return NS_OK;
}
NS_IMETHODIMP_(nsrefcnt) OPCServersFactory::AddRef()
{
	return ++mRefCnt;
}
NS_IMETHODIMP_(nsrefcnt) OPCServersFactory::Release()
{
	if (--mRefCnt == 0) 
	{
		delete this;
		return 0;
	}
	return mRefCnt;
}
NS_IMETHODIMP
OPCServersFactory::CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result)
{

	if (result==NULL)
		return NS_ERROR_NULL_POINTER;
	
  // создаем экземпляр эксперта
  nsOPCServers* servers = new nsOPCServers();
	nsresult rv = servers->QueryInterface(iid, result);
	if (NS_FAILED(rv)) 
	{
		*result = nsnull;
    servers->Release();
	}
	return rv;
}
NS_IMETHODIMP
OPCServersFactory::LockFactory(PRBool lock)
{
	return NS_ERROR_NOT_IMPLEMENTED;
}
