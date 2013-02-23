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
#include "GroupEnumerators.h"
#include "OpcGroup.h"

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************

NS_IMPL_ISUPPORTS1(nsGroupStringEnum, nsIStringEnumerator)

nsGroupStringEnum::nsGroupStringEnum(IUnknown* en)
{
  this->en=NULL;
  cnt=0;
  cur=0;
  str=NULL;
  mal=NULL;

  en->QueryInterface(IID_IEnumString,(void**)&this->en);
  CoGetMalloc(1,&mal);
}

nsGroupStringEnum::~nsGroupStringEnum()
{
  if(mal!=NULL)
  {
    Free();
    mal->Release();
    mal=NULL;
  }
  if(en!=NULL)
  {
    en->Release();
    en=NULL;
  }
}
void nsGroupStringEnum::Free()
{
  for(int i=0;i<cnt;i++)
  {
    if(groups[i]!=NULL)
    {
      mal->Free(groups[i]);
    }
  }
}
/* boolean hasMore (); */
NS_IMETHODIMP nsGroupStringEnum::HasMore(PRBool *_retval)
{
  *_retval=PR_TRUE;
  if(cnt==0 && en!=NULL)
  {
    memset(groups,0,sizeof(groups));
    if(FAILED(en->Next(100,groups,&cnt)) || cnt==0)
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
NS_IMETHODIMP nsGroupStringEnum::GetNext(nsAString & _retval)
{
  // если количество ноль - новый получаем список OPC серверов
  if(cnt==0 && en!=NULL)
  {
    memset(groups,0,sizeof(groups));
    if(FAILED(en->Next(100,groups,&cnt) || cnt==0))
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

  str=NULL;
  nsAutoString wstr;
  wstr.Assign(groups[--cnt]);
  _retval.Assign(ToNewUnicode(wstr));

  mal->Free(groups[cnt]);
  return NS_OK;
}

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************

NS_IMPL_ISUPPORTS1(nsGroupSupportsEnum , nsISimpleEnumerator)

nsGroupSupportsEnum ::nsGroupSupportsEnum (IUnknown* en)
{
  // initialization
  this->en=NULL;
  cnt=0;
  cur=0;
  str=NULL;
  mal=NULL;

  if(en==NULL)
    return;
  // quering needful interfaces
  CoGetMalloc(1,&mal);
  en->QueryInterface(IID_IEnumUnknown,(void**)&this->en);
}

nsGroupSupportsEnum::~nsGroupSupportsEnum()
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
void nsGroupSupportsEnum::Free()
{
  // realeasing all remaining interfaces
  for(int i=0;i<cnt;i++)
  {
    if(groups[i]!=NULL)
    {
      groups[i]->Release();
    }
  }
}
/* boolean hasMoreElements (); */
NS_IMETHODIMP nsGroupSupportsEnum::HasMoreElements(PRBool *_retval)
{
  *_retval=PR_TRUE;
  if(cnt==0 && en!=NULL)
  {
    memset(groups,0,sizeof(groups));
    if(FAILED(en->Next(100,groups,&cnt)) || cnt==0)
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
NS_IMETHODIMP nsGroupSupportsEnum::GetNext(nsISupports** _retval)
{
  // если количество ноль - новый получаем список OPC серверов
  if(cnt==0 && en!=NULL)
  {
    memset(groups,0,sizeof(groups));
    if(FAILED(en->Next(100,groups,&cnt) || cnt==0))
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


  nsOPCGroup* nsgr=new nsOPCGroup();
  nsgr->Init(groups[--cnt]);
  groups[cnt]->Release();
  groups[cnt]=NULL;

  nsresult res=nsgr->QueryInterface(NS_GET_IID(nsISupports),(void**)_retval);
  if(FAILED(res))
  {
    *_retval=NULL;
    delete nsgr;
    return res;
  }
  
  return NS_OK;
}

