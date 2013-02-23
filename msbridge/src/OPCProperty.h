// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __NS_OPCPROPERTY__H__
#define __NS_OPCPROPERTY__H__


#include "mozopc.h"
#include "opcda.h"

#include <nsStringAPI.h>
#include <windows.h>

/* Header file */
class nsOPCProperty : public nsIOPCItemProperty
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOPCITEMPROPERTY

  nsOPCProperty(const OPCITEMPROPERTY* prop);
  nsOPCProperty(LPCWSTR _itemId,DWORD _ID,LPCWSTR _description,const VARIANT* _value);

private:
  unsigned long ID;
  nsAutoString itemId;
  nsAutoString description;
  nsAutoString value;

  ~nsOPCProperty();

protected:
  /* additional members */
};

#endif // __NS_OPCPROPERTY__H__