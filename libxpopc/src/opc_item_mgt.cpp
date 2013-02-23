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

#include "opc_item_mgt.h"

#include "ns_array.h"
#include "ns_for_each.h"
#include "ns_pruint32.h"
#include "ns_simple_enumerator.h"
#include "opc_item_attributes.h"
#include "opc_item_result.h"
#include "xp_strings.h"

#include <boost/bind.hpp>
#include <common/addons_core/addon_ids.h>
#include <common/addons_core/addon_manager.h>
#include <common/exception.h>
#include <managers/error_manager/manager.h>
#include <mozopc.h>
#include <nsStringAPI.h>
#include <nsISupportsPrimitives.h>
#include <nsIVariant.h>

namespace
{
  Common::ValueType VariantTypeToCommonType(PRUint32 varType)
  {
    switch (varType)
    {
      case nsIDataType::VTYPE_CHAR:
        return Common::CHAR_TYPE;
      case nsIDataType::VTYPE_INT32:
        return Common::INT_TYPE;
      case nsIDataType::VTYPE_UINT32:
        return Common::UNSIGNED_INT_TYPE;
      case nsIDataType::VTYPE_FLOAT:
        return Common::FLOAT_TYPE;
      case nsIDataType::VTYPE_DOUBLE:
        return Common::DOUBLE_TYPE;
      case nsIDataType::VTYPE_CHAR_STR:
        return Common::STRING_TYPE;
      case nsIDataType::VTYPE_WCHAR_STR:
        return Common::WSTRING_TYPE;
      default:
        return Common::UNKNOWN_TYPE;
    }
  }

  PRUint32 CommonTypeToVariantType(Common::ValueType commonType)
  {
    switch (commonType)
    {
      case Common::CHAR_TYPE:
        return nsIDataType::VTYPE_CHAR;
      case Common::INT_TYPE:
        return nsIDataType::VTYPE_INT32;
      case Common::UNSIGNED_INT_TYPE:
        return nsIDataType::VTYPE_UINT32;
      case Common::FLOAT_TYPE:
        return nsIDataType::VTYPE_FLOAT;
      case Common::DOUBLE_TYPE:
        return nsIDataType::VTYPE_DOUBLE;
      case Common::STRING_TYPE:
        return nsIDataType::VTYPE_CHAR_STR;
      case Common::WSTRING_TYPE:
        return nsIDataType::VTYPE_WCHAR_STR;
      default:
        return nsIDataType::VTYPE_EMPTY;
    }
  }


  class nsOPCItemMgt : public nsIOPCItemMgt
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCITEMMGT

    explicit nsOPCItemMgt(std::shared_ptr<OpcClient::Group> group)
      : Group(group)
    {
    }

  private:
    virtual ~nsOPCItemMgt()
    {
    }

