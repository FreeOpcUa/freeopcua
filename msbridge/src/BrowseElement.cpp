// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <nsStringAPI.h>
#include "BrowseElement.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCBrowseElement, nsIOPCBrowseElement)

nsOPCBrowseElement::nsOPCBrowseElement(const OPCBROWSEELEMENT& el)
{
  elem=el;
  /* member initializers and constructor code */
}

nsOPCBrowseElement::~nsOPCBrowseElement()
{
  /* destructor code */
}

/* readonly attribute wstring name; */
NS_IMETHODIMP nsOPCBrowseElement::GetName(PRUnichar * *aName)
{
  nsAutoString wstr;
  wstr.Assign((PRUnichar*)elem.szName);
  *aName=ToNewUnicode(wstr);
  return NS_OK;
}

/* readonly attribute wstring szItemId; */
NS_IMETHODIMP nsOPCBrowseElement::GetSzItemId(PRUnichar * *aSzItemId)
{
  nsAutoString wstr;
  wstr.Assign((PRUnichar*)elem.szItemID);
  *aSzItemId=ToNewUnicode(wstr);
  return NS_OK;
}

/* readonly attribute boolean hasChildren; */
NS_IMETHODIMP nsOPCBrowseElement::GetHasChildren(PRBool *aHasChildren)
{
  *aHasChildren=elem.dwFlagValue&OPC_BROWSE_HASCHILDREN;
  return NS_OK;
}

/* readonly attribute boolean isItem; */
NS_IMETHODIMP nsOPCBrowseElement::GetIsItem(PRBool *aIsItem)
{
  *aIsItem=elem.dwFlagValue&OPC_BROWSE_ISITEM;
  return NS_OK;
}