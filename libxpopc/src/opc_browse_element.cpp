#include "opc_browse_element.h"
#include "xp_strings.h"


NS_IMPL_ISUPPORTS1(XPCOM::nsOPCBrowseElement, nsIOPCBrowseElement)

XPCOM::nsOPCBrowseElement::nsOPCBrowseElement(const OpcClient::BrowseElement& element)
  : Element(element)
{
  /* member initializers and constructor code */
}

XPCOM::nsOPCBrowseElement::~nsOPCBrowseElement()
{
  /* destructor code */
}

/* readonly attribute wstring name; */
NS_IMETHODIMP XPCOM::nsOPCBrowseElement::GetName(PRUnichar * *aName)
{
  NS_ENSURE_ARG_POINTER(aName);
  *aName = ToPRUnichar(Element.Name);
  return NS_OK;
}

/* readonly attribute wstring szItemId; */
NS_IMETHODIMP XPCOM::nsOPCBrowseElement::GetItemID(PRUnichar * *aItemID)
{
  NS_ENSURE_ARG(aItemID);
  *aItemID = ToPRUnichar(Element.ID);
  return NS_OK;
}

/* readonly attribute boolean hasChildren; */
NS_IMETHODIMP XPCOM::nsOPCBrowseElement::GetHasChildren(PRBool *aHasChildren)
{
  NS_ENSURE_ARG(aHasChildren);
  *aHasChildren = Element.HasChildren ? PR_TRUE : PR_FALSE;
  return NS_OK;
}

/* readonly attribute boolean isItem; */
NS_IMETHODIMP XPCOM::nsOPCBrowseElement::GetIsItem(PRBool *aIsItem)
{
  NS_ENSURE_ARG(aIsItem);
  *aIsItem = Element.IsItem ? PR_TRUE : PR_FALSE;
  return NS_OK;
}

/* readonly attribute nsIArray Properties; */
NS_IMETHODIMP XPCOM::nsOPCBrowseElement::GetProperties(nsIArray * *aProperties)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}
