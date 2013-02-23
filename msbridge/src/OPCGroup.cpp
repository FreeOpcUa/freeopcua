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

#include "OPCGroup.h"

namespace MsOpcBridge
{
  NS_IMPL_ISUPPORTS1(nsOPCGroupStateMgt, nsIOPCGroupStateMgt)

  nsOPCGroupStateMgt::nsOPCGroupStateMgt(CComPtr<IOPCGroupStateMgt> group)
    : Group(group)
  {
    /* member initializers and constructor code */
  }

  nsOPCGroupStateMgt::~nsOPCGroupStateMgt()
  {
    /* destructor code */
  }

  /* attribute unsigned long updateRate; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetUpdateRate(PRUint32 *aUpdateRate)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetUpdateRate(PRUint32 aUpdateRate)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* attribute boolean active; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetActive(PRBool *aActive)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetActive(PRBool aActive)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* attribute wstring name; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetName(PRUnichar * *aName)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetName(const PRUnichar * aName)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* attribute long timebias; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetTimebias(PRInt32 *aTimebias)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetTimebias(PRInt32 aTimebias)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* attribute float deadband; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetDeadband(float *aDeadband)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetDeadband(float aDeadband)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* PRUint32 setState (in unsigned long requestedUpdateRate, in boolean active, in long timeBias, in float percentDeadband, in unsigned long locale); */
  NS_IMETHODIMP nsOPCGroupStateMgt::SetState(PRUint32 requestedUpdateRate, PRBool active, PRInt32 timeBias, float percentDeadband, PRUint32 locale, PRUint32 *_retval NS_OUTPARAM)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }

  /* nsISupports CloneGroup (in wstring name); */
  NS_IMETHODIMP nsOPCGroupStateMgt::CloneGroup(const PRUnichar *name, nsISupports **_retval NS_OUTPARAM)
  {
      return NS_ERROR_NOT_IMPLEMENTED;
  }
}

#endif // (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)
