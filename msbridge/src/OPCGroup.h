// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_OPCGROUP_H__
#define __MOZ_OPCGROUP_H__

#if (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#include <atlbase.h>
#include <mozopc.h>
#include <opcda.h>

namespace MsOpcBridge
{
  class nsOPCGroupStateMgt : public nsIOPCGroupStateMgt
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCGROUPSTATEMGT

    nsOPCGroupStateMgt(CComPtr<IOPCGroupStateMgt> group);

  private:
    ~nsOPCGroupStateMgt();

  private:
    CComPtr<IOPCGroupStateMgt> Group;
  };
}

#endif // (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#endif // __MOZ_OPCSERVER_H__
