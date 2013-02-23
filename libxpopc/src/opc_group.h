/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of OPC group object
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __opc_group_h__052f2a3c_d812_4bf0_8916_0378850e0584
#define __opc_group_h__052f2a3c_d812_4bf0_8916_0378850e0584

#include <mozopc.h>
#include <opc_client/group.h>

namespace OpcClient
{
  class Group;
}

namespace XPCOM
{
  nsCOMPtr<nsISupports> CreateGroup(std::shared_ptr<OpcClient::Group> group);
}

#endif // __opc_group_h__052f2a3c_d812_4bf0_8916_0378850e0584

