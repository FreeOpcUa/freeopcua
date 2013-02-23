/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Group class
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#if !defined(___TGEFEST_GROUP__H___)
#define ___TGEFEST_GROUP__H___

#include <opccore/managers/property_tree/item.h>
#include <opccore/managers/property_tree/named_object.h>
#include <opccore/managers/property_tree/tree_object.h>

#include <memory>
#include <string>

namespace Gefest
{
  //--------------------------------------------------------------------------------------
  /// @brief Class Group.
  /// @note typename T in members may be std::string or Common::ObjectID
  class Group
    : public TreeObject
    , public std::enable_shared_from_this<Group>
  {
  public:
    DEFINE_CLASS_POINTERS(Group);

  protected:
    Group()
    {
    }

  public:
   static Group::SharedPtr Create();
   Group::SharedPtr GetSelfPtr();
  
  public:
    virtual void SetName(const std::string& name);

  public: // Items management
    Item::SharedPtr AddItem(const std::string& name);

    void RemoveItem(const std::string& name);
    void RemoveItem(const Common::ObjectID& name);

    bool ItemExists(const std::string& name) const;
    bool ItemExists(const Common::ObjectID& name) const;

    Item::SharedPtr GetItem(const std::string& name) const;
    Item::SharedPtr GetItem(const Common::ObjectID& name) const;

    std::vector<std::string> GetItemsNames() const;
    std::vector<Common::ObjectID> GetItemsIDs() const;

    std::vector<std::shared_ptr<Item> > GetItems() const;

  public: // Subgroups management
    Group::SharedPtr AddSubGroup(const std::string& name);
    
    void RemoveSubGroup(const std::string& name);
    void RemoveSubGroup(const Common::ObjectID& name);

    bool SubGroupExists(const std::string& name) const;
    bool SubGroupExists(const Common::ObjectID& name) const;

    Group::SharedPtr GetSubGroup(const std::string& name);
    Group::SharedPtr GetSubGroup(const Common::ObjectID& name);
    
    std::vector<std::string> GetSubGroupsNames() const;
    std::vector<Common::ObjectID> GetSubGroupsIDs() const;

    std::vector<std::shared_ptr<Group> > GetSubGroups() const;

  public:
    typedef std::map<std::string, Item::SharedPtr> ItemsContainer;
    typedef std::map<std::string, Group::SharedPtr> GroupsContainer;

  private:
    void EnsureGroupNotExist(const std::string& name) const;
    void EnsureGroupNotExist(const Common::ObjectID& name) const;
 
    GroupsContainer::iterator  EnsureGroupExist(const std::string& name);
    GroupsContainer::iterator  EnsureGroupExist(const Common::ObjectID& name);
  
    GroupsContainer::const_iterator EnsureGroupExist(const std::string& name) const;
    GroupsContainer::const_iterator EnsureGroupExist(const Common::ObjectID& name) const;

    void EnsureItemNotExist(const std::string& name) const;
    void EnsureItemNotExist(const Common::ObjectID& name) const;

    ItemsContainer::iterator  EnsureItemExist(const std::string& name);
    ItemsContainer::iterator  EnsureItemExist(const Common::ObjectID& name);

    ItemsContainer::const_iterator EnsureItemExist(const std::string& name) const;
    ItemsContainer::const_iterator EnsureItemExist(const Common::ObjectID& name) const;

  private:
    ItemsContainer Items;
    GroupsContainer Groups;
  };
};

#endif // ___TGEFEST_GROUP__H___

