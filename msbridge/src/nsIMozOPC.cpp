// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include "nsIMozOPC.h"

#if (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#include "../../mozopc/src/nsOPC.h"

#include "opcda.h"
#include <atlbase.h>
#include <objbase.h>

#include <nsStringAPI.h>


#include "OPCServer.h"

namespace
{
  const wchar_t LOCALHOST_NAME[] = L"localhost";
  inline void ThrowIfError(HRESULT hr)
  {
    if (FAILED(hr))
    {
      throw hr;
    }
  }
 
  template<class Interface>
  CComPtr<Interface> CreateInstance(REFCLSID clsId)
  {
    CComPtr<Interface> server;
    ThrowIfError(CoCreateInstance(clsId, NULL, CLSCTX_ALL, __uuidof(Interface), reinterpret_cast<void**>(&server)));
    return server;
  }

  template<class Interface>
  CComPtr<Interface> CreateInstance(REFCLSID clsId, const wchar_t* host)
  {
    if (host == NULL)
    {
      return CreateInstance<Interface>(clsId);
    }

    REFIID iid = __uuidof(Interface);
    MULTI_QI clsData = {0};
    clsData.pIID = &iid; 

    CComBSTR hostString = static_cast<const wchar_t*>(host);
    COSERVERINFO osInfo = {0};
    osInfo.pwszName = hostString;
    osInfo.pAuthInfo  = NULL; // TODO: May be required to create server under anover account
    ThrowIfError(CoCreateInstanceEx(clsId, NULL, CLSCTX_ALL, &osInfo, 1, &clsData));

    return CComPtr<Interface>(reinterpret_cast<Interface*>(clsData.pItf));
  }
}

NS_IMPL_ISUPPORTS1(MsOpcBridge::nsMsOpc, nsIMsOpc);
namespace MsOpcBridge
{

  nsMsOpc::nsMsOpc(nsISupports* outer)
    : Outer(outer)
  {
  }

  nsMsOpc::~nsMsOpc()
  {
  }

  // nsISupports create (in wstring cls_name, in wstring host); 
  NS_IMETHODIMP nsMsOpc::Create(const PRUnichar *clsName, const PRUnichar *host, nsISupports **retval NS_OUTPARAM)
  {
    *retval = NULL;

    CLSID clsId = {0};
    {
      CComBSTR idString = static_cast<const wchar_t*>(clsName);
      if (FAILED(CLSIDFromString(idString, &clsId)) && FAILED(CLSIDFromProgID(idString, &clsId))) 
      {
        return NS_ERROR_FAILURE;
      }
    }
    
    CComPtr<IOPCServer> server = CreateInstance<IOPCServer>(clsId, host);
    *retval = new nsOPCServer(server);
    return NS_OK;
  }

//  PRUnichar* aCls_name = ToNewUnicode(NS_ConvertASCIItoUTF16(NS_LITERAL_CSTRING(ClassName)));
}

namespace OPC
{
  namespace XPCOM
  {
    nsISupports* CreateAggregableClassObject(nsISupports* outer)
    { 
      return NULL;
    }
  }
}
#endif // (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

namespace OPC
{
  namespace COM
  {
    IUnknown* CreateAggregableClassObject(IUnknown* outer)
    { 
      return NULL;
    }
  }
}
