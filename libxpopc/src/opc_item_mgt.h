/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCItemMgt interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __opc_item_mgt_h__932d4879_db42_495b_aa4c_ed6c23ffc317
#define __opc_item_mgt_h__932d4879_db42_495b_aa4c_ed6c23ffc317

#include <nsCOMPtr.h>
#include <opc_client/group.h>

class nsIOPCItemMgt;

namespace XPCOM
{
  nsCOMPtr<nsIOPCItemMgt> CreateItemMgt(std::shared_ptr<OpcClient::Group> group);
}

#endif // __opc_item_mgt_h__932d4879_db42_495b_aa4c_ed6c23ffc317

