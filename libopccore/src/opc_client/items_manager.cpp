/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief OpcClient errors dfinitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/opc_client/items_manager.h>

#include <opccore/opc_client/errors.h>

#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/errors.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/property_tree.h>

#include <tr1/functional>

namespace
{
  std::shared_ptr<Gefest::Item> GetItem(const Gefest::PropertyTree& propertyTree, const std::string& itemPath)
  {
    Gefest::Item::SharedPtr item = propertyTree.FindItem(itemPath);
    if (!item)
    {
      THROW_ERROR1(ItemPathNotFound, itemPath);
    }
    return item;
  }

  void CheckAccessPath(const std::string& accessPath)
  {
    if (!accessPath.empty())
    {
      THROW_ERROR1(InvalidAccessPath, accessPath);
    }
  }

  void CheckBlob(const std::vector<char>& blob)
  {
    if (!blob.empty())
    {
      THROW_ERROR(BlobsUnsupported);
    }
  }

  void CheckValueType(Common::ValueType type)
  {
    switch(type)
    {
      case Common::CHAR_TYPE:
      case Common::INT_TYPE:
      case Common::UNSIGNED_INT_TYPE:
      case Common::FLOAT_TYPE:
      case Common::DOUBLE_TYPE:
      case Common::STRING_TYPE:
      case Common::WSTRING_TYPE:
      case Common::UNKNOWN_TYPE:
        return;
      default:
        break;
    }
    THROW_ERROR1(InvalidValueType, type);
  }

  class ItemsManagerImpl : public OpcClient::ItemsManager
  {
  public:
    virtual void Add(std::vector<OpcClient::AddItemDef>& itemDefs)
    {
      std::shared_ptr<PropertyTree::Manager> propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE);
      std::shared_ptr<Gefest::PropertyTree> propertyTree = propertyTreeManager->GetPropertyTree();
      for (std::vector<OpcClient::AddItemDef>::iterator itemDefIt = itemDefs.begin(); itemDefIt != itemDefs.end(); ++itemDefIt)
      {
        try
        {
          CheckAccessPath(itemDefIt->AccessPath);
          CheckValueType(itemDefIt->RequestedDataType);
          CheckBlob(itemDefIt->Blob);
          std::shared_ptr<Gefest::Item> item = GetItem(*propertyTree, itemDefIt->ItemID);

          OpcClient::ItemAttributes attr;
          attr.Item = item;
          attr.AccessPath = itemDefIt->AccessPath;
          attr.ItemID = itemDefIt->ItemID;
          attr.IsActive = itemDefIt->IsActive;
          attr.ServerID = item->GetID();
          attr.ClientID = itemDefIt->ClientID;
          attr.AccessRights = 3; // TODO replace with constants bit mask
          attr.RequestedDataType = itemDefIt->RequestedDataType;
          attr.CanonicalDataType = Common::GetValueType(item->GetValue().Data);
          Items[attr.ServerID] = attr;

          itemDefIt->ResultServerID = attr.ServerID;
          itemDefIt->ResultAccessRights = attr.AccessRights;
          itemDefIt->ResultCanonicalDataType = attr.CanonicalDataType;
        }
        catch(const Common::Error& error)
        {
          itemDefIt->ResultError = error;
        }
      }
    }

