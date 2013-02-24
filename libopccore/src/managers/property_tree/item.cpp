/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Item realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/managers/property_tree/item.h>

#include <opccore/managers/property_tree/errors.h>
#include <opccore/managers/property_tree/group.h>

namespace
{
  using namespace Gefest;
  
  class PropertyEqualByID
  {
  public:
    PropertyEqualByID(const Common::ObjectID& id)
      : ID(id)
    {
    }
    bool operator()(const Item::PropertiesContainer::value_type& property) const
    {
      return IsEqual(property.second);
    }

  private:
    bool IsEqual(const Property& property) const
    {
      return property.ID == ID;
    }

  private:
    const Common::ObjectID& ID;
  };

  inline void ThrowPropertyDoesntExist(const std::string& name)
  {
    THROW_ERROR1(PropertyDoesntExist, name);
  }

  inline void ThrowPropertyDoesntExist(const Common::ObjectID& id)
  {
    THROW_ERROR1(PropertyDoesntExist, id);
  }

  inline void ThrowPropertyExist(const std::string& name)
  {
    THROW_ERROR1(PropertyAlreadyExist, name);
  }

  inline void ThrowPropertyExist(const Common::ObjectID& id)
  {
    THROW_ERROR1(PropertyAlreadyExist, id);
  }

  class GetPropertyName
  {
  public:
    GetPropertyName(std::vector<std::string>& names)
      : Names(names)
    {
    }
 
    void operator()(const Item::PropertiesContainer::value_type& val)
    {
      Names.push_back(val.second.Name);
    }
  
  private:
    std::vector<std::string>& Names;
  };

  class GetPropertyID
  {
  public:
    GetPropertyID(std::vector<Common::ObjectID>& ids)
      : IDs(ids)
    {
    }
 
    void operator()(const Item::PropertiesContainer::value_type& val)
    {
      IDs.push_back(val.second.ID);
    }
  
  private:
    std::vector<Common::ObjectID>& IDs;
  };

} 


namespace Gefest
{
  Item::SharedPtr Item::Create()
  {
    return Item::SharedPtr(new Item);
  }

  Item::SharedPtr Item::GetSelfPtr()
  {
    return shared_from_this();
  }


  void Item::SetName(const std::string& name)
  {
    Group::SharedPtr parentGroup = std::static_pointer_cast<Group>(GetParent());
    if (parentGroup && parentGroup->SubGroupExists(name))
    {
      THROW_ERROR2(UnableToRenameParentAlreadyContainElement, GetName(), name);
    }
    NamedObject::SetName(name);
  }

  Common::Value Item::GetValue() const
  {
    return Value;
  }
  void Item::SetValue(const Common::Value& val)
  {
    Value = val;
  }

  void Item::AddProperty(const Property& property)
  {
    EnsurePropertyNotExist(property.Name);
    Properties[property.Name] = property;
  }

  void Item::RemoveProperty(const std::string& name)
  {
    Properties.erase(EnsurePropertyExist(name));
  }

  void Item::RemoveProperty(const Common::ObjectID& id)
  {
    Properties.erase(EnsurePropertyExist(id));
  }

  Property Item::GetProperty(const std::string& name) const
  {
   return EnsurePropertyExist(name)->second;
  }

  Property Item::GetProperty(const Common::ObjectID& id) const
  {
    return EnsurePropertyExist(id)->second;
  }

  void Item::SetPropertyValue(const std::string& name, const Common::Value& value)
  {
    EnsurePropertyExist(name)->second.Value = value;
  }

  void Item::SetPropertyValue(const Common::ObjectID& id, const Common::Value& value)
  {
    EnsurePropertyExist(id)->second.Value = value;
  }

  std::vector<std::string> Item::GetPropertiesNames() const
  {
    std::vector<std::string> names;
    std::for_each(Properties.begin(), Properties.end(), GetPropertyName(names));
    return names;
  }
  
  std::vector<Common::ObjectID> Item::GetPropertiesIDs() const
  {
    std::vector<Common::ObjectID> ids;
    std::for_each(Properties.begin(), Properties.end(), GetPropertyID(ids));
    return ids;
  }

  Item::PropertiesContainer::iterator Item::EnsurePropertyExist(const std::string& name)
  {
    PropertiesContainer::iterator it = Properties.find(name);
    if (it == Properties.end())
    {
      ThrowPropertyDoesntExist(name);
    } 
    return it;
  }

  Item::PropertiesContainer::const_iterator Item::EnsurePropertyExist(const std::string& name) const
  {
    PropertiesContainer::const_iterator it = Properties.find(name);
    if (it == Properties.end())
    {
      ThrowPropertyDoesntExist(name);
    } 
    return it;
  }

  Item::PropertiesContainer::iterator Item::EnsurePropertyExist(const Common::ObjectID& id)
  {
    PropertiesContainer::iterator it = std::find_if(Properties.begin(), Properties.end(), PropertyEqualByID(id));
    if (it == Properties.end())
    {
      ThrowPropertyDoesntExist(id);
    } 
    return it;
  }

  Item::PropertiesContainer::const_iterator Item::EnsurePropertyExist(const Common::ObjectID& id) const
  {
    PropertiesContainer::const_iterator it = std::find_if(Properties.begin(), Properties.end(), PropertyEqualByID(id));
    if (it == Properties.end())
    {
      ThrowPropertyDoesntExist(id);
    } 
    return it;
  }

  void Item::EnsurePropertyNotExist(const std::string& name) const
  {
    if (Properties.find(name) != Properties.end())
    {
      ThrowPropertyExist(name);
    } 
  }

  void Item::EnsurePropertyNotExist(const Common::ObjectID& id) const
  {
    if (std::find_if(Properties.begin(), Properties.end(), PropertyEqualByID(id)) != Properties.end())
    {
      ThrowPropertyExist(id);
    } 
  }
}
