// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __OPC_ITEM_ATTRIBUTES__H__
#define __OPC_ITEM_ATTRIBUTES__H__


#include "mozopc.h"
#include "opcda.h"

#include <string>
/* Header file */
class nsOPCItemAttributes : public nsIOPCItemAttributes
{
  boolean bActive;

  unsigned long hClient;
  unsigned long hServer;
  unsigned long dwAccessRights;

  std::wstring szAccessPath;
  std::wstring szItemID;
  std::wstring vtRequestedDataType;
  std::wstring vtCanonicalDataType;
  std::wstring vEUInfo;

public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOPCITEMATTRIBUTES

  nsOPCItemAttributes(OPCITEMATTRIBUTES* attrs);

private:
  ~nsOPCItemAttributes();

protected:
  /* additional members */
};

//******************************************************************
//******************************************************************
//******************************************************************
//******************************************************************


/* Header file */
class nsItemAttributesEnum: public nsISimpleEnumerator
{
  DWORD cnt,cur;
  OPCITEMATTRIBUTES* attrs;//[100];
  BSTR str;
  IEnumOPCItemAttributes* en;
  IMalloc* mal;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISIMPLEENUMERATOR

  nsItemAttributesEnum(IUnknown* en);

  ~nsItemAttributesEnum ();

private:
  void Free();
protected:
  /* additional members */
};

#endif // __OPC_ITEM_ATTRIBUTES__H__