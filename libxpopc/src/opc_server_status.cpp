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

#include "opc_server_status.h"
#include <nsStringAPI.h>


NS_IMPL_ISUPPORTS1(XPCOM::nsOPCServerStatus, nsIOPCServerStatus)


XPCOM::nsOPCServerStatus::nsOPCServerStatus(const OpcClient::ServerStatus& status)
  : Status(status)
{
  /* member initializers and constructor code */
}

XPCOM::nsOPCServerStatus::~nsOPCServerStatus()
{
  /* destructor code */
}

/* readonly attribute PRTime ftStartTime; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetFtStartTime(PRTime *aFtStartTime)
{
  NS_ENSURE_ARG_POINTER(aFtStartTime);
  *aFtStartTime = static_cast<PRTime>(Status.StartTime);
  return NS_OK;
}

/* readonly attribute PRTime ftCurrentTime; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetFtCurrentTime(PRTime *aFtCurrentTime)
{
  NS_ENSURE_ARG_POINTER(aFtCurrentTime);
  *aFtCurrentTime = PR_Now();
  return NS_OK;
}

/* readonly attribute PRTime ftLastUpdateTime; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetFtLastUpdateTime(PRTime *aFtLastUpdateTime)
{
  NS_ENSURE_ARG_POINTER(aFtLastUpdateTime);
  *aFtLastUpdateTime = static_cast<PRTime>(Status.LastUpdateTime);
  return NS_OK;
}

namespace
{
  inline nsresult GetServerState(OpcClient::ServerState state, PRUint32& result)
  {
    switch(state)
    {
    case OpcClient::STATE_RUNNING:
      result = nsIOPCServerStatus::OPC_STATUS_RUNNING;
      return NS_OK;
    case OpcClient::STATE_NOCONFIG:
      result = nsIOPCServerStatus::OPC_STATUS_NOCONFIG;
      return NS_OK;
    case OpcClient::STATE_SUSPENDED:
      result = nsIOPCServerStatus::OPC_STATUS_SUSPENDED;
      return NS_OK;
    case OpcClient::STATE_TEST:
      result = nsIOPCServerStatus::OPC_STATUS_TEST;
      return NS_OK;
    case OpcClient::STATE_COMM_FAULT:
      result = nsIOPCServerStatus::OPC_STATUS_COMM_FAULT;
      return NS_OK;
    case OpcClient::STATE_FAILED:
      result = nsIOPCServerStatus::OPC_STATUS_FAILED;
      return NS_OK;
    }
    return NS_ERROR_FAILURE;
  }
}

/* readonly attribute PRUint32 dwServerState; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetDwServerState(PRUint32 *aDwServerState)
{
  NS_ENSURE_ARG_POINTER(aDwServerState);
  return GetServerState(Status.State, *aDwServerState);
}

/* readonly attribute PRUint32 dwGroupCount; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetDwGroupCount(PRUint32 *aDwGroupCount)
{
  NS_ENSURE_ARG_POINTER(aDwGroupCount);
  *aDwGroupCount = Status.GroupCount;
  return NS_OK;
}

/* readonly attribute PRUint32 dwBandWidth; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetDwBandWidth(PRUint32 *aDwBandWidth)
{
  NS_ENSURE_ARG_POINTER(aDwBandWidth);
  *aDwBandWidth = Status.BandWidth;
  return NS_OK;
}

/* readonly attribute PRUint32 wMajorVersion; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetWMajorVersion(PRUint32 *aWMajorVersion)
{
  NS_ENSURE_ARG_POINTER(aWMajorVersion);
  *aWMajorVersion = Status.MajorVersion;
  return NS_OK;
}

/* readonly attribute PRUint32 wMinorVersion; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetWMinorVersion(PRUint32 *aWMinorVersion)
{
  NS_ENSURE_ARG_POINTER(aWMinorVersion);
  *aWMinorVersion = Status.MinorVersion;
  return NS_OK;
}

/* readonly attribute PRUint32 wBuildNumber; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetWBuildNumber(PRUint32 *aWBuildNumber)
{
  NS_ENSURE_ARG_POINTER(aWBuildNumber);
  *aWBuildNumber = Status.BuildNumber;
  return NS_OK;
}

/* readonly attribute wstring szVendorInfo; */
NS_IMETHODIMP XPCOM::nsOPCServerStatus::GetSzVendorInfo(PRUnichar * *aSzVendorInfo)
{
  *aSzVendorInfo = ToNewUnicode(NS_ConvertUTF8toUTF16(Status.VendorInfo.c_str()));
  return NS_OK;
}
