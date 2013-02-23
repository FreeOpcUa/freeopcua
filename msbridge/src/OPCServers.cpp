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
#include "opcda.h"

#include "OPCServers.h"

const nsCID nsOPCServers::cid={ 0x39460237, 0xd584, 0x4f06, { 0x89, 0xca, 0x36, 0x70, 0x59, 0xaf, 0x7d, 0x97} };
const char* nsOPCServers::class_name="opc_servers";
const char* nsOPCServers::contract_id="@treww.org/opc_servers;1";


/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCServers,nsIStringEnumerator);

nsOPCServers::nsOPCServers()
{
  cnt=0;
  cur=0;
  str=NULL;
  en=NULL;
  mal=NULL;

  HRESULT res;
  ICatInformation* CatInfo=NULL;

  COSERVERINFO info={0};
  res=CoCreateInstance(CLSID_StdComponentCategoriesMgr,NULL,CLSCTX_INPROC_SERVER,IID_ICatInformation,(void**)&CatInfo);

  if(FAILED(res)) return;
  do
  {
    if(FAILED(CoGetMalloc(1,&mal))) break;

    CATID cats[2];
    cats[0]=CATID_OPCDAServer20;
    cats[1]=CATID_OPCDAServer30;

    res=CatInfo->EnumClassesOfCategories(2,cats,0,NULL,&en);
    if(FAILED(res)) break;
  }
  while(0);

  if(CatInfo!=NULL)
  {
    CatInfo->Release();
    CatInfo=NULL;
  }
}

nsOPCServers::~nsOPCServers()
{
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

void nsOPCServers::Free()
{
}

/* boolean hasMore (); */
NS_IMETHODIMP nsOPCServers::HasMore(PRBool *_retval)
{
  *_retval=PR_TRUE;
  if(cnt==0 && en!=NULL)
  {
    memset(guids,0,sizeof(guids));
    if(FAILED(en->Next(100,guids,&cnt)) || cnt==0)
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
NS_IMETHODIMP nsOPCServers::GetNext(nsAString & _retval)
{
  // если количество ноль - новый получаем список OPC серверов
  if(cnt==0 && en!=NULL)
  {
    memset(guids,0,sizeof(guids));
    if(FAILED(en->Next(100,guids,&cnt) || cnt==0))
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
  HRESULT res=ProgIDFromCLSID(guids[--cnt],&str);
  nsAutoString wstr;
  wstr.Assign(str);
  _retval.Assign(ToNewUnicode(wstr));
  return NS_OK;
}