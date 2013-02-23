// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <string>
#include <nsStringAPI.h>
#include "OPCItemResult.h"
#include "VariantUtils.h"
/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCItemResult, nsIOPCItemResult)

nsOPCItemResult::nsOPCItemResult(OPCHANDLE _hServer,DWORD _access,VARTYPE _type,HRESULT _err)
{
  hServer=_hServer;
  access=_access;
  type=_type;
  error=_err;
}

nsOPCItemResult::~nsOPCItemResult()
{
  /* destructor code */
}

/* readonly attribute unsigned long hServer; */
NS_IMETHODIMP nsOPCItemResult::GetHServer(PRUint32 *aHServer)
{
  *aHServer=hServer;
  return NS_OK;
}

/* readonly attribute unsigned long dwAccessRights; */
NS_IMETHODIMP nsOPCItemResult::GetDwAccessRights(PRUint32 *aDwAccessRights)
{
  *aDwAccessRights=access;
  return NS_OK;
}

/* readonly attribute AString data_type; */
NS_IMETHODIMP nsOPCItemResult::GetData_type(nsAString & aData_type)
{
  
  std::wstring str;

  type2str(type,str);
  aData_type.Assign(str.c_str());
  return NS_OK;
}

/* readonly attribute unsigned long error; */
NS_IMETHODIMP nsOPCItemResult::GetError(PRUint32 *aError)
{
  *aError=error;
  return NS_OK;
}

/* End of implementation class template. */
