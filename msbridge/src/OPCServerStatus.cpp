// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <nsStringAPI.h>
#include <locale/nsIDateTimeFormat.h>
#include <nsIComponentManager.h>
#include "OPCServerStatus.h"

NS_IMPL_ISUPPORTS1(nsOPCServerStatus, nsIOPCServerStatus)

nsOPCServerStatus::nsOPCServerStatus()
{
  /* member initializers and constructor code */
  status=NULL;
}

nsOPCServerStatus::~nsOPCServerStatus()
{
  /* destructor code */
//  SysFreeString(status->szVendorInfo);
  LPMALLOC Malloc=NULL; 
  if(SUCCEEDED(CoGetMalloc(1,&Malloc)))
  {
    Malloc->Free(status);
  }
}

/* readonly attribute wstring ftStartTime; */
NS_IMETHODIMP nsOPCServerStatus::GetFtStartTime(PRUnichar * *aFtStartTime)
{
 nsAutoString wstr;
 nsresult res;
 
 // create an instance
 nsIComponentManager* mgr=NULL;
 res = NS_GetComponentManager(&mgr);
 if(NS_FAILED(res))
   return res;


  nsIDateTimeFormat* fmt=NULL;
  res=mgr->CreateInstanceByContractID("@mozilla.org/intl/datetimeformat;1",
         NULL, nsIDateTimeFormat::GetIID(), (void**) &fmt);

  if(NS_SUCCEEDED(res))
  {
    // format by using FormatTime
    PRExplodedTime t={0};
    SYSTEMTIME st;
    FileTimeToSystemTime(&status->ftStartTime,&st);
    SystemTimeToTzSpecificLocalTime(NULL,&st,&st);
    t.tm_year=st.wYear;
    t.tm_month=st.wMonth;
    t.tm_mday=st.wDay;
    t.tm_wday=st.wDayOfWeek;
    t.tm_hour=st.wHour;
    t.tm_min=st.wMinute;
    t.tm_sec=st.wSecond;
    t.tm_usec=st.wMilliseconds*1000;

    res = fmt->FormatPRExplodedTime(NULL, kDateFormatShort, kTimeFormatSeconds,
                            &t, wstr);
    if(NS_SUCCEEDED(res))
      *aFtStartTime=ToNewUnicode(wstr);
  }


  if(mgr!=NULL)
    NS_RELEASE(mgr);

  if(fmt!=NULL)
    NS_RELEASE(fmt);

  return res;
}

/* readonly attribute wstring ftCurrentTime; */
NS_IMETHODIMP nsOPCServerStatus::GetFtCurrentTime(PRUnichar * *aFtCurrentTime)
{
 nsAutoString wstr;
 nsresult res;
 
 // create an instance
 nsIComponentManager* mgr=NULL;
 res = NS_GetComponentManager(&mgr);
 if(NS_FAILED(res))
   return res;


  nsIDateTimeFormat* fmt=NULL;
  res=mgr->CreateInstanceByContractID("@mozilla.org/intl/datetimeformat;1",
         NULL, nsIDateTimeFormat::GetIID(), (void**) &fmt);

  if(NS_SUCCEEDED(res))
  {
    // format by using FormatTime
    PRExplodedTime t={0};
    SYSTEMTIME st;
    FileTimeToSystemTime(&status->ftCurrentTime,&st);
    SystemTimeToTzSpecificLocalTime(NULL,&st,&st);

    t.tm_year=st.wYear;
    t.tm_month=st.wMonth;
    t.tm_mday=st.wDay;
    t.tm_wday=st.wDayOfWeek;
    t.tm_hour=st.wHour;
    t.tm_min=st.wMinute;
    t.tm_sec=st.wSecond;
    t.tm_usec=st.wMilliseconds*1000;

    res = fmt->FormatPRExplodedTime(NULL, kDateFormatShort, kTimeFormatSeconds,
                            &t, wstr);
    if(NS_SUCCEEDED(res))
      *aFtCurrentTime=ToNewUnicode(wstr);
  }


  if(mgr!=NULL)
    NS_RELEASE(mgr);

  if(fmt!=NULL)
    NS_RELEASE(fmt);

  return res;
}

