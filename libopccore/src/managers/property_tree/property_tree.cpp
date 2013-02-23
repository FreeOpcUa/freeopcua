/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Property tree realization
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opccore/managers/property_tree/property_tree.h>

#include <opccore/managers/property_tree/errors.h>

namespace
{
  using namespace Gefest;
  const char PATH_DELIMITER = '/';

  void EnsurePathValid(std::string::const_iterator startIt, std::string::const_iterator endIt)
  {
    if (startIt == endIt)
    {
      THROW_ERROR1(IllegalPropertyTreePath, std::string(startIt, endIt));
    }
    if (*startIt != PATH_DELIMITER)
    {
      THROW_ERROR(PropertyTreePathShouldStartFromSlash);
    }
  }

  
  // Find group  by path. 
  // StartIt will point to last part of path which may contain item and properties
  // if not found will throw an error
  Gefest::Group::SharedPtr GetGroup(const Gefest::Group::SharedPtr& rootGroup, std::string::const_iterator& startIt, std::string::const_iterator& endIt)
  {
    EnsurePathValid(startIt, endIt);

    ++startIt;
    std::string::const_iterator nameEnd = std::find(startIt, endIt, PATH_DELIMITER);
    const std::string name(startIt, nameEnd);
    if (nameEnd == endIt)
    {
      return rootGroup->SubGroupExists(name) ? rootGroup->GetSubGroup(name) : Gefest::Group::SharedPtr();
    }

    return GetGroup(rootGroup->GetSubGroup(name), nameEnd, endIt);
  }

  Gefest::Item::SharedPtr GetItem(const Gefest::Group::SharedPtr& rootGroup, std::string::const_iterator& startIt, std::string::const_iterator& endIt)
  {
    EnsurePathValid(startIt, endIt);
    // find last delimeter - where starts item name and finishes group path
    std::string delim(1, PATH_DELIMITER);
    std::string::const_iterator itemAndGroupDelimiter = std::find_end(startIt, endIt, delim.begin(), delim.end());

    Gefest::Group::SharedPtr group = (itemAndGroupDelimiter == startIt) ? rootGroup : GetGroup(rootGroup, startIt, itemAndGroupDelimiter);
    if (!group.get())
    {
      return Gefest::Item::SharedPtr();
    }
    const std::string itemName(itemAndGroupDelimiter + 1, endIt);
    if (!group->ItemExists(itemName))
    {
      return Gefest::Item::SharedPtr();
    }
    return group->GetItem(itemName);
  }
  
  Gefest::Property GetProperty(const Gefest::Group::SharedPtr rootGroup, std::string::const_iterator& startIt, std::string::const_iterator& endIt)
  {
    EnsurePathValid(startIt, endIt);
    // find last delimeter - where starts item name and finishes group path
    const std::string delim(1, PATH_DELIMITER);
    std::string::const_iterator itemAndPropertyDelimiter = std::find_end(startIt, endIt, delim.begin(), delim.end());
    if (itemAndPropertyDelimiter == startIt)
    {
      return Gefest::Property();
    }

    const Gefest::Item::SharedPtr item = GetItem(rootGroup, startIt, itemAndPropertyDelimiter);
    const std::string name(itemAndPropertyDelimiter + 1, endIt);
    if (!item.get())
    {
      THROW_ERROR1(PropertyDoesntExist, name);
    }
    return item->GetProperty(name);
  }
  
  class RootGroupImpl : public Gefest::Group
  {
  private:
    RootGroupImpl()
    {
    }
  
  public:
    static Gefest::Group::SharedPtr Create()
    {
      return Gefest::Group::SharedPtr(new RootGroupImpl);
    }
    
    virtual void SetName(const std::string&)
    {
      THROW_ERROR(UnableToRenameRootGroup);
    }
  };
}

namespace Gefest
{
  PropertyTree::PropertyTree()
    : RootGroup(::RootGroupImpl::Create())
  {
  }

  PropertyTree::SharedPtr PropertyTree::Create()
  {
    return PropertyTree::SharedPtr(new PropertyTree);
  }

  PropertyTree::SharedPtr PropertyTree::GetSelfPtr()
  {
    return shared_from_this();
  }


  Property PropertyTree::FindProperty(const std::string& pathToProperty) const
  {
    std::string::const_iterator startIt = pathToProperty.begin();
    std::string::const_iterator endIt = pathToProperty.end();
    return GetProperty(RootGroup, startIt, endIt);
  }

  Item::SharedPtr PropertyTree::FindItem(const std::string& pathToItem) const
  {
    std::string::const_iterator startIt = pathToItem.begin();
    std::string::const_iterator endIt = pathToItem.end();
    return GetItem(RootGroup, startIt, endIt);
  }

  Group::SharedPtr PropertyTree::FindGroup(const std::string& groupPath) const
  {
    std::string::const_iterator startIt = groupPath.begin();
    std::string::const_iterator endIt = groupPath.end();
    return GetGroup(RootGroup, startIt, endIt);
  }

  Group::SharedPtr PropertyTree::GetRootGroup()
  {
    return RootGroup;
  }
}

