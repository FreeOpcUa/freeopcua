// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef  __OPC_BROWSEELEMNT__H__
#define  __OPC_BROWSEELEMNT__H__

#include "mozopc.h"
#include "opcda.h"


/* Header file */
class nsOPCBrowseElement : public nsIOPCBrowseElement
{
  OPCBROWSEELEMENT elem;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOPCBROWSEELEMENT

  nsOPCBrowseElement(const OPCBROWSEELEMENT& el);

private:
  ~nsOPCBrowseElement();

protected:
  /* additional members */
};

#endif // __OPC_BROWSEELEMNT__H__