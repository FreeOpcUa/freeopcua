/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Property tree
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __PROPERTY_TREE_H__
#define __PROPERTY_TREE_H__

#include <opc/managers/property_tree/group.h>

namespace Gefest
{
  class PropertyTree
    : public std::enable_shared_from_this<PropertyTree>
  {
  public:
    DEFINE_CLASS_POINTERS(PropertyTree);

  public:
    virtual ~PropertyTree()
    {
    }
    
  private:
    PropertyTree();
 
  public:
    static PropertyTree::SharedPtr Create();
    PropertyTree::SharedPtr GetSelfPtr();

  public:
    /// @brief find Item by path
    /// @param pathToItem path to item.
    /// @return Item or NULL
    /// @throws if path is invalid
    Property FindProperty(const std::string& pathToProperty) const;

    /// @brief find Item by path
    /// @param pathToItem path to required item.
    /// @return Item or NULL
    /// @throws if path is invalid
    Item::SharedPtr FindItem(const std::string& pathToItem) const;

    /// @brief find Group by path
    /// @param pathToGroup path to required group.
    /// @return Item or NULL
    /// @throws if path is invalid
    Group::SharedPtr FindGroup(const std::string& pathToGroup) const;

    /// @brief Get Root group of property tree.
    /// @return Not NUL Group.
    Group::SharedPtr GetRootGroup();

  private:
    Group::SharedPtr RootGroup;
  };
}

#endif // __PROPERTY_TREE_H__

