// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $


#include "BrowseElement.h"
#include "Browse.h"

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsOPCBrowse, nsISimpleEnumerator)

nsOPCBrowse::nsOPCBrowse(IOPCBrowse* br,
                         const PRUnichar *szItemID, 
                         PRUint16 dwBrowseFilter, 
                         const PRUnichar *szElementNameFilter, 
                         const PRUnichar *szVendorFilter)

{
  LPWSTR cont_point=NULL;
  BOOL  has_more=false;
  DWORD count=0;
  DWORD ids=0;
  OPCBROWSEELEMENT* browse=NULL; 

  br->AddRef();
  do
  {
    HRESULT res=br->Browse( (LPWSTR)szItemID,
                &cont_point,
                100,
                (OPCBROWSEFILTER)dwBrowseFilter,
                (LPWSTR)szElementNameFilter,
                (LPWSTR)szVendorFilter,
                false,  // bReturnAllProperties
                false,  // bReturnPropertyValues
                0,
                &ids,   // pdwPropertyIDs
                &has_more,
                &count,
                &browse);
    for(DWORD i=0;i<count;i++)
    {
      lst_browse.push_back(browse[i]);
    }
  }
  while(has_more);

  br->Release();
  /* member initializers and constructor code */
}

nsOPCBrowse::~nsOPCBrowse()
{
/*
  IMalloc* mal=NULL;
  if(lst_browse.size() && !FAILED(CoGetMalloc(1,&mal)))
  {
    list<OPCBROWSEELEMENT*>::iterator it;
    for(it=lst_browse.begin();it!=lst_browse.end();++it)
    {
//      mal->Free((*it)->szItemID);
//      mal->Free((*it)->szName);
//      mal->Free(*it);
    }
  }
*/
}

/* boolean hasMoreElements (); */
NS_IMETHODIMP nsOPCBrowse::HasMoreElements(PRBool *_retval)
{
  *_retval=lst_browse.size()?PR_TRUE:PR_FALSE;
  return NS_OK;
}

/* nsISupports getNext (); */
NS_IMETHODIMP nsOPCBrowse::GetNext(nsISupports **_retval)
{

  if(lst_browse.size()==0)
    return NS_ERROR_FAILURE;

  nsOPCBrowseElement* el=new nsOPCBrowseElement(*lst_browse.begin());
  lst_browse.pop_front();
  el->AddRef();

  nsresult res=el->QueryInterface(nsISupports::GetIID(),(void**)_retval);
  if(NS_FAILED(res))
  {
    el->Release();
    *_retval=NULL;
    return res;
  }

  return NS_OK;
}
