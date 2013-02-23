// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef NS_GROUPENUMERATORS__H__
#define NS_GROUPENUMERATORS__H__


#include "mozopc.h"

#include <windows.h>
#include "opcda.h"

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************


/* Header file */
class nsGroupStringEnum : public nsIStringEnumerator
{
  DWORD cnt,cur;
  wchar_t* groups[100];
  BSTR str;
  IEnumString* en;
  IMalloc* mal;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISTRINGENUMERATOR

  nsGroupStringEnum(IUnknown* en);

  ~nsGroupStringEnum();

private:
  void Free();
protected:
  /* additional members */
};

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************


/* Header file */
class nsGroupSupportsEnum : public nsISimpleEnumerator
{
  DWORD cnt,cur;
  IUnknown* groups[100];
  BSTR str;
  IEnumUnknown* en;
  IMalloc* mal;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISIMPLEENUMERATOR

  nsGroupSupportsEnum (IUnknown* en);

  ~nsGroupSupportsEnum ();

private:
  void Free();
protected:
  /* additional members */
};

#endif // NS_GROUPENUMERATORS__H__