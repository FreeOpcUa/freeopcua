/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCGroupInterfacesFactory
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef  __opc_group_interfces_factory_h__9246a666_0aae_4758_bc54_c2bb0ed1131a
#define  __opc_group_interfces_factory_h__9246a666_0aae_4758_bc54_c2bb0ed1131a

#include <nsCOMPtr.h>

class nsIOPCGroupInterfacesFactory;

namespace XPCOM
{
  nsCOMPtr<nsIOPCGroupInterfacesFactory> CreateGroupInterfacesFactory();
}

#endif //  __opc_group_interfces_factory_h__9246a666_0aae_4758_bc54_c2bb0ed1131a