  private:
    std::shared_ptr<OpcClient::Group> Group;
  };

  NS_IMPL_ISUPPORTS1(nsOPCItemMgt, nsIOPCItemMgt)

  nsresult ProcessError(const Common::Error& error)
  {
    ErrorManager::Manager::SharedPtr errorManager = Common::GetAddon<ErrorManager::Manager>(Common::ADDON_ID_ERROR_MANAGER);
    errorManager->SetLastError(error);
    return NS_ERROR_FAILURE;
  }

  void AddItemDef(std::vector<OpcClient::AddItemDef>& elements, void* itemPtr)
  {
    nsIOPCItemDef* itemDef = static_cast<nsIOPCItemDef*>(itemPtr);

    std::string accessPath;
    {
      PRUnichar* tmpAccessPath = nsnull;
      nsresult res = itemDef->GetAccessPath(&tmpAccessPath);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetAccessPath));
      accessPath = XPCOM::ToStdString(tmpAccessPath);
      NS_Free(tmpAccessPath);
    }

    std::string itemID;
    {
      PRUnichar* tmpItemID = nsnull;
      nsresult res = itemDef->GetItemID(&tmpItemID);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetItemID));
      itemID = XPCOM::ToStdString(tmpItemID);
      NS_Free(tmpItemID);
    }

    PRBool isActive = PR_FALSE;
    {
      nsresult res = itemDef->GetIsActive(&isActive);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetIsActive));
    }

    PRUint32 clientHandle = 0;
    {
      nsresult res = itemDef->GetClientHandle(&clientHandle);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetClientHandle));
    }
    
    OpcClient::AddItemDef def;
    def.AccessPath = accessPath;
    def.ItemID =itemID;
    def.IsActive = (isActive == PR_TRUE);
    def.ClientID = static_cast<Common::ObjectID>(clientHandle);
    elements.push_back(def);
  }

  void AddValidateItemDef(std::vector<OpcClient::ValidateItemDef>& elements, void* itemPtr)
  {
    nsIOPCItemDef* itemDef = static_cast<nsIOPCItemDef*>(itemPtr);

    std::string accessPath;
    {
      PRUnichar* tmpAccessPath = nsnull;
      nsresult res = itemDef->GetAccessPath(&tmpAccessPath);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetAccessPath));
      accessPath = XPCOM::ToStdString(tmpAccessPath);
      NS_Free(tmpAccessPath);
    }

    std::string itemID;
    {
      PRUnichar* tmpItemID = nsnull;
      nsresult res = itemDef->GetItemID(&tmpItemID);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetItemID));
      itemID = XPCOM::ToStdString(tmpItemID);
      NS_Free(tmpItemID);
    }

    PRUint32 dataType = nsIDataType::VTYPE_EMPTY;
    {
      nsresult res = itemDef->GetRequestedDataType(&dataType);
      XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetRequestedDataType));
    }

    OpcClient::ValidateItemDef def;
    def.AccessPath = accessPath;
    def.ItemID = itemID;
    def.RequestedDataType = VariantTypeToCommonType(dataType); 
    elements.push_back(def);
  }
  
  void AddItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItems, const OpcClient::AddItemDef& itemDef)
  {
    const PRUint32 accessRights = 3; // TODO will be implemented. By default full access.
    const PRUint16 dataType = CommonTypeToVariantType(Common::UNKNOWN_TYPE);
    const PRUint32 errorCode = itemDef.ResultError.GetCode();
    nsCOMPtr<nsIOPCItemResult> itemResult = XPCOM::CreateItemResult(itemDef.ResultServerID, accessRights, dataType, errorCode);
    nsCOMPtr<nsISupports> supports = do_QueryInterface(itemResult);
    resultItems.push_back(supports);
  }

  // nsIArray addItems (in nsIArray itemDefArray)
  NS_IMETHODIMP nsOPCItemMgt::AddItems(nsIArray *itemDefArray, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(itemDefArray);
    NS_ENSURE_ARG_POINTER(_retval);

    std::vector<OpcClient::AddItemDef> itemDefs;
    XPCOM::ForEach(*itemDefArray, NS_GET_IID(nsIOPCItemDef), boost::bind(AddItemDef, boost::ref(itemDefs), _1));
    Group->Add(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > itemResult;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddItemResult, boost::ref(itemResult), _1));
    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(itemResult)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }


  void AddValidateItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItems, const OpcClient::ValidateItemDef& itemDef)
  {
    const PRUint32 errorCode = itemDef.ResultError.GetCode();
    nsCOMPtr<nsIOPCItemResult> itemResult = XPCOM::CreateItemResult(0, 0, Common::UNKNOWN_TYPE, errorCode);
    nsCOMPtr<nsISupports> supports = do_QueryInterface(itemResult);
    resultItems.push_back(supports);
  }

  // nsIArray validateItems (in nsIArray itemDefArray);
  NS_IMETHODIMP nsOPCItemMgt::ValidateItems(nsIArray *itemDefArray, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(itemDefArray);
    NS_ENSURE_ARG_POINTER(_retval);

    std::vector<OpcClient::ValidateItemDef> itemDefs;
    XPCOM::ForEach(*itemDefArray, NS_GET_IID(nsIOPCItemDef), boost::bind(AddValidateItemDef, boost::ref(itemDefs), _1));
    Group->Validate(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > itemResult;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddValidateItemResult, boost::ref(itemResult), _1));
    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(itemResult)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }

  void AddRemoveItemDef(std::vector<OpcClient::RemoveItemDef>& elements, void* serverHandlePtr)
  {
    nsISupportsPRUint32* serverHandle = static_cast<nsISupportsPRUint32*>(serverHandlePtr);

    PRUint32 serverHandleData = 0;
    nsresult res = serverHandle->GetData(&serverHandleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetServerHandle));

    OpcClient::RemoveItemDef def;
    def.ServerID = serverHandleData;
    elements.push_back(def);
  }

  void AddRemoveItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItems, const OpcClient::RemoveItemDef& itemDef)
  {
    const PRUint32 errorCode = itemDef.ResultError.GetCode();
    nsCOMPtr<nsISupportsPRUint32> itemResult = XPCOM::CreateSupportsPRUint32(errorCode);
    nsCOMPtr<nsISupports> supports = do_QueryInterface(itemResult);
    resultItems.push_back(supports);
  }

  // nsIArray removeItems (in nsIArray serverHandles);
  NS_IMETHODIMP nsOPCItemMgt::RemoveItems(nsIArray *serverHandles, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(serverHandles);
    NS_ENSURE_ARG_POINTER(_retval);

    std::vector<OpcClient::RemoveItemDef> itemDefs;
    XPCOM::ForEach(*serverHandles, NS_GET_IID(nsISupportsPRUint32), boost::bind(AddRemoveItemDef, boost::ref(itemDefs), _1));
    Group->Remove(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > itemResult;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddRemoveItemResult, boost::ref(itemResult), _1));
    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(itemResult)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }
 
 
  void AddActiveStateItemDef(std::vector<OpcClient::SetActiveStateItemDef>& elements, bool isActive, void* serverHandle)
  {
    OpcClient::SetActiveStateItemDef itemDef;
    itemDef.IsActive = isActive;
    nsISupportsPRUint32* prUint32 = static_cast<nsISupportsPRUint32*>(serverHandle);
    PRUint32 handleData = 0;
    nsresult res = prUint32->GetData(&handleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetServerHandle));
    itemDef.ServerID = static_cast<Common::ObjectID>(handleData);
    elements.push_back(itemDef);
  }

  void AddActiveStateItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItem, const OpcClient::SetActiveStateItemDef& def)
  {
    nsCOMPtr<nsISupportsPRUint32> prUint(XPCOM::CreateSupportsPRUint32(def.ResultError.GetCode()));
    nsCOMPtr<nsISupports> supports = do_QueryInterface(prUint);
    resultItem.push_back(supports);
  }
  
  // nsIArray setActiveState (in nsIArray serverHandles, in boolean isActive)
  NS_IMETHODIMP nsOPCItemMgt::SetActiveState(nsIArray *serverHandles, PRBool isActive, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(serverHandles);
    NS_ENSURE_ARG_POINTER(_retval);
    std::vector<OpcClient::SetActiveStateItemDef> itemDefs;
    XPCOM::ForEach(*serverHandles, NS_GET_IID(nsISupportsPRUint32), boost::bind(AddActiveStateItemDef, boost::ref(itemDefs), isActive == PR_TRUE ,_1));
    Group->SetActiveState(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > itemResult;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddActiveStateItemResult, boost::ref(itemResult), _1));
    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(itemResult)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }
 

  void AddClientIDsItemDef(std::vector<OpcClient::SetClientIDsItemDef>& elements, void* serverHandlePtr, void* clientHandlePtr)
  {
    OpcClient::SetClientIDsItemDef itemDef;
    nsISupportsPRUint32* serverHandle = static_cast<nsISupportsPRUint32*>(serverHandlePtr);
    PRUint32 serverHandleData = 0;
    nsresult res = serverHandle->GetData(&serverHandleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetServerHandle));
    itemDef.ServerID = static_cast<Common::ObjectID>(serverHandleData);

    nsISupportsPRUint32* clientHandle = static_cast<nsISupportsPRUint32*>(clientHandlePtr);
    PRUint32 clientHandleData = 0;
    res = clientHandle->GetData(&clientHandleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetClientHandle));
    itemDef.ClientID = static_cast<Common::ObjectID>(clientHandleData);
 
    elements.push_back(itemDef);
  }
 
  void AddClientIDsItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItem, const OpcClient::SetClientIDsItemDef& def)
  {
    nsCOMPtr<nsISupportsPRUint32> prUint(XPCOM::CreateSupportsPRUint32(def.ResultError.GetCode()));
    nsCOMPtr<nsISupports> supports = do_QueryInterface(prUint);
    resultItem.push_back(supports);
  }
 
  // nsIArray setClientHandles (in nsIArray serverHandles, in nsIArray clientHandles);
  NS_IMETHODIMP nsOPCItemMgt::SetClientHandles(nsIArray *serverHandles, nsIArray *clientHandles, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(serverHandles);
    NS_ENSURE_ARG_POINTER(clientHandles);
    NS_ENSURE_ARG_POINTER(_retval);
    std::vector<OpcClient::SetClientIDsItemDef> itemDefs;
    XPCOM::ForEach(*serverHandles, NS_GET_IID(nsISupportsPRUint32), *clientHandles, NS_GET_IID(nsISupportsPRUint32), boost::bind(AddClientIDsItemDef, boost::ref(itemDefs), _1, _2));
    Group->SetClientIDs(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > itemResult;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddClientIDsItemResult, boost::ref(itemResult), _1));
    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(itemResult)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }


  void AddDataTypeItemDef(std::vector<OpcClient::SetDataTypeItemDef>& elements, void* serverHandlePtr, void* dataTypePtr)
  {
    OpcClient::SetDataTypeItemDef itemDef;
    nsISupportsPRUint32* serverHandle = static_cast<nsISupportsPRUint32*>(serverHandlePtr);
    PRUint32 serverHandleData = 0;
    nsresult res = serverHandle->GetData(&serverHandleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetServerHandle));
    itemDef.ServerID = static_cast<Common::ObjectID>(serverHandleData);

    nsISupportsPRUint32* dataType = static_cast<nsISupportsPRUint32*>(dataTypePtr);
    PRUint32 typeData = 0;
    res = dataType->GetData(&typeData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetRequestedDataType));
    itemDef.RequestedDataType = VariantTypeToCommonType(typeData);
 
    elements.push_back(itemDef);
  }
 
  void AddDataTypeItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItem, const OpcClient::SetDataTypeItemDef& def)
  {
    nsCOMPtr<nsISupportsPRUint32> prUint(XPCOM::CreateSupportsPRUint32(def.ResultError.GetCode()));
    nsCOMPtr<nsISupports> supports = do_QueryInterface(prUint);
    resultItem.push_back(supports);
  }
 
  // nsIArray setDatatypes (in nsIArray serverHandles, in nsIArray requestedDatatypes);
  NS_IMETHODIMP nsOPCItemMgt::SetDataTypes(nsIArray *serverHandles, nsIArray *requestedDataTypes, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(serverHandles);
    NS_ENSURE_ARG_POINTER(requestedDataTypes);
    NS_ENSURE_ARG_POINTER(_retval);

    std::vector<OpcClient::SetDataTypeItemDef> itemDefs;
    XPCOM::ForEach(*serverHandles, NS_GET_IID(nsISupportsPRUint32), *requestedDataTypes, NS_GET_IID(nsISupportsPRUint32), boost::bind(AddDataTypeItemDef, boost::ref(itemDefs), _1, _2));
    Group->SetDataType(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > itemResult;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddDataTypeItemResult, boost::ref(itemResult), _1));
    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(itemResult)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }


  nsCOMPtr<nsISupports> CreateOPCItemAttributes(const std::pair<Common::ObjectID, OpcClient::ItemAttributes>& itemAttributes)
  {
    nsCOMPtr<XPCOM::nsIOPCItemAttributesEditor> attributes = XPCOM::CreateItemAttributes();
    attributes->SetServerHandle(static_cast<PRUint32>(itemAttributes.first));
    attributes->SetClientHandle(static_cast<PRUint32>(itemAttributes.second.ClientID));
    attributes->SetIsActive(itemAttributes.second.IsActive ? PR_TRUE : PR_FALSE);
    attributes->SetItemID(XPCOM::ToNsString(itemAttributes.second.ItemID));
    attributes->SetAccessPath(XPCOM::ToNsString(itemAttributes.second.AccessPath));
    attributes->SetRequestedDataType(CommonTypeToVariantType(itemAttributes.second.RequestedDataType));
    attributes->SetCanonicalDataType(CommonTypeToVariantType(itemAttributes.second.CanonicalDataType));
    attributes->SetAccessRights(itemAttributes.second.AccessRights);
    nsCOMPtr<nsISupports> supports = do_QueryInterface(attributes);
    return supports;
  }

  // nsISimpleEnumerator createEnumerator ();
  NS_IMETHODIMP nsOPCItemMgt::CreateEnumerator(nsISimpleEnumerator **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    const std::map<Common::ObjectID, OpcClient::ItemAttributes>& itemAttributes = Group->GetItemsAttributes();
    std::vector<nsCOMPtr<nsISupports> > attributes(itemAttributes.size());
    std::transform(
      itemAttributes.begin(), 
      itemAttributes.end(),
      attributes.begin(),
      boost::bind(CreateOPCItemAttributes, _1));
    *_retval = XPCOM::CreateSimpleEnumerator(attributes).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }
}


namespace XPCOM
{
  nsCOMPtr<nsIOPCItemMgt> CreateItemMgt(std::shared_ptr<OpcClient::Group> group)
  {
    return nsCOMPtr<nsIOPCItemMgt>(new nsOPCItemMgt(group));
  }
}

