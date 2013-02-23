// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __OPBROWSE_INTERFACE_H
#define __OPBROWSE_INTERFACE_H

#include <nsISimpleEnumerator.h>


#include <list>

#include "mozopc.h"
#include "opcda.h"

using namespace std;


class nsOPCBrowse : public nsISimpleEnumerator
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISIMPLEENUMERATOR

  nsOPCBrowse(IOPCBrowse* br,const PRUnichar *szItemID, PRUint16 dwBrowseFilter, const PRUnichar *szElementNameFilter, const PRUnichar *szVendorFilter);

private:
  ~nsOPCBrowse();
  list<OPCBROWSEELEMENT> lst_browse;

protected:
  /* additional members */
};



#endif // __OPBROWSE_INTERFACE_H