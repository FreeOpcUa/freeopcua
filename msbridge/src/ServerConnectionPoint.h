// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef NS_IENUM_CONNECTION_POINTS__H__
#define NS_IENUM_CONNECTION_POINTS__H__


#include "mozopc.h"

#include <windows.h>
#include <ocidl.h>

/* Header file */
class nsEnumConnectionPoints : public nsISimpleEnumerator
{
  DWORD cnt,cur;
  IConnectionPoint* points[100];
  BSTR str;
  IEnumConnectionPoints  * en;
  IMalloc* mal;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISIMPLEENUMERATOR

  nsEnumConnectionPoints (IUnknown* en);

  ~nsEnumConnectionPoints ();

private:
  void Free();
protected:
  /* additional members */
};

#endif // NS_IENUM_CONNECTION_POINTS__H__