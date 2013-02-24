/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Item class
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#if !defined(___TGEFEST_TAG__H___)
#define ___TGEFEST_TAG__H___

#include <opccore/common/value.h>
#include <opccore/managers/property_tree/tree_object.h>

#include <map>
#include <string>
#include <vector>



namespace Gefest
{
  class Group;
  
  struct Property
  {
    Common::ObjectID ID;
    std::string Name;
    Common::Value Value;

  public:
    Property()
      : ID(Common::GenerateNewID())
    {
    }

    explicit Property(const std::string& name)
      : ID(Common::GenerateNewID())
      , Name(name)
    {
    }

    Property(const std::string& name, const Common::Value& value)
      : ID(Common::GenerateNewID())
      , Name(name)
      , Value(value)
    {
    }
  };

  //--------------------------------------------------------------------------------------
  /**
    \brief Class Item.
    Item class Consist from value and set of properties.
  */

  class Item
    : public TreeObject
    , public std::enable_shared_from_this<Item>
  {
  public:
    DEFINE_CLASS_POINTERS(Item);

  protected:
    Item()
    {
    }

  public:
    static Item::SharedPtr Create();
    Item::SharedPtr GetSelfPtr();

    virtual ~Item(){}

    virtual Common::Value GetValue() const;
    virtual void SetValue(const Common::Value& val);

    virtual void SetName(const std::string& name);

  public: 
    void AddProperty(const Property& property);
    void RemoveProperty(const std::string& name);
    void RemoveProperty(const Common::ObjectID& id);

    Property GetProperty(const std::string& name) const;
    Property GetProperty(const Common::ObjectID& id) const;

    void SetPropertyValue(const std::string& name, const Common::Value& value);
    void SetPropertyValue(const Common::ObjectID& id, const Common::Value& value);

    std::vector<std::string> GetPropertiesNames() const;
    std::vector<Common::ObjectID> GetPropertiesIDs() const;

  public: 
    typedef std::map<std::string, Property> PropertiesContainer;

  private:
    PropertiesContainer::iterator EnsurePropertyExist(const std::string& name);
    PropertiesContainer::const_iterator EnsurePropertyExist(const std::string& name) const;

    PropertiesContainer::iterator EnsurePropertyExist(const Common::ObjectID& name);
    PropertiesContainer::const_iterator EnsurePropertyExist(const Common::ObjectID& name) const;

    void EnsurePropertyNotExist(const std::string& iname) const;
    void EnsurePropertyNotExist(const Common::ObjectID& id) const;
 
  private:
     PropertiesContainer Properties;
     Common::Value Value;
  };
};

#endif // ___TGEFEST_TAG__H__

