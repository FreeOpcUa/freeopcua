// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __OPC_ITEM_RESULT__H__
#define __OPC_ITEM_RESULT__H__

#include "mozopc.h"
#include "opcda.h"

/* Header file */
class nsOPCItemResult : public nsIOPCItemResult
{
  PRUint32 hServer;
  PRUint32 access;
  PRUint32 type;
  PRUint32 error;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOPCITEMRESULT

  nsOPCItemResult(OPCHANDLE hServer,DWORD access,VARTYPE type,HRESULT err);

private:
  ~nsOPCItemResult();

protected:
  /* additional members */
};


#endif  //__OPC_ITEM_RESULT__H__