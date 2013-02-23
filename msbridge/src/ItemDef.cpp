// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include "ItemDef.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCItemDef, nsIOPCItemDef)

nsOPCItemDef::nsOPCItemDef()
{
  /* member initializers and constructor code */
}

nsOPCItemDef::~nsOPCItemDef()
{
  /* destructor code */
}

/* attribute wstring szAccessPath; */
NS_IMETHODIMP nsOPCItemDef::GetSzAccessPath(PRUnichar * *aSzAccessPath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsOPCItemDef::SetSzAccessPath(const PRUnichar * aSzAccessPath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring szItemID; */
NS_IMETHODIMP nsOPCItemDef::GetSzItemID(PRUnichar * *aSzItemID)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsOPCItemDef::SetSzItemID(const PRUnichar * aSzItemID)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean bActive; */
NS_IMETHODIMP nsOPCItemDef::GetBActive(PRBool *aBActive)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsOPCItemDef::SetBActive(PRBool aBActive)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long hClient; */
NS_IMETHODIMP nsOPCItemDef::GetHClient(PRUint32 *aHClient)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsOPCItemDef::SetHClient(PRUint32 aHClient)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
