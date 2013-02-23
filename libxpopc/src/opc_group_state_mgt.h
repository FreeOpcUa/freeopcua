/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCGroupStateMgt interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __opc_group_state_mgt_h__07f03d94_035d_49e5_a96b_9fbca63aa853
#define __opc_group_state_mgt_h__07f03d94_035d_49e5_a96b_9fbca63aa853


#include <nsCOMPtr.h>
#include <opc_client/group.h>


class nsIOPCGroupStateMgt;

namespace XPCOM
{
  nsCOMPtr<nsIOPCGroupStateMgt> CreateGroupStateMgt(std::shared_ptr<OpcClient::Group> group);
}

#endif //  __opc_group_state_mgt_h__07f03d94_035d_49e5_a96b_9fbca63aa853
