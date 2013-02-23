/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCGroupInterfacesFactory
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "opc_group_interfaces_factory.h"

#include "xp_strings.h"

#include <iostream>
#include <nsStringAPI.h>
#include <string>
#include <mozopc.h>


namespace
{
  class nsOPCItemDef : public nsIOPCItemDef
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCITEMDEF

    nsOPCItemDef();

  private:
    std::string AccessPath;
    std::string ItemID;
    PRBool IsActive;
    PRUint32 ClientID;
    PRUint32 DataType;
  };

  NS_IMPL_ISUPPORTS1(nsOPCItemDef, nsIOPCItemDef)

  inline nsOPCItemDef::nsOPCItemDef()
    : IsActive(PR_TRUE)
    , ClientID(0)
  {
  }

  // attribute AString szAccessPath
  NS_IMETHODIMP nsOPCItemDef::GetAccessPath(PRUnichar** accessPath)
  {
    if (accessPath == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }
    *accessPath = XPCOM::ToPRUnichar(AccessPath);
    return NS_OK;
  }
  NS_IMETHODIMP nsOPCItemDef::SetAccessPath(const PRUnichar* accessPath)
  {
    if (accessPath == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }
    AccessPath = XPCOM::ToStdString(accessPath);
    return NS_OK;
  }

  // attribute AString szItemID
  NS_IMETHODIMP nsOPCItemDef::GetItemID(PRUnichar** itemID)
  {
    if (itemID == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }

    *itemID = XPCOM::ToPRUnichar(ItemID);
    return NS_OK;
  }
  NS_IMETHODIMP nsOPCItemDef::SetItemID(const PRUnichar* itemID)
  {
    if (itemID == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }

    ItemID = XPCOM::ToStdString(itemID);
    std::cout << "nsOPCItemDef::SetItemID = " << ItemID << std::endl;
    return NS_OK;
  }

  // attribute boolean bActive
  NS_IMETHODIMP nsOPCItemDef::GetIsActive(PRBool *aBActive)
  {
    if (aBActive == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }

    *aBActive = IsActive;
    return NS_OK;
  }
  NS_IMETHODIMP nsOPCItemDef::SetIsActive(PRBool aBActive)
  {
    IsActive = aBActive;
    return NS_OK;
  }

  // attribute unsigned long hClient
  NS_IMETHODIMP nsOPCItemDef::GetClientHandle(PRUint32 *clientID)
  {
    if (clientID == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }

    *clientID = ClientID;
    return NS_OK;
  }
  NS_IMETHODIMP nsOPCItemDef::SetClientHandle(PRUint32 clientID)
  {
    ClientID = clientID;
    return NS_OK;
  }

  // attribute unsigned long RequestedDataType
  NS_IMETHODIMP nsOPCItemDef::GetRequestedDataType(PRUint32 *aRequestedDataType)
  {
    if (aRequestedDataType == NULL)
    {
      return NS_ERROR_INVALID_POINTER;
    }

    *aRequestedDataType = DataType;
    return NS_OK;
  }
  NS_IMETHODIMP nsOPCItemDef::SetRequestedDataType(PRUint32 aRequestedDataType)
  {
    DataType = aRequestedDataType;
    return NS_OK;
  }


  class nsOPCGroupInterfacesFactory : public nsIOPCGroupInterfacesFactory
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCGROUPINTERFACESFACTORY
  };

  NS_IMPL_ISUPPORTS1(nsOPCGroupInterfacesFactory, nsIOPCGroupInterfacesFactory)

  // nsIOPCItemDef CreateItemDef ()
  NS_IMETHODIMP nsOPCGroupInterfacesFactory::CreateItemDef(nsIOPCItemDef **_retval NS_OUTPARAM)
  {
    NS_ENSURE_ARG_POINTER(_retval);
    *_retval = nsCOMPtr<nsIOPCItemDef>(new nsOPCItemDef).forget().get();
    return NS_OK;
  }

  // nsIOPCItemVQT CreateItemVQT ()
  NS_IMETHODIMP nsOPCGroupInterfacesFactory::CreateItemVQT(nsIOPCItemVQT **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }
}

namespace XPCOM
{
  nsCOMPtr<nsIOPCGroupInterfacesFactory> CreateGroupInterfacesFactory()
  {
    return nsCOMPtr<nsIOPCGroupInterfacesFactory>(new nsOPCGroupInterfacesFactory);
  }
}

