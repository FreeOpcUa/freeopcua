/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCSyncIO
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef ___ns_opc_sync_io_h__9b3e7604_8356_4738_a91b_90b2d69ccf8
#define ___ns_opc_sync_io_h__9b3e7604_8356_4738_a91b_90b2d69ccf8

#include <memory>
#include <nsCOMPtr.h>


class nsIOPCSyncIO;

namespace OpcClient
{
  struct SyncIO;
}

namespace XPCOM
{
  nsCOMPtr<nsIOPCSyncIO> CreateSyncIO(std::unique_ptr<OpcClient::SyncIO> syncIO);
}

#endif // __ns_opc_sync_io_h__9b3e7604_8356_4738_a91b_90b2d69ccf8

