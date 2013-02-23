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
#include "VariantUtils.h"
#include "OPCItemAttributes.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCItemAttributes, nsIOPCItemAttributes)

nsOPCItemAttributes::nsOPCItemAttributes(OPCITEMATTRIBUTES* attrs)
{
  if(attrs==NULL)
    return;
  if(attrs->szAccessPath!=NULL)
    szAccessPath=attrs->szAccessPath;
  if(attrs->szItemID!=NULL)
    szItemID=attrs->szItemID;

  bActive=attrs->bActive;
  hClient=attrs->hClient;
  hServer=attrs->hServer;
  dwAccessRights=attrs->dwAccessRights;
  type2str(attrs->vtRequestedDataType,vtRequestedDataType);
  type2str(attrs->vtCanonicalDataType,vtCanonicalDataType);
  vEUInfo;

  /* member initializers and constructor code */
}

nsOPCItemAttributes::~nsOPCItemAttributes()
{
  /* destructor code */
}

/* readonly attribute AString szAccessPath; */
NS_IMETHODIMP nsOPCItemAttributes::GetSzAccessPath(nsAString & aSzAccessPath)
{
  aSzAccessPath.Assign(szAccessPath.c_str());
  return NS_OK;
}

/* readonly attribute AString szItemID; */
NS_IMETHODIMP nsOPCItemAttributes::GetSzItemID(nsAString & aSzItemID)
{
  aSzItemID.Assign(szItemID.c_str());
  return NS_OK;
}

/* readonly attribute boolean bActive; */
NS_IMETHODIMP nsOPCItemAttributes::GetBActive(PRBool *aBActive)
{
  *aBActive=bActive;
  return NS_OK;
}

/* readonly attribute unsigned long hClient; */
NS_IMETHODIMP nsOPCItemAttributes::GetHClient(PRUint32 *aHClient)
{
  *aHClient=hClient;
  return NS_OK;
}

/* readonly attribute unsigned long hServer; */
NS_IMETHODIMP nsOPCItemAttributes::GetHServer(PRUint32 *aHServer)
{
  *aHServer=hServer;
  return NS_OK;
}

/* readonly attribute unsigned long dwAccessRights; */
NS_IMETHODIMP nsOPCItemAttributes::GetDwAccessRights(PRUint32 *aDwAccessRights)
{
  *aDwAccessRights=dwAccessRights;
  return NS_OK;
}

/* readonly attribute AString vtRequestedDataType; */
NS_IMETHODIMP nsOPCItemAttributes::GetVtRequestedDataType(nsAString & aVtRequestedDataType)
{
  aVtRequestedDataType.Assign(vtRequestedDataType.c_str());
  return NS_OK;
}

/* readonly attribute AString vtCanonicalDataType; */
NS_IMETHODIMP nsOPCItemAttributes::GetVtCanonicalDataType(nsAString & aVtCanonicalDataType)
{
  aVtCanonicalDataType.Assign(vtCanonicalDataType.c_str());
  return NS_OK;
}

/* readonly attribute nsIVariant vEUInfo; */
NS_IMETHODIMP nsOPCItemAttributes::GetVEUInfo(nsIVariant * *aVEUInfo)
{
  return NS_OK;
}

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************

NS_IMPL_ISUPPORTS1(nsItemAttributesEnum , nsISimpleEnumerator)

nsItemAttributesEnum::nsItemAttributesEnum(IUnknown* en)
{
  // initialization
  this->en=NULL;
  cnt=0;
  cur=0;
  str=NULL;
  mal=NULL;
  attrs=NULL;

  if(en==NULL)
    return;
  // quering needful interfaces
  CoGetMalloc(1,&mal);
  en->QueryInterface(IID_IEnumOPCItemAttributes,(void**)&this->en);
}

nsItemAttributesEnum::~nsItemAttributesEnum()
{
  Free(); 
  if(mal!=NULL)
  {
    mal->Release();
    mal=NULL;
  }
  if(en!=NULL)
  {
    en->Release();
    en=NULL;
  }
}
void nsItemAttributesEnum::Free()
{
  // realeasing all remaining interfaces
  for(int i=0;i<cnt;i++)
  {
    if(attrs[i].szItemID!=NULL)
      mal->Free(attrs[i].szItemID);
    if(attrs[i].szAccessPath!=NULL)
      mal->Free(attrs[i].szAccessPath);
    if(attrs[i].pBlob!=NULL)
      mal->Free(attrs[i].pBlob);
    VariantClear(&attrs[i].vEUInfo);
  }
  if(attrs!=NULL)
    mal->Free(attrs);
}
/* boolean hasMoreElements (); */
NS_IMETHODIMP nsItemAttributesEnum::HasMoreElements(PRBool *_retval)
{
  *_retval=PR_TRUE;
  if(cnt==0 && en!=NULL)
  {
    Free();
    if(FAILED(en->Next(100,&attrs,&cnt)) || cnt==0)
    {
      en->Release();
      en=NULL;
      mal->Release();
      mal=NULL;
    }
  }
  
  if(en==NULL || mal==NULL)
  {
    cnt=0;
  }

  *_retval=(cnt==0)?PR_FALSE:PR_TRUE;

  return NS_OK;
}


/* AString getNext (); */
NS_IMETHODIMP nsItemAttributesEnum::GetNext(nsISupports** _retval)
{
  // если количество ноль - новый получаем список OPC серверов
  if(cnt==0 && en!=NULL)
  {
    Free();
    if(FAILED(en->Next(100,&attrs,&cnt) || cnt==0))
    {
      en->Release();
      en=NULL;
      mal->Release();
      mal=NULL;
    }
    cur=0;
  }
  
  if(en==NULL || mal==NULL)
  {
    cnt=0;
  }

  if(cnt==0)
  {
    return NS_OK;
  }


  nsOPCItemAttributes* attr=new nsOPCItemAttributes(&attrs[--cnt]);
  attr->AddRef();
  if(attrs[cnt].szItemID!=NULL)
    mal->Free(attrs[cnt].szItemID);
  if(attrs[cnt].szAccessPath!=NULL)
    mal->Free(attrs[cnt].szAccessPath);
  if(attrs[cnt].pBlob!=NULL)
    mal->Free(attrs[cnt].pBlob);
  VariantClear(&attrs[cnt].vEUInfo);

  nsresult res=attr->QueryInterface(NS_GET_IID(nsISupports),(void**)_retval);
  if(FAILED(res))
  {
    *_retval=NULL;
    attr->Release();
    return res;
  }
  attr->Release();
  
  return NS_OK;
}
