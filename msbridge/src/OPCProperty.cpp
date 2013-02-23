// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <xpcom/nsIComponentManager.h>
#include <xpcom/nsComponentManagerUtils.h>

#include "OPCProperty.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCProperty, nsIOPCItemProperty)

nsOPCProperty::nsOPCProperty(const OPCITEMPROPERTY* prop)
{
  ID=prop->dwPropertyID;
  itemId.Assign(prop->szItemID);
  description.Assign(prop->szDescription);
  if(SUCCEEDED(VariantChangeType((VARIANTARG*)&prop->vValue,(VARIANTARG*)&prop->vValue,0,VT_BSTR)))
  {
    value.Assign(prop->vValue.bstrVal);
  }
  
  /* member initializers and constructor code */
}
nsOPCProperty::nsOPCProperty(LPCWSTR _itemId,DWORD _ID,LPCWSTR _description,const VARIANT* _value)
{
  ID=_ID;
  itemId.Assign(_itemId);
  description.Assign(_description);
  if(SUCCEEDED(VariantChangeType((VARIANTARG*)_value,(VARIANTARG*)_value,0,VT_BSTR)))
  {
    value.Assign(_value->bstrVal);
  }
}

nsOPCProperty::~nsOPCProperty()
{
//  value->Release();
  /* destructor code */
}

/* readonly attribute unsigned long ID; */
NS_IMETHODIMP nsOPCProperty::GetID(PRUint32 *aID)
{
  *aID=ID;
  return NS_OK;
}

/* readonly attribute AString itemId; */
NS_IMETHODIMP nsOPCProperty::GetItemId(nsAString & aItemId)
{
  aItemId=ToNewUnicode(itemId);
  return NS_OK;
}

/* readonly attribute AString description; */
NS_IMETHODIMP nsOPCProperty::GetDescription(nsAString & aDescription)
{
  aDescription=ToNewUnicode(description);
  return NS_OK;
}

/* readonly attribute AString value; */
NS_IMETHODIMP nsOPCProperty::GetValue(nsAString & aValue)
{
  aValue=ToNewUnicode(value);
  return NS_OK;
}
