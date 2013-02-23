/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCItemMgt interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "opc_item_attributes.h"

#include <nsStringAPI.h>
#include <nsIVariant.h>


namespace
{
  class nsOPCItemAttributes : public XPCOM::nsIOPCItemAttributesEditor
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCITEMATTRIBUTES

    void SetAccessPath(const nsAString& aAccessPath);
    void SetItemID(const nsAString& aItemID);
    void SetIsActive(PRBool aIsActive);
    void SetClientHandle(PRUint32 aClientHandle);
    void SetServerHandle(PRUint32 aServerHandle);
    void SetAccessRights(PRUint32 aAccessRights);
    void SetRequestedDataType(PRUint32 aRequestedDataType);
    void SetCanonicalDataType(PRUint32 aCanonicalDataType);

  public:
    nsOPCItemAttributes();

  private:
    ~nsOPCItemAttributes();

  private:
    nsString AccessPath;
    nsString ItemID;
    PRBool IsActive;
    PRUint32 ClientHandle;
    PRUint32 ServerHandle;
    PRUint32 AccessRights;
    PRUint32 RequestedDataType;
    PRUint32 CanonicalDataType; 
  };

  NS_IMPL_ISUPPORTS1(nsOPCItemAttributes, nsIOPCItemAttributes)

  nsOPCItemAttributes::nsOPCItemAttributes()
    : ClientHandle(0)
    , ServerHandle(0)
    , AccessRights(0)
    , RequestedDataType(nsIDataType::VTYPE_EMPTY)
    , CanonicalDataType(nsIDataType::VTYPE_EMPTY)
  {
  }

  nsOPCItemAttributes::~nsOPCItemAttributes()
  {
    /* destructor code */
  }

  // readonly attribute AString AccessPath;
  NS_IMETHODIMP nsOPCItemAttributes::GetAccessPath(nsAString& aAccessPath)
  {
    aAccessPath = AccessPath;
    return NS_OK;
  }

  // readonly attribute AString ItemID;
  NS_IMETHODIMP nsOPCItemAttributes::GetItemID(nsAString& aItemID)
  {
    aItemID = ItemID;
    return NS_OK;
  }

  // readonly attribute boolean IsActive;
  NS_IMETHODIMP nsOPCItemAttributes::GetIsActive(PRBool *aIsActive)
  {
    NS_ENSURE_ARG_POINTER(aIsActive);
    *aIsActive = IsActive;
    return NS_OK;
  }

  // readonly attribute unsigned long ClientHandle;
  NS_IMETHODIMP nsOPCItemAttributes::GetClientHandle(PRUint32 *aClientHandle)
  {
    NS_ENSURE_ARG_POINTER(aClientHandle);
    *aClientHandle = ClientHandle;
    return NS_OK;
  }

  // readonly attribute unsigned long ServeriHandle;
  NS_IMETHODIMP nsOPCItemAttributes::GetServerHandle(PRUint32 *aServerHandle)
  {
    NS_ENSURE_ARG_POINTER(aServerHandle);
    *aServerHandle = ServerHandle;
    return NS_OK;
  }

  // readonly attribute unsigned long AccessRights;
  NS_IMETHODIMP nsOPCItemAttributes::GetAccessRights(PRUint32 *aAccessRights)
  {
    NS_ENSURE_ARG_POINTER(aAccessRights);
    *aAccessRights = AccessRights;
    return NS_OK;
  }

  // readonly attribute AString RequestedDataType;
  NS_IMETHODIMP nsOPCItemAttributes::GetRequestedDataType(PRUint32 *aRequestedDataType)
  {
    NS_ENSURE_ARG_POINTER(aRequestedDataType);
    *aRequestedDataType = RequestedDataType;
    return NS_OK;
  }

  // readonly attribute AString CanonicalDataType;
  NS_IMETHODIMP nsOPCItemAttributes::GetCanonicalDataType(PRUint32 *aCanonicalDataType)
  {
    NS_ENSURE_ARG_POINTER(aCanonicalDataType);
    *aCanonicalDataType = CanonicalDataType;
    return NS_OK;
  }

  void nsOPCItemAttributes::SetAccessPath(const nsAString& aAccessPath)
  {
    AccessPath = aAccessPath;
  }

  void nsOPCItemAttributes::SetItemID(const nsAString& aItemID)
  {
    ItemID = aItemID;
  }

  void nsOPCItemAttributes::SetIsActive(PRBool aIsActive)
  {
    IsActive = aIsActive;
  }

  void nsOPCItemAttributes::SetClientHandle(PRUint32 aClientHandle)
  {
    ClientHandle = aClientHandle;
  }

  void nsOPCItemAttributes::SetServerHandle(PRUint32 aServerHandle)
  {
    ServerHandle = aServerHandle;
  }

  void nsOPCItemAttributes::SetAccessRights(PRUint32 aAccessRights)
  {
    AccessRights = aAccessRights;
  }

  void nsOPCItemAttributes::SetRequestedDataType(PRUint32 aRequestedDataType)
  {
    RequestedDataType = aRequestedDataType;
  }

  void nsOPCItemAttributes::SetCanonicalDataType(PRUint32 aCanonicalDataType)
  {
    CanonicalDataType = aCanonicalDataType;
  }
}

namespace XPCOM
{
  nsCOMPtr<nsIOPCItemAttributesEditor> CreateItemAttributes()
  {
    return nsCOMPtr<nsIOPCItemAttributesEditor>(new nsOPCItemAttributes);
  }
}


