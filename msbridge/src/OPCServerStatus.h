// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_OPCSERVERSTATUS_H__
#define __MOZ_OPCSERVERSTATUS_H__

#include "mozopc.h"
#include <wchar.h>
#include <string>
#include <windows.h>
#include <objbase.h>

#include "opcda.h"

using namespace std;

class nsOPCServerStatus : public nsIOPCServerStatus
{
public:
  OPCSERVERSTATUS* status;

  NS_DECL_ISUPPORTS
  NS_DECL_NSIOPCSERVERSTATUS

  nsOPCServerStatus();


private:
  ~nsOPCServerStatus();

protected:
  /* additional members */
};



#endif // __MOZ_OPCSERVERSTATUS_H__