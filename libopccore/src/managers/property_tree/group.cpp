
/// @email rykovanov.as@gmail.com
/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Group realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/errors.h>
#include <opc/managers/property_tree/group.h>
#include <opc/managers/property_tree/errors.h>
#include <opc/managers/property_tree/named_object.h>
#include <opc/managers/property_tree/tree_object.h>
#include <opc/managers/property_tree/item.h>

#include <iterator>

#include <tr1/functional>

namespace
{
  inline void ThrowObjectExistButShouldnt()
  {
    THROW_ERROR(ObjectExistButShouldnt);
  }

  inline void ThrowObjectDoesntExistButShould()
  {
    THROW_ERROR(ObjectDoesntExistButShould);
  }

  inline void ThrowUnsupported()
  {
    THROW_ERROR(UnsupportedFeature);
  }
  
  class NameGetter
  {
  public:
    template<typename NamedObjectType>
    std::string operator()(const NamedObjectType& object) const
    {
      return GetName(object);
    }

  private:  
    std::string GetName(const Gefest::NamedObject& object) const
    {
      return object.GetName();
    }

    std::string GetName(Gefest::NamedObject::SharedPtr object) const
    {
      return object->GetName();
    }
    
    std::string GetName(const std::pair<std::string, Gefest::NamedObject::SharedPtr>& object) const
    {
      return object.second->GetName();
    }
  };
  
  template<typename MapType>
  std::vector<std::string> GetNames(const MapType& container)
  {
    typedef std::vector<std::string> StringVector;
    StringVector names;
    std::transform(container.begin(), container.end(), std::back_inserter<StringVector>(names), NameGetter());
    return names;
  }

  class IDGetter
  {
  public:
    template<typename NamedObjectType>
    Common::ObjectID operator()(const NamedObjectType& object) const
    {
      return GetID(object);
    }

  private:
    Common::ObjectID GetID(const Gefest::NamedObject& object) const
    {
      return object.GetID();
    }
   
    Common::ObjectID GetID(const Gefest::NamedObject::SharedPtr object) const
    {
      return object->GetID();
    }
    
    Common::ObjectID GetID(const std::pair<std::string, Gefest::NamedObject::SharedPtr>& object) const 
    {
      return object.second->GetID();
    }
  };
  
  template<typename MapType>
  std::vector<Common::ObjectID> GetIDs(const MapType& container)
  {
    std::vector<Common::ObjectID> ids(container.size(), Common::ObjectID());
    std::transform(container.begin(), container.end(), ids.begin(), IDGetter());
    return ids;
  }

  class NamedObjectIDComparer
  {
  public:
    NamedObjectIDComparer(const Common::ObjectID& param)
      : Param(param)
    {
    }

    template<typename NamedObjectType>
    bool operator()(const NamedObjectType& object) const
    {
      return Param == IDGetter()(object);
    }
    
 
  private:
    const Common::ObjectID& Param;
  };

  
  class NamedObjectNameComparer
  {
  public:
    NamedObjectNameComparer(const std::string& param)
      : Param(param)
    {
    }

    template<typename NamedObjectType>
    bool operator()(const NamedObjectType& object) const
    {
      return Param == NameGetter()(object);
    }
    
 
  private:
    const std::string& Param;
  };

  template<typename Container, typename Comparer>
  void EnsureObjectNotExist(const Container& container, const Comparer& comparer)
  {
    if (std::find_if(container.begin(), container.end(), comparer) != container.end())
    {
      ThrowObjectExistButShouldnt();
    }
  }

  template<typename Container>
  void EnsureObjectNotExist(const Common::ObjectID& id, const Container& container)
  {
    const NamedObjectIDComparer comparer(id);
    EnsureObjectNotExist(container, comparer);
  }


  template<typename Container>
  void EnsureObjectNotExist(const std::string& name, const Container& container)
  {
    const NamedObjectNameComparer comparer(name);
    EnsureObjectNotExist(container, comparer);
  }
 
  template<typename Iterator, typename Container, typename Comparer>
  Iterator EnsureObjectExistImpl(const Comparer& comparer, Container& container)
  {
    Iterator it = std::find_if(container.begin(), container.end(), comparer);
    if (it == container.end())
    {
      ThrowObjectExistButShouldnt();
    }
    return it;
  }

  template<typename Container>
  typename Container::const_iterator EnsureObjectExistConst(const Common::ObjectID& id, const Container& container)
  {
    return EnsureObjectExistImpl<typename Container::const_iterator>(NamedObjectIDComparer(id), container);
  }

  template<typename Container>
  typename Container::const_iterator EnsureObjectExistConst(const std::string& name, const Container& container)
  {
    return EnsureObjectExistImpl<typename Container::const_iterator>(NamedObjectNameComparer(name), container);
  }
  
  template<typename ParamType, typename Container>
  typename Container::iterator  EnsureObjectExist(const std::string& name, Container& container)
  {
    return EnsureObjectExistImpl<typename Container::iterator>(NamedObjectNameComparer(name), container);
  }
 
  template<typename ParamType, typename Container>
  typename Container::iterator  EnsureObjectExist(const Common::ObjectID& id, Container& container)
  {
    return EnsureObjectExistImpl<typename Container::iterator>(NamedObjectIDComparer(id), container);
  }
}

namespace Gefest
{
  void Group::EnsureItemNotExist(const std::string& name) const
  {
    EnsureObjectNotExist(name, Items);
  }
  void Group::EnsureItemNotExist(const Common::ObjectID& id) const
  {
    EnsureObjectNotExist(id, Items);
  }
  
  Group::ItemsContainer::iterator Group::EnsureItemExist(const std::string& name)
  {
    return EnsureObjectExist<std::string, Group::ItemsContainer>(name, Items); 
  }
  Group::ItemsContainer::iterator Group::EnsureItemExist(const Common::ObjectID& id)
  {
    return EnsureObjectExist<Common::ObjectID, Group::ItemsContainer>(id, Items); 
  }

  
  Group::ItemsContainer::const_iterator Group::EnsureItemExist(const std::string& name) const
  {
    return EnsureObjectExistConst(name, Items);
  }
  Group::ItemsContainer::const_iterator Group::EnsureItemExist(const Common::ObjectID& id) const
  {
    return EnsureObjectExistConst(id, Items);
  }

  void Group::EnsureGroupNotExist(const std::string& name) const
  {
    EnsureObjectNotExist(name, Groups);
  }
  void Group::EnsureGroupNotExist(const Common::ObjectID& id) const
  {
    EnsureObjectNotExist(id, Groups);
  }
  
  Group::GroupsContainer::iterator Group::EnsureGroupExist(const std::string& name)
  {
    return EnsureObjectExist<std::string, Group::GroupsContainer>(name, Groups); 
  }
  Group::GroupsContainer::iterator Group::EnsureGroupExist(const Common::ObjectID& id)
  {
    return EnsureObjectExist<Common::ObjectID, Group::GroupsContainer>(id, Groups); 
  }

  Group::GroupsContainer::const_iterator Group::EnsureGroupExist(const std::string& name) const
  {
    return EnsureObjectExistConst(name, Groups);
  }
  Group::GroupsContainer::const_iterator Group::EnsureGroupExist(const Common::ObjectID& id) const
  {
    return EnsureObjectExistConst(id, Groups);
  }


  Group::SharedPtr Group::Create()
  {
    return Group::SharedPtr(new Group);
  }

  Group::SharedPtr Group::GetSelfPtr()
  {
    return shared_from_this();
  }
  void Group::SetName(const std::string& name)
  {
    Group::SharedPtr parentGroup = std::static_pointer_cast<Group>(GetParent());
    if (parentGroup && parentGroup->SubGroupExists(name))
    {
      THROW_ERROR2(UnableToRenameParentAlreadyContainElement, GetName(), name);
    }
    NamedObject::SetName(name);
  }

  Item::SharedPtr Group::AddItem(const std::string& name)
  {
    EnsureItemNotExist(name);
    EnsureGroupNotExist(name);

    Item::SharedPtr item(Item::Create());
    item->SetParent(GetSelfPtr());
    item->SetName(name);
    Items.insert(std::make_pair(name, item));
    return item;
  }

  void Group::RemoveItem(const std::string& name)
  {
    Items.erase(EnsureItemExist(name));
  }

  void Group::RemoveItem(const Common::ObjectID& id)
  {
    Items.erase(EnsureItemExist(id));
  }
  
  bool Group::ItemExists(const std::string& name) const
  {
    return Items.find(name) != Items.end();
  }
  
  bool Group::ItemExists(const Common::ObjectID& id) const
  {
    return std::find_if(Items.begin(), Items.end(), NamedObjectEqualByID<Item>(id)) != Items.end();
  }
 
  Item::SharedPtr Group::GetItem(const std::string& name) const
  {
    return EnsureItemExist(name)->second;
  }
  Item::SharedPtr Group::GetItem(const Common::ObjectID& id) const
  {
    return EnsureItemExist(id)->second;
  }

  std::vector<std::string> Group::GetItemsNames() const
  {
    return GetNames(Items);
  }
  std::vector<Common::ObjectID> Group::GetItemsIDs() const
  {
    return GetIDs(Items);
  }
 
  Group::SharedPtr Group::AddSubGroup(const std::string& name)
  {
    EnsureItemNotExist(name);
    EnsureGroupNotExist(name);

    Group::SharedPtr group(new Group);
    group->SetName(name);
    group->SetParent(GetSelfPtr());
    Groups.insert(std::make_pair(name, group));
    return group;
  }

  void Group::RemoveSubGroup(const std::string& name)
  {
    Groups.erase(EnsureGroupExist(name));
  }
  void Group::RemoveSubGroup(const Common::ObjectID& groupID)
  {
    Groups.erase(EnsureGroupExist(groupID));
  }
 
  bool Group::SubGroupExists(const std::string& name) const
  {
    return Groups.find(name) != Groups.end();
  }
  bool Group::SubGroupExists(const Common::ObjectID& groupID) const
  {
    return std::find_if(Groups.begin(), Groups.end(), NamedObjectEqualByID<Group>(groupID)) != Groups.end();
  }
 
  Group::SharedPtr Group::GetSubGroup(const std::string& name)
  {
    return EnsureGroupExist(name)->second;
  }
  Group::SharedPtr Group::GetSubGroup(const Common::ObjectID& id)
  {
    return EnsureGroupExist(id)->second;
  }
 
  std::vector<std::string> Group::GetSubGroupsNames() const
  {
    return GetNames(Groups);
  }

  std::vector<Common::ObjectID> Group::GetSubGroupsIDs() const
  {
    return GetIDs(Groups);
  }

  std::vector<Group::SharedPtr> Group::GetSubGroups() const
  {
    std::vector<Group::SharedPtr> groups(Groups.size());
    std::transform(Groups.begin(), Groups.end(), groups.begin(),
      std::tr1::bind(&GroupsContainer::value_type::second, std::tr1::placeholders::_1));
    return groups;   
  }
  std::vector<Item::SharedPtr> Group::GetItems() const
  {
    std::vector<Item::SharedPtr> items(Items.size());
    std::transform(Items.begin(), Items.end(), items.begin(),
      std::tr1::bind(&ItemsContainer::value_type::second, std::tr1::placeholders::_1));
    return items;
  }
}

