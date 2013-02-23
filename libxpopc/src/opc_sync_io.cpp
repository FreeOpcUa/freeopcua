/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCSyncIO
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include "ns_array.h"
#include "ns_for_each.h"
#include "ns_pruint32.h"
#include "opc_item_state.h"
#include "opc_sync_io.h"

#include <boost/bind.hpp>
#include <opc_client/sync_io.h>
#include <common/addons_core/addon_manager.h>
#include <common/noncopyable.h>
#include <managers/error_manager/manager.h>
#include <nsComponentManagerUtils.h>
#include <nsISupportsPrimitives.h>
#include <mozopc.h>
//#include <nsVariant.h>

namespace
{
  /// @brief Class for acceessing to server status data
  class nsOPCSyncIO
    : public nsIOPCSyncIO
    , private Common::NonCopyable
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCSYNCIO

    explicit nsOPCSyncIO(std::unique_ptr<OpcClient::SyncIO> synIOImpl);

  private:
    const OpcClient::SyncIO& GetImpl() const;
    OpcClient::SyncIO& GetImpl();

  private:
    virtual ~nsOPCSyncIO()
    {
    }

  private:
    const std::unique_ptr<OpcClient::SyncIO> Impl;
  };


  NS_IMPL_ISUPPORTS1(nsOPCSyncIO, nsIOPCSyncIO)

  nsresult ProcessError(const Common::Error& error)
  {
    ErrorManager::Manager::SharedPtr errorManager = Common::GetAddon<ErrorManager::Manager>(Common::ADDON_ID_ERROR_MANAGER);
    errorManager->SetLastError(error);
    return NS_ERROR_FAILURE;
  }

  PRTime ToPRTime(const boost::posix_time::ptime& t)
  {
    const boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    const boost::posix_time::time_duration diff(t - epoch);
    return diff.total_microseconds();
  }

  nsCOMPtr<nsIVariant> ToNsVariant(const Common::VariantType& var)
  {
    nsCOMPtr<nsIWritableVariant> variant = do_CreateInstance(NS_VARIANT_CONTRACTID);
    switch (var.which())
    {
      case Common::CHAR_TYPE:
        variant->SetAsInt8(boost::get<char>(var));
        break;

      case Common::INT_TYPE:
        variant->SetAsInt32(boost::get<int>(var));
        break;

      case Common::UNSIGNED_INT_TYPE:
        variant->SetAsUint32(boost::get<unsigned>(var));
        break;

      case Common::FLOAT_TYPE:
        variant->SetAsFloat(boost::get<float>(var));
        break;

      case Common::DOUBLE_TYPE:
        variant->SetAsDouble(boost::get<double>(var));
        break;

      case Common::STRING_TYPE:
        variant->SetAsString(boost::get<std::string>(var).c_str());
        break;
    }
    return do_QueryInterface(variant);
  }

  nsOPCSyncIO::nsOPCSyncIO(std::unique_ptr<OpcClient::SyncIO> syncIOImpl)
    : Impl(std::move(syncIOImpl))
  {
  }


  void AddReadItemDef(std::vector<OpcClient::ReadItemDef>& elements, void* serverHandlePtr)
  {
    OpcClient::ReadItemDef itemDef;
    nsISupportsPRUint32* serverHandle = static_cast<nsISupportsPRUint32*>(serverHandlePtr);
    PRUint32 serverHandleData = 0;
    nsresult res = serverHandle->GetData(&serverHandleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetServerHandle));
    itemDef.ServerID = static_cast<Common::ObjectID>(serverHandleData);
    elements.push_back(itemDef);
  }
 
  void AddOPCItemState(std::vector<nsCOMPtr<nsISupports> >& resultItem, const OpcClient::ReadItemDef& def)
  {
    nsCOMPtr<XPCOM::nsIOPCItemStateEditor> state = XPCOM::CreateOPCItemState();
    state->SetServerHandle(static_cast<PRUint32>(def.ServerID));
//    state->SetClientHandle(); // TODO add client handle tp ReadItemDef
    state->SetTimeStamp(ToPRTime(def.Data.Time));
//    virtual void SetQuality(PRUint16 aQuality) = 0;
    state->SetDataValue(ToNsVariant(def.Data.Data));
    state->SetErrorCode(def.ResultError.GetCode());
    nsCOMPtr<nsISupports> supports = do_QueryInterface(state);
    resultItem.push_back(supports);
  }

  // nsIArray Read (in boolean fromCache, in nsIArray serverHandles);
  NS_IMETHODIMP nsOPCSyncIO::Read(PRBool fromCache, nsIArray *serverHandles, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(serverHandles);
    NS_ENSURE_ARG_POINTER(_retval);

    std::vector<OpcClient::ReadItemDef> itemDefs;
    XPCOM::ForEach(*serverHandles, NS_GET_IID(nsISupportsPRUint32), boost::bind(AddReadItemDef, boost::ref(itemDefs), _1));
    if (fromCache == PR_TRUE)
    {
      GetImpl().ReadFromCache(itemDefs);
    }
    else
    {
      GetImpl().ReadFromDevice(itemDefs);
    }
    std::vector<nsCOMPtr<nsISupports> > values;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddOPCItemState, boost::ref(values), _1));

    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(values)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }


  nsresult CreateCommonValue(nsIVariant& variant, Common::VariantType& resultValue)
  {
    PRUint16 type = nsIDataType::VTYPE_EMPTY;
    nsresult res = variant.GetDataType(&type);
    NS_ENSURE_SUCCESS(res, res);
    switch (type)
    {
      case nsIDataType::VTYPE_CHAR:
      {
        char value = 0;
        res = variant.GetAsChar(&value);
        NS_ENSURE_SUCCESS(res, res);
        resultValue = value;
        break;
      }
      case nsIDataType::VTYPE_INT32:
      {
        PRInt32 value = 0;
        res = variant.GetAsInt32(&value);
        NS_ENSURE_SUCCESS(res, res);
        resultValue = static_cast<int>(value);
        break;
      }
      case nsIDataType::VTYPE_UINT32:
      {
        PRUint32 value = 0;
        res = variant.GetAsUint32(&value);
        NS_ENSURE_SUCCESS(res, res);
        resultValue = static_cast<unsigned>(value);
        break;
      }
      case nsIDataType::VTYPE_FLOAT:
      {
        float value = 0;
        res = variant.GetAsFloat(&value);
        NS_ENSURE_SUCCESS(res, res);
        resultValue = static_cast<float>(value);
        break;
      }
      case nsIDataType::VTYPE_DOUBLE:
      {
        double value = 0;
        res = variant.GetAsDouble(&value);
        NS_ENSURE_SUCCESS(res, res);
        resultValue = value;
        break;
      }
      case nsIDataType::VTYPE_STRING_SIZE_IS:
      case nsIDataType::VTYPE_CHAR_STR:
      {
        char* value = 0;
        res = variant.GetAsString(&value);
        NS_ENSURE_SUCCESS(res, res);
        resultValue = std::string(value);
        NS_Free(value);
        break;
      }
      default:
      {
        THROW_ERROR1(VariantTypeInvalid, type); 
      }
    }
    return NS_OK;
  }

  void AddWriteItemDef(std::vector<OpcClient::WriteItemDef>& elements, void* serverHandlePtr, void* variantPtr)
  {
    nsISupportsPRUint32* serverHandle = static_cast<nsISupportsPRUint32*>(serverHandlePtr);
    PRUint32 serverHandleData = 0;
    nsresult res = serverHandle->GetData(&serverHandleData);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetServerHandle));

    nsIVariant* variant = static_cast<nsIVariant*>(variantPtr);

    OpcClient::WriteItemDef itemDef;
    itemDef.ServerID = static_cast<Common::ObjectID>(serverHandleData);
    res = CreateCommonValue(*variant, itemDef.Data.Data);
    XPCOM::NsThrowIfFailed(res, CREATE_ERROR(UnableToGetValueForWriting));
    elements.push_back(itemDef);
  }
 

  void AddWriteItemResult(std::vector<nsCOMPtr<nsISupports> >& resultItem, const OpcClient::WriteItemDef& def)
  {
    nsCOMPtr<nsISupportsPRUint32> prUint(XPCOM::CreateSupportsPRUint32(def.ResultError.GetCode()));
    nsCOMPtr<nsISupports> supports = do_QueryInterface(prUint);
    resultItem.push_back(supports);
  }

  // nsIArray Write (in nsIArray serverHandles, in nsIArray itemValues);
  NS_IMETHODIMP nsOPCSyncIO::Write(nsIArray *serverHandles, nsIArray *itemValues, nsIArray **_retval NS_OUTPARAM)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(serverHandles);
    NS_ENSURE_ARG_POINTER(_retval);

    std::vector<OpcClient::WriteItemDef> itemDefs;
    XPCOM::ForEach(*serverHandles, NS_GET_IID(nsISupportsPRUint32), *itemValues, NS_GET_IID(nsIVariant), boost::bind(AddWriteItemDef, boost::ref(itemDefs), _1, _2));
    GetImpl().Write(itemDefs);
    std::vector<nsCOMPtr<nsISupports> > values;
    std::for_each(itemDefs.begin(), itemDefs.end(), boost::bind(AddWriteItemResult, boost::ref(values), _1));

    *_retval = nsCOMPtr<nsIArray>(new XPCOM::nsStlArray(values)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }

  const OpcClient::SyncIO&  nsOPCSyncIO::GetImpl() const
  {
    return *Impl;
  }

  OpcClient::SyncIO& nsOPCSyncIO::GetImpl()
  {
    return *Impl;
  }

} // namespace

namespace XPCOM
{
  nsCOMPtr<nsIOPCSyncIO> CreateSyncIO(std::unique_ptr<OpcClient::SyncIO> syncIO)
  {
    return nsCOMPtr<nsIOPCSyncIO>(new nsOPCSyncIO(std::move(syncIO)));
  }
} // namespace XPCOM

