// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#if (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#include "OPCServer.h"                      
#include "OPCGroup.h"

#include "utils/convert.h"

namespace MsOpcBridge
{
  /* Implementation file */
  NS_IMPL_ISUPPORTS1(nsOPCServer, nsIOPCServer)

  nsOPCServer::nsOPCServer(CComPtr<IOPCServer> server)
    : OpcServer(server)
  {
     new nsOPCGroupStateMgt(CComPtr<IOPCGroupStateMgt>());
    /* member initializers and constructor code */
  }

  nsOPCServer::~nsOPCServer()
  {
    /* destructor code */
  }

  /* nsISupports addGroup (in wstring szName, in boolean active, in PRUint32 requestedUpdateRate, in PRUint32 timeBias, in float percentDeadband); */
  NS_IMETHODIMP nsOPCServer::AddGroup(const PRUnichar *szName, PRBool active, PRUint32 requestedUpdateRate, PRUint32 timeBias, float percentDeadband, nsISupports **_retval NS_OUTPARAM)
  {
    return NS_OK;
  }

  /* wstring getLastError (); */
  NS_IMETHODIMP nsOPCServer::GetLastError(PRUnichar **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* wstring getErrorString (in unsigned long error); */
  NS_IMETHODIMP nsOPCServer::GetErrorString(PRUint32 error, PRUnichar **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* nsISupports getGroupByName (in wstring name); */
  NS_IMETHODIMP nsOPCServer::GetGroupByName(const PRUnichar *name, nsISupports **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* nsIOPCServerStatus getStatus (); */
  NS_IMETHODIMP nsOPCServer::GetStatus(nsIOPCServerStatus **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* boolean removeGroup (in nsISupports group); */
  NS_IMETHODIMP nsOPCServer::RemoveGroup(nsISupports *group, PRBool *_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* nsISupports createGroupEnumerator (in boolean bAll, in nsIDPtr riid); */
  NS_IMETHODIMP nsOPCServer::CreateGroupEnumerator(PRBool bAll, const nsID * riid, nsISupports **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }
}

#endif // (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)
