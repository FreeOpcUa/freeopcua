/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Items Manager interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __items_manager_h_edadf82b_7c5b_4be7_b45e_b10bee0c3935
#define __items_manager_h_edadf82b_7c5b_4be7_b45e_b10bee0c3935

#include <opccore/common/exception.h>
#include <opccore/common/noncopyable.h>
#include <opccore/common/object_id.h>
#include <opccore/common/value.h>
#include <string>
#include <vector>
#include <map>

namespace Gefest
{
  class Item;
}

namespace OpcClient
{
  /// @brief Description of the item parameters for validating
  /// it may be used for vlidating parameters of new item which user suppose to add.
  /// @sa ItemManager::Validate
  struct ValidateItemDef
  {
    /// @brief access path of the item
    std::string AccessPath;
    /// @brief Item id in form '/path/to/the/item'
    std::string ItemID;
    /// @brief raw data 
    std::vector<char> Blob;
    /// @brief Type of item data to which native data should be converted
    Common::ValueType RequestedDataType;
    /// @brief resul of validating current parameters
    Common::Error ResultError;

    ValidateItemDef()
      : RequestedDataType(Common::UNKNOWN_TYPE)
    {
    }
  };

  /// @brief Definition of new item
  /// @sa ItemManager::Add
  struct AddItemDef
  {
    /// @brief Strange thing. in the opc specification it can be an a path the file in the os from which data rerieves.
    std::string AccessPath;
    /// @brief Path to the item in the property tree
    std::string ItemID;  
    /// @brief i dont know what it is.
    std::vector<char> Blob; 
    /// @brief Type of data to which should be converted source data
    Common::ValueType RequestedDataType;
    /// @brief is item active. If active data of item will update.
    bool IsActive;
    /// @brief Id which user want to assign for the item
    unsigned ClientID;

    /// @brief server ID of the item which generates and assigns for item by server.
    Common::ObjectID ResultServerID;
    /// @brief result of adding item.
    Common::Error ResultError;
    /// @brief Access Rights to item
    unsigned ResultAccessRights;
    /// @brief Native type of data holded in item.
    Common::ValueType ResultCanonicalDataType;
    
    AddItemDef()
      : RequestedDataType(Common::UNKNOWN_TYPE)
      , IsActive(true)
      , ClientID(0)
      , ResultServerID(0)
      , ResultAccessRights(3) // TODO replace with bit mask
      , ResultCanonicalDataType(Common::UNKNOWN_TYPE)
    {
    }
  };

  /// @brief description of the item to be deleted.
  /// @sa ItemManager::Remove
  struct RemoveItemDef
  {
    /// @bried Server ID of the item to be deleted
    Common::ObjectID ServerID;
    /// @brief result of item deletion.
    Common::Error ResultError;
    
    RemoveItemDef()
      : ServerID(0)
    {
    }
  };

  /// @bried descriptor for updating Active state of the item
  /// @sa ItemManager::SetActiveState
  struct SetActiveStateItemDef
  {
    /// @brief Server ID of the item of which ActiveState is updated.
    Common::ObjectID ServerID;
    /// @brief new state of the item.
    bool IsActive;
    /// @brief result of updating item state
    Common::Error ResultError;
    
    SetActiveStateItemDef()
      : ServerID(0)
      , IsActive(true)
    {
    }
  };


  /// @brief Update new client ID of the item
  /// @sa ItemsManager::SetClientID
  struct SetClientIDsItemDef
  {
    /// @brief Server ID of the item for wich client ID is updated
    Common::ObjectID ServerID;
    /// @brief new client ID of the item.
    Common::ObjectID ClientID;
    /// @brief result of the updating itsm client ID
    Common::Error ResultError;
    
    SetClientIDsItemDef()
      : ServerID(0)
      , ClientID(0)
    {
    }
  };

