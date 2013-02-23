/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCServerStatus
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __ns_opc_server_status_h__313ff6ab_cea8_4839_82a3_7b678b329848
#define __ns_opc_server_status_h__313ff6ab_cea8_4839_82a3_7b678b329848

#include <mozopc.h>
#include <opc_client/server.h>

namespace OpcClient
{
  struct ServerStatus;
}

namespace XPCOM
{

  /// @brief Class for acceessing to server status data
  class nsOPCServerStatus : public nsIOPCServerStatus
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCSERVERSTATUS

    explicit nsOPCServerStatus(const OpcClient::ServerStatus& status);
    virtual ~nsOPCServerStatus();

  private:
    OpcClient::ServerStatus Status;
  };
}

#endif // __ns_opc_server_status_h__313ff6ab_cea8_4839_82a3_7b678b329848