    virtual void Validate(std::vector<OpcClient::ValidateItemDef>& itemDefs) const
    {
      std::shared_ptr<PropertyTree::Manager> propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE);
      std::shared_ptr<Gefest::PropertyTree> propertyTree = propertyTreeManager->GetPropertyTree();
      for (std::vector<OpcClient::ValidateItemDef>::iterator itemDefIt = itemDefs.begin(); itemDefIt != itemDefs.end(); ++itemDefIt)
      {
        try
        {
          CheckAccessPath(itemDefIt->AccessPath);
          CheckValueType(itemDefIt->RequestedDataType);
          GetItem(*propertyTree, itemDefIt->ItemID);
          CheckBlob(itemDefIt->Blob);
        }
        catch(const Common::Error& error)
        {
          itemDefIt->ResultError = error;
        }
      }
    }

    virtual void Remove(std::vector<OpcClient::RemoveItemDef>& itemDefs)
    {
      for (std::vector<OpcClient::RemoveItemDef>::iterator itemDefIt = itemDefs.begin(); itemDefIt != itemDefs.end(); ++itemDefIt)
      {
        std::map<Common::ObjectID, OpcClient::ItemAttributes>::iterator it = FindItem(itemDefIt->ServerID);
        if (it == Items.end())
        {
          itemDefIt->ResultError = CREATE_ERROR1(InvalidServerID, itemDefIt->ServerID);
          continue;
        }
        Items.erase(it);
      }
    }

    virtual void SetActiveState(std::vector<OpcClient::SetActiveStateItemDef>& itemDefs)
    {
      for (std::vector<OpcClient::SetActiveStateItemDef>::iterator itemDefIt = itemDefs.begin(); itemDefIt != itemDefs.end(); ++itemDefIt)
      {
        std::map<Common::ObjectID, OpcClient::ItemAttributes>::iterator it = FindItem(itemDefIt->ServerID);
        if (it == Items.end())
        {
          itemDefIt->ResultError = CREATE_ERROR1(InvalidServerID, itemDefIt->ServerID);
          continue;
        }
        it->second.IsActive = itemDefIt->IsActive;
      }
    }

    virtual void SetClientIDs(std::vector<OpcClient::SetClientIDsItemDef>& itemDefs)
    {
      for (std::vector<OpcClient::SetClientIDsItemDef>::iterator itemDefIt = itemDefs.begin(); itemDefIt != itemDefs.end(); ++itemDefIt)
      {
        std::map<Common::ObjectID, OpcClient::ItemAttributes>::iterator it = FindItem(itemDefIt->ServerID);
        if (it == Items.end())
        {
          itemDefIt->ResultError = CREATE_ERROR1(InvalidServerID, itemDefIt->ServerID);
          continue;
        }
        it->second.ClientID = itemDefIt->ClientID;
      }
    }

    virtual void SetDataType(std::vector<OpcClient::SetDataTypeItemDef>& itemDefs)
    {
      for (std::vector<OpcClient::SetDataTypeItemDef>::iterator itemDefIt = itemDefs.begin(); itemDefIt != itemDefs.end(); ++itemDefIt)
      {
        std::map<Common::ObjectID, OpcClient::ItemAttributes>::iterator it = FindItem(itemDefIt->ServerID);
        if (it == Items.end())
        {
          itemDefIt->ResultError = CREATE_ERROR1(InvalidServerID, itemDefIt->ServerID);
          continue;
        }
        it->second.RequestedDataType = itemDefIt->RequestedDataType;
      }
    }

    virtual std::map<Common::ObjectID, OpcClient::ItemAttributes> GetItemsAttributes() const
    {
      return Items;
    }

    virtual OpcClient::ItemAttributes GetItemAttributes(Common::ObjectID id) const
    {
      std::map<Common::ObjectID, OpcClient::ItemAttributes>::const_iterator it = FindItem(id);
      if (it == Items.end())
      {
        THROW_ERROR1(InvalidServerID, id);
      }
      return it->second;
    }

  private:
    std::map<Common::ObjectID, OpcClient::ItemAttributes>::iterator FindItem(Common::ObjectID id)
    {
      return Items.find(id);
    }

    std::map<Common::ObjectID, OpcClient::ItemAttributes>::const_iterator FindItem(Common::ObjectID id) const
    {
       return Items.find(id);
    }

  private:
    std::map<Common::ObjectID, OpcClient::ItemAttributes> Items;
  };
}


std::unique_ptr<OpcClient::ItemsManager> OpcClient::CreateItemsManager()
{
  return std::unique_ptr<ItemsManager>(new ItemsManagerImpl);
}