/* readonly attribute wstring ftLastUpdateTime; */
NS_IMETHODIMP nsOPCServerStatus::GetFtLastUpdateTime(PRUnichar * *aFtLastUpdateTime)
{
 nsAutoString wstr;
 nsresult res;
 
 // create an instance
 nsIComponentManager* mgr=NULL;
 res = NS_GetComponentManager(&mgr);
 if(NS_FAILED(res))
   return res;


  nsIDateTimeFormat* fmt=NULL;
  res=mgr->CreateInstanceByContractID("@mozilla.org/intl/datetimeformat;1",
         NULL, nsIDateTimeFormat::GetIID(), (void**) &fmt);

  if(NS_SUCCEEDED(res))
  {
    // format by using FormatTime
    PRExplodedTime t={0};
    SYSTEMTIME st;
    FileTimeToSystemTime(&status->ftLastUpdateTime,&st);
    SystemTimeToTzSpecificLocalTime(NULL,&st,&st);

    t.tm_year=st.wYear;
    t.tm_month=st.wMonth;
    t.tm_mday=st.wDay;
    t.tm_wday=st.wDayOfWeek;
    t.tm_hour=st.wHour;
    t.tm_min=st.wMinute;
    t.tm_sec=st.wSecond;
    t.tm_usec=st.wMilliseconds*1000;

    res = fmt->FormatPRExplodedTime(NULL, kDateFormatShort, kTimeFormatSeconds,
                            &t, wstr);
    if(NS_SUCCEEDED(res))
      *aFtLastUpdateTime=ToNewUnicode(wstr);
  }


  if(mgr!=NULL)
    NS_RELEASE(mgr);

  if(fmt!=NULL)
    NS_RELEASE(fmt);

  return res;
}

/* readonly attribute PRUint32 dwServerState; */
NS_IMETHODIMP nsOPCServerStatus::GetDwServerState(PRUint32 *aDwServerState)
{
  *aDwServerState=status->dwServerState;
  return NS_OK;
}

/* readonly attribute PRUint32 dwGroupCount; */
NS_IMETHODIMP nsOPCServerStatus::GetDwGroupCount(PRUint32 *aDwGroupCount)
{
  *aDwGroupCount=status->dwGroupCount;
  return NS_OK;
}

/* readonly attribute PRUint32 dwBandWidth; */
NS_IMETHODIMP nsOPCServerStatus::GetDwBandWidth(PRUint32 *aDwBandWidth)
{
  if(status->dwBandWidth!=(DWORD)-1)
  {
    *aDwBandWidth=status->dwBandWidth;
  }
  return NS_OK;
}

/* readonly attribute PRUint32 wMajorVersion; */
NS_IMETHODIMP nsOPCServerStatus::GetWMajorVersion(PRUint32 *aWMajorVersion)
{
  *aWMajorVersion=status->wMajorVersion;
  return NS_OK;
}

/* readonly attribute PRUint32 wMinorVersion; */
NS_IMETHODIMP nsOPCServerStatus::GetWMinorVersion(PRUint32 *aWMinorVersion)
{
  *aWMinorVersion=status->wMinorVersion;
  return NS_OK;
}

/* readonly attribute PRUint32 wBuildNumber; */
NS_IMETHODIMP nsOPCServerStatus::GetWBuildNumber(PRUint32 *aWBuildNumber)
{
  *aWBuildNumber=status->wBuildNumber;
  return NS_OK;
}

/* readonly attribute wstring szVendorInfo; */
NS_IMETHODIMP nsOPCServerStatus::GetSzVendorInfo(PRUnichar * *aSzVendorInfo)
{
  nsAutoString wstr;
  wstr.Assign(status->szVendorInfo);
  *aSzVendorInfo=ToNewUnicode(wstr);
  return NS_OK;
}