  /// @brief Update type of the data to which native data should be converted
  /// @sa ItemsManager::SetDataType
  struct SetDataTypeItemDef
  {
    /// @brief Server ID of the item
    Common::ObjectID ServerID;
    /// @brief Type to which native data should be converted
    Common::ValueType RequestedDataType;
    /// @brief result of updating item data type
    Common::Error ResultError;
    
    SetDataTypeItemDef()
      : ServerID(0)
      , RequestedDataType(Common::UNKNOWN_TYPE)
    {
    }
  };


  /// @brief Item descriptor
  struct ItemAttributes
  {
    /// @brief access path for item
    std::string AccessPath;
    /// @brief path to item
    std::string ItemID;
    /// @brief state of the item.
    /// If true data for item is collected.
    bool IsActive;
    /// @brief ID of the item that generates by server
    Common::ObjectID ServerID;
    /// @brief ID of the item wich updates by client
    Common::ObjectID ClientID;
    /// @brief  access rights
    unsigned AccessRights;
    /// @brief TODO
    std::vector<char> Blob;
    /// @brief Type of data to which native data should be converted when passes to user
    Common::ValueType RequestedDataType;
    /// @brief native type of the item data
    Common::ValueType CanonicalDataType;
    // TODO add EUTYPE & EUINFO
    /// @brief item assosiated with decriptor
    std::shared_ptr<Gefest::Item> Item;
    
    ItemAttributes()
      : IsActive(true)
      , ServerID(Common::GenerateNewID())
      , ClientID(0)
      , AccessRights(3)
      , RequestedDataType(Common::UNKNOWN_TYPE)
      , CanonicalDataType(Common::UNKNOWN_TYPE)
    {
    }
  };

  class ItemsManager
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(ItemsManager);

  public:
    /// @brief add new item to the group
    /// @param itemDefs - definition of new item
    /// @throws in case of error general error not linked with adding of the concrete item.
    /// @note result error of adding for every element of the input vector stores into ResultError field.
    virtual void Add(std::vector<AddItemDef>& itemDefs) = 0;
    
    /// @brief Validate of parameters item to be addded.
    /// @param itemDefs parameters of items for validating
    /// @throws In case of error. Result of validating item will be in the field ResultError.
    virtual void Validate(std::vector<ValidateItemDef>& itemDefs) const = 0;

    /// @brief Remove items from group
    /// @param itemsDefs items to be deleted.
    /// @throws Common::Error in case of error. Result of removing will be stored in ResultError field.
    virtual void Remove(std::vector<RemoveItemDef>& itemDefs) = 0;
    
    /// @brief update IsActive state for items.
    /// @param itemDefs items for which active state changes.
    /// @throws Common::Error in case of error. Result for every item stores in the field ResultError.
    virtual void SetActiveState(std::vector<SetActiveStateItemDef>& itemDefs) = 0;
    
    /// @brief Update client item ID.
    /// @param itemDefs items to be updated 
    /// @throws Common::Error in case of error. Result for every item stores in the field ResultError.
    virtual void SetClientIDs(std::vector<SetClientIDsItemDef>& itemDefs) = 0;

    /// @brief Update requested data type of item.
    /// @param itemDefs descriptors of requested items data types
    /// @throws Common::Error in case of error. Result of updating requested data type for items.
    virtual void SetDataType(std::vector<SetDataTypeItemDef>& itemDefs) = 0;
    
    /// @brief Get items attributes
    virtual std::map<Common::ObjectID, ItemAttributes> GetItemsAttributes() const = 0;
    /// @brief Get item attributes by item ID.
    virtual OpcClient::ItemAttributes GetItemAttributes(Common::ObjectID id) const = 0;
  };

  ///@brief Create instance of the ItemsManager.
  ItemsManager::UniquePtr CreateItemsManager();

} // namespace OpcClient

#endif //__items_manager_h_edadf82b_7c5b_4be7_b45e_b10bee0c3935
