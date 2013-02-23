// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include "ServerConnectionPoint.h"

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************

NS_IMPL_ISUPPORTS1(nsIEnumConnectionPoints, nsISimpleEnumerator)

nsEnumConnectionPoints ::nsEnumConnectionPoints (IUnknown* en)
{
  // initialization
  this->en=NULL;
  cnt=0;
  cur=0;
  str=NULL;
  mal=NULL;

  // quering needful interfaces
  CoGetMalloc(1,&mal);
  en->QueryInterface(IID_IEnumConnectionPoints,(void**)&this->en);
}

nsEnumConnectionPoints ::~nsEnumConnectionPoints ()
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
void nsEnumConnectionPoints ::Free()
{
  // realeasing all remaining interfaces
  for(int i=0;i<cnt;i++)
  {
    if(points[i]!=NULL)
    {
      points[i]->Release();
    }
  }
}
/* boolean hasMoreElements (); */
NS_IMETHODIMP nsEnumConnectionPoints ::HasMoreElements(PRBool *_retval)
{
  *_retval=PR_TRUE;
  if(cnt==0 && en!=NULL)
  {
    memset(points,0,sizeof(points));
    if(FAILED(en->Next(100,points,&cnt)) || cnt==0)
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
NS_IMETHODIMP nsEnumConnectionPoints ::GetNext(nsISupports** _retval)
{
  // если количество ноль - новый получаем список OPC серверов
  if(cnt==0 && en!=NULL)
  {
    memset(points,0,sizeof(points));
    if(FAILED(en->Next(100,points,&cnt) || cnt==0))
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

/*
  nsOPCGroup* nsgr=new nsOPCGroup();
  nsgr->Init(points[--cnt]);
  groups[cnt]->Release();
  groups[cnt]=NULL;

  nsresult res=nsgr->QueryInterface(NS_GET_IID(nsISupports),(void**)_retval);
  if(FAILED(res))
  {
    *_retval=NULL;
    delete nsgr;
    return res;
  }
  */
  return NS_OK;
}
