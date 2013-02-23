/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCItemState interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __opc_item_state_h__2d30852d_49f8_4f47_a5fd_730864c0e489
#define __opc_item_state_h__2d30852d_49f8_4f47_a5fd_730864c0e489

#include <boost/shared_ptr.hpp>
#include <mozopc.h>
#include <nsCOMPtr.h>


namespace OpcClient
{
  class Group;
}

namespace XPCOM
{
  class nsIOPCItemStateEditor : public nsIOPCItemState
  {
  public:
    virtual void SetServerHandle(PRUint32 serverHandle) = 0;
    virtual void SetClientHandle(PRUint32 aClientHandle) = 0;
    virtual void SetTimeStamp(PRTime aTimeStamp) = 0;
    virtual void SetQuality(PRUint16 aQuality) = 0;
    virtual void SetDataValue(nsCOMPtr<nsIVariant> aDataValue) = 0;
    virtual void SetErrorCode(PRUint32 errorCode) = 0;
 };

  nsCOMPtr<nsIOPCItemStateEditor> CreateOPCItemState();
} // namespace XPCOM

#endif // __opc_item_state_h__2d30852d_49f8_4f47_a5fd_730864c0e489

