/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCItemResult interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __opc_item_result_h__78cd4fce_1d4a_4541_a1ef_8dd5909e564f
#define __opc_item_result_h__78cd4fce_1d4a_4541_a1ef_8dd5909e564f

#include <nsCOMPtr.h>

class nsIOPCItemResult;

namespace XPCOM
{
  nsCOMPtr<nsIOPCItemResult> CreateItemResult(PRUint32 serverHandle, PRUint32 accessRights, PRUint16 dataType, PRUint32 errorCode);
}

#endif // __opc_item_result_h__78cd4fce_1d4a_4541_a1ef_8dd5909e564f
