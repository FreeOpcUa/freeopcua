/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCServer
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __MOZ_NS_IOPCSERVER_H__
#define __MOZ_NS_IOPCSERVER_H__

#include <mozopc.h>
#include <opc_client/server.h>
#include <opc_client/group.h>

#include <string>

namespace XPCOM
{
  class nsOPCServer
    : public nsIOPCServer
    , public nsIOPCBrowse
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCSERVER
    NS_DECL_NSIOPCBROWSE

  public:
    nsOPCServer();

  private:
    virtual ~nsOPCServer();
    // @throw not throws.
    nsresult ProcessError(const Common::Error& error);

  private:
    const std::unique_ptr<OpcClient::Server> Server;
    nsCOMPtr<nsIOPCBrowse> OpcBrowse;
  };
}

#endif // __MOZ_NS_IOPCSERVER_H__

