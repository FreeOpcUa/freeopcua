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

#include "opc_item_result.h"

#include <mozopc.h>
#include <nsStringAPI.h>

namespace
{
  class nsOPCItemResult : public nsIOPCItemResult
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCITEMRESULT

  public:
    nsOPCItemResult(PRUint32 serverHandle, PRUint32 accessRights, PRUint16 dataType, PRUint32 errorCode)
      : ServerHandle(serverHandle)
      , AccessRights(accessRights)
      , DataType(dataType)
      , ErrorCode(errorCode)
    {
    }

  private:
    const PRUint32 ServerHandle;
    const PRUint32 AccessRights;
    const PRUint16 DataType;
    const PRUint32 ErrorCode;   
  };

  /* Implementation file */
  NS_IMPL_ISUPPORTS1(nsOPCItemResult, nsIOPCItemResult)

  // readonly attribute unsigned long ServerHandle
  NS_IMETHODIMP nsOPCItemResult::GetServerHandle(PRUint32 *aServerHandle)
  {
    NS_ENSURE_ARG_POINTER(aServerHandle);
    *aServerHandle = ServerHandle;
    return NS_OK;
  }

  // readonly attribute unsigned long AccessRights
  NS_IMETHODIMP nsOPCItemResult::GetAccessRights(PRUint32 *aAccessRights)
  {
    NS_ENSURE_ARG_POINTER(aAccessRights);
    *aAccessRights = AccessRights;
    return NS_OK;
  }

  // readonly attribute AString DataType;
  NS_IMETHODIMP nsOPCItemResult::GetDataType(PRUint16  *aDataType)
  {
    NS_ENSURE_ARG_POINTER(aDataType);
    *aDataType = DataType;
    return NS_OK;
  }

  // readonly attribute unsigned long Error;
  NS_IMETHODIMP nsOPCItemResult::GetError(PRUint32 *aError)
  {
    NS_ENSURE_ARG_POINTER(aError);
    *aError = ErrorCode;
    return NS_OK;
  }
}

namespace XPCOM
{
  nsCOMPtr<nsIOPCItemResult> CreateItemResult(PRUint32 serverHandle, PRUint32 accessRights, PRUint16 dataType, PRUint32 errorCode)
  {
    return nsCOMPtr<nsIOPCItemResult>(new nsOPCItemResult(serverHandle, accessRights, dataType, errorCode));
  }
}

