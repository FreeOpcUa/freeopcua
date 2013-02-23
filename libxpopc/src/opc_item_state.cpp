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

#include "opc_item_state.h"

namespace
{
  class nsOPCItemState : public XPCOM::nsIOPCItemStateEditor
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCITEMSTATE

    virtual void SetServerHandle(PRUint32 serverHandle);
    virtual void SetClientHandle(PRUint32 aClientHandle);
    virtual void SetTimeStamp(PRTime aTimeStamp);
    virtual void SetQuality(PRUint16 aQuality);
    virtual void SetDataValue(nsCOMPtr<nsIVariant> aDataValue);
    virtual void SetErrorCode(PRUint32 errorCode);

  public:
    nsOPCItemState()
      : ServerHandle(0)
      , ClientHandle(0)
      , TimeStamp(0)
      , Quality(0)
      , ErrorCode(0)
    {
    }

  private:
    PRUint32 ServerHandle;
    PRUint32 ClientHandle;
    PRTime TimeStamp;
    PRUint16 Quality;
    nsCOMPtr<nsIVariant> DataValue;
    PRUint32 ErrorCode;
  };

  NS_IMPL_ISUPPORTS1(nsOPCItemState, nsIOPCItemState);

  /* readonly attribute unsigned long ServerHandle; */
  NS_IMETHODIMP nsOPCItemState::GetServerHandle(PRUint32 *aServerHandle)
  {
    NS_ENSURE_ARG_POINTER(aServerHandle);
    *aServerHandle = ServerHandle;
    return NS_OK;
  }

  // readonly attribute unsigned long ClientHandle;
  NS_IMETHODIMP nsOPCItemState::GetClientHandle(PRUint32 *aClientHandle)
  {
    NS_ENSURE_ARG_POINTER(aClientHandle);
    *aClientHandle = ClientHandle;
    return NS_OK;
  }

  // readonly attribute PRTime TimeStamp;
  NS_IMETHODIMP nsOPCItemState::GetTimeStamp(PRTime *aTimeStamp)
  {
    NS_ENSURE_ARG_POINTER(aTimeStamp);
    *aTimeStamp = TimeStamp;
    return NS_OK;
  }

  // readonly attribute unsigned short Quality;
  NS_IMETHODIMP nsOPCItemState::GetQuality(PRUint16 *aQuality)
  {
    NS_ENSURE_ARG_POINTER(aQuality);
    *aQuality = Quality;
    return NS_OK;
  }

  // readonly attribute nsIVariant Data;
  NS_IMETHODIMP nsOPCItemState::GetData(nsIVariant **aDataValue)
  {
    NS_ENSURE_ARG_POINTER(aDataValue); 
    return CallQueryInterface(DataValue, aDataValue);
  }

  // readonly attribute unsigned long ErrorCode;
  NS_IMETHODIMP nsOPCItemState::GetErrorCode(PRUint32 *aErrorCode)
  {
    NS_ENSURE_ARG_POINTER(aErrorCode);
    *aErrorCode = ErrorCode;
    return NS_OK;
  }

  void nsOPCItemState::SetServerHandle(PRUint32 serverHandle)
  {
    ServerHandle = serverHandle;
  }

  void nsOPCItemState::SetClientHandle(PRUint32 aClientHandle)
  {
    ClientHandle = aClientHandle;
  }

  void nsOPCItemState::SetTimeStamp(PRTime aTimeStamp)
  {
    TimeStamp = aTimeStamp;
  }

  void nsOPCItemState::SetQuality(PRUint16 aQuality)
  {
    Quality = aQuality;
  }

  void nsOPCItemState::SetDataValue(nsCOMPtr<nsIVariant> aDataValue)
  {
    DataValue = aDataValue;
  }

  void nsOPCItemState::SetErrorCode(PRUint32 errorCode)
  {
    ErrorCode = errorCode;
  }
} // namespace

namespace XPCOM
{
  nsCOMPtr<nsIOPCItemStateEditor> CreateOPCItemState()
  {
    return nsCOMPtr<nsIOPCItemStateEditor>(new nsOPCItemState);
  }
} // namespace XPCOM

