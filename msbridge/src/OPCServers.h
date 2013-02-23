// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_OPC_ENUM_H__
#define __MOZ_OPC_ENUM_H__


#define MOZILLA_STRICT_API

#include "nsIStringEnumerator.h"

#include <string>

using namespace std;

#include <windows.h>
#include <comcat.h>


/* Header file */
class nsOPCServers : public nsIStringEnumerator
{
    DWORD cnt,cur;
    GUID guids[100];
    BSTR str;
    IEnumCLSID* en;
    IMalloc* mal;
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISTRINGENUMERATOR
  nsOPCServers();

  static const nsCID cid;
  static const char* class_name;
  static const char* contract_id;

private:
  ~nsOPCServers();
  void Free();
protected:
  /* additional members */
};

#endif