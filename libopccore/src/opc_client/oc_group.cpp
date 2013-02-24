/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Server class
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/opc_client/group.h>
#include <opccore/opc_client/items_manager.h>
#include <opccore/managers/io_manager/manager.h>

namespace
{
  class GroupPropertiesImpl : public OpcClient::GroupProperties 
  {
  public:
    GroupPropertiesImpl()
      : Active(false)
      , UpdateRate(0)
      , Bias(0)
      , Deadband(0)
    {
    }

    virtual bool IsActive() const
    {
      return Active;
    }
    virtual void SetIsActive(bool isActive)
    {
      Active = isActive;
    }
    
    virtual unsigned GetUpdateRate() const
    {
      return UpdateRate;
    }
    virtual void SetUpdateRate(unsigned rate)
    {
      UpdateRate = rate;
    }

    virtual unsigned GetTimeBias() const
    {
      return Bias;
    }
    virtual void SetTimeBias(unsigned bias)
    {
      Bias = bias;
    }

    virtual float GetDeadband() const
    {
      return Deadband;
    }
    virtual void SetDeadband(float deadband)
    {
      Deadband = deadband;
    }

  private:
    bool Active;
    unsigned UpdateRate;
    unsigned Bias;
    float Deadband;
  };


  class GroupImpl : public OpcClient::Group
  {
  public:
    GroupImpl()
      : IsPrivate(false)
      , ItemsManager(OpcClient::CreateItemsManager())
      , Properties(new GroupPropertiesImpl)
    {
    }

    virtual std::string GetName() const
    {
      return Name;
    }
    virtual void SetName(const std::string& newName)
    {
      Name = newName;
    }

    virtual void SetIsPrivate(bool value)
    {
      IsPrivate = value;
    }

    virtual bool GetIsPrivate() const
    {
      return IsPrivate;
    }

    virtual void Add(std::vector<OpcClient::AddItemDef>& itemDefs)
    {
      ItemsManager->Add(itemDefs);
    }

    virtual void Validate(std::vector<OpcClient::ValidateItemDef>& itemDefs) const
    {
      ItemsManager->Validate(itemDefs);
    }

    virtual void Remove(std::vector<OpcClient::RemoveItemDef>& itemDefs)
    {
      ItemsManager->Remove(itemDefs);
    }

    virtual void SetActiveState(std::vector<OpcClient::SetActiveStateItemDef>& itemDefs)
    {
      ItemsManager->SetActiveState(itemDefs);
    }

    virtual void SetClientIDs(std::vector<OpcClient::SetClientIDsItemDef>& itemDefs)
    {
      ItemsManager->SetClientIDs(itemDefs);
    }

    virtual void SetDataType(std::vector<OpcClient::SetDataTypeItemDef>& itemDefs)
    {
      ItemsManager->SetDataType(itemDefs);
    }

    virtual std::map<Common::ObjectID, OpcClient::ItemAttributes> GetItemsAttributes() const
    {
      return ItemsManager->GetItemsAttributes();
    }

    virtual OpcClient::ItemAttributes GetItemAttributes(Common::ObjectID id) const
    {
      return ItemsManager->GetItemAttributes(id);
    }

    virtual OpcClient::GroupProperties::SharedPtr GetProperties() const
    {
      return std::static_pointer_cast<OpcClient::GroupProperties>(Properties);
    }
 
  private:
    std::string Name;
    bool IsPrivate;
    const std::unique_ptr<OpcClient::ItemsManager> ItemsManager;
    OpcClient::GroupProperties::SharedPtr Properties;
  };
}

OpcClient::Group::UniquePtr OpcClient::CreateGroup()
{
  return OpcClient::Group::UniquePtr(new GroupImpl());
}
