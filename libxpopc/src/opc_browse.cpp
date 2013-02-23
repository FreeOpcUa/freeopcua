#include "opc_browse.h"

#include "errors.h"
#include "ns_simple_enumerator.h"
#include "opc_browse_element.h"
#include "xp_strings.h"

#include <nsISupportsPrimitives.h>
#include <mozopc.h>


namespace
{
  inline bool IsValidBrowseFilter(PRUint16 filter)
  {
    switch (filter)
    {
      case nsIOPCBrowse::OPC_BROWSE_FILTER_ALL:
      case nsIOPCBrowse::OPC_BROWSE_FILTER_BRANCHES:
      case nsIOPCBrowse::OPC_BROWSE_FILTER_ITEMS:
        return true;

      default:
        return false;
    }
  }

  inline OpcClient::BrowseFilter GetBrowseFilter(PRUint16 filter)
  {
    switch (filter)
    {
      case nsIOPCBrowse::OPC_BROWSE_FILTER_ALL:
        return OpcClient::OPC_BROWSE_FILTER_ALL;

      case nsIOPCBrowse::OPC_BROWSE_FILTER_BRANCHES:
        return OpcClient::OPC_BROWSE_FILTER_BRANCHES;

      case nsIOPCBrowse::OPC_BROWSE_FILTER_ITEMS:
        return OpcClient::OPC_BROWSE_FILTER_ITEMS;

      default:
        THROW_ERROR1(InvalidBrowserFilterValue, filter);
    }
  }
  
  inline nsCOMPtr<nsISupports> CreateNsBrowseElement(const OpcClient::BrowseElement& element)
  {
    return nsCOMPtr<nsISupports>(new XPCOM::nsOPCBrowseElement(element));
  }

  inline void ThrowIfError(nsresult res, const char* msg)
  {
    if (NS_FAILED(res))
    {
      THROW_ERROR2(XPCOMError, res, msg);
    }
  }

  inline bool HasMoreElements(nsISimpleEnumerator& en)
  {
    PRBool hasMoreElements = PR_FALSE;
    ThrowIfError(en.HasMoreElements(&hasMoreElements), "There is no elements in the enumerator.");
    return hasMoreElements == PR_TRUE;
  }

  inline nsCOMPtr<nsISupports> GetNextElement(nsISimpleEnumerator& enumerator)
  {
    nsCOMPtr<nsISupports> resultPtr;
    ThrowIfError(enumerator.GetNext(getter_AddRefs(resultPtr)), "Failed to obtain next element.");
    return resultPtr;
  }

  inline PRUint32 GetValue(nsISupportsPRUint32& integer)
  {
    PRUint32 result = 0;
    ThrowIfError(integer.GetData(&result), "Failed to get data from nsISupportsPRUint32.");
    return result;
  }

  std::vector<Common::ObjectID> GetPropertiesIDs(nsISimpleEnumerator& propertiesIDs)
  {
    std::vector<Common::ObjectID> ids;
    while(HasMoreElements(propertiesIDs))
    {
      nsCOMPtr<nsISupports> element = GetNextElement(propertiesIDs);
      nsCOMPtr<nsISupportsPRUint32> integer(do_QueryInterface(element));
      ids.push_back(static_cast<Common::ObjectID>(GetValue(*integer)));
    }
    return ids;
  }

  class nsOPCBrowse : public nsIOPCBrowse
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCBROWSE

    nsOPCBrowse()
      : Browser(OpcClient::CreateServerBrowser())
    {
    }

    virtual ~nsOPCBrowse()
    {
    }
  
  private:
    nsresult ProcessError(const Common::Error& error);

  private:
    const std::unique_ptr<OpcClient::ServerBrowser> Browser;
  };

  NS_IMPL_ISUPPORTS1(nsOPCBrowse, nsIOPCBrowse)

  // nsISimpleEnumerator browse (in wstring szItemID, in unsigned short dwBrowseFilter, in wstring szElementNameFilter, in wstring szVendorFilter);
  NS_IMETHODIMP nsOPCBrowse::Browse(const PRUnichar *szItemID, PRUint16 dwBrowseFilter, const PRUnichar *szElementNameFilter, const PRUnichar *szVendorFilter, PRBool bAllProperties, PRBool bPropertiesValues, nsIArray *propertiesIDs, nsISimpleEnumerator **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK

    NS_ENSURE_ARG(szItemID);
    NS_ENSURE_ARG(IsValidBrowseFilter(dwBrowseFilter));
    NS_ENSURE_ARG(szElementNameFilter);
    NS_ENSURE_ARG(szVendorFilter);
    NS_ENSURE_ARG_POINTER(_retval);

    const std::string& itemID = XPCOM::ToStdString(szItemID);
    const std::string& nameFilter = XPCOM::ToStdString(szElementNameFilter);
    const std::string& vendorFilter = XPCOM::ToStdString(szVendorFilter);
    bool allProperties = (bAllProperties == PR_TRUE);
    bool propertiesValues = (bPropertiesValues == PR_TRUE);
    OpcClient::BrowseFilter browseFilter = GetBrowseFilter(dwBrowseFilter);
    nsCOMPtr<nsISimpleEnumerator> propertiesEnum;
    // TODO add properties supporting
    std::vector<Common::ObjectID> ids;
    //  if (propertiesIDs && propertiesIDs->Enumerate(getter_AddRefs(propertiesEnum)) == NS_OK && propertiesEnum)
    //  {
    //    ids = GetPropertiesIDs(*propertiesEnum);
    //  }
    // Below parameters not added to IDL source because it gets complicity.
    // Using Enumerator as result and this parameters can be emulated with it.
    //bool moreElements = false;  // TODO Add if will be needed later to XPIDL.
    //unsigned maxElementsReturned = 0; // todo Add if will be needed to XPIDL source
    const std::vector<OpcClient::BrowseElement>& browsedElements = Browser->Browse(itemID, browseFilter, nameFilter, vendorFilter, allProperties, propertiesValues, ids);
    std::vector<nsCOMPtr<nsISupports> > elements;
    std::transform(browsedElements.begin(), browsedElements.end(), std::back_inserter(elements), CreateNsBrowseElement);

    nsCOMPtr<nsISimpleEnumerator> enumerator = XPCOM::CreateSimpleEnumerator(elements);
    enumerator.forget(_retval);
    return NS_OK;

    END_TRY_BLOCK(ProcessError);
  }

  // nsIArray getProperties (in wstring itemID);
  NS_IMETHODIMP nsOPCBrowse::GetProperties(const PRUnichar *itemID, nsIArray **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }


  nsresult nsOPCBrowse::ProcessError(const Common::Error& error)
  {
    return NS_ERROR_FAILURE;
  }
} // namespace

nsCOMPtr<nsIOPCBrowse> XPCOM::CreateOpcBrowse()
{
  return nsCOMPtr<nsIOPCBrowse>(new nsOPCBrowse());
}

