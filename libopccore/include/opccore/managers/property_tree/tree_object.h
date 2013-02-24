/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Named object
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __MOZOPC_PROPERTY_TREE_OBJECT_H__
#define __MOZOPC_PROPERTY_TREE_OBJECT_H__

#include <opccore/managers/property_tree/named_object.h>
#include <opccore/common/class_pointers.h>

namespace Gefest
{
  class TreeObject : public NamedObject
  {
  public:
    DEFINE_CLASS_POINTERS(TreeObject);

  public:
    virtual ~TreeObject()
    {
    }
    
    TreeObject::SharedPtr GetParent() const;
    void SetParent(TreeObject::SharedPtr parent);

    std::string GetObjectPath() const;

  private:
    TreeObject::SharedPtr ParentObject;
  };
}

#endif // __MOZOPC_PROPERTY_TREE_OBJECT_H_

