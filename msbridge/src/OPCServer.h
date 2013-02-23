// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_OPCSERVER_H__
#define __MOZ_OPCSERVER_H__

#if (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#include <mozopc.h>
#include <opcda.h>
#include <opccommon.h>

#include <atlbase.h>

namespace MsOpcBridge
{
  class nsOPCServer : public nsIOPCServer
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCSERVER

    nsOPCServer(CComPtr<IOPCServer> opcServer);

  private:
    ~nsOPCServer();

  private:
    CComPtr<IOPCServer> OpcServer;
  };
} // namespace MsOpcBridge

#endif // (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)


#endif // __MOZ_OPCSERVER_H__