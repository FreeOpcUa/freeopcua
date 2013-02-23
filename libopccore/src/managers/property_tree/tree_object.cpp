/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Named object realization
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opccore/managers/property_tree/tree_object.h>
#include <opccore/managers/property_tree/errors.h>


namespace Gefest
{
  TreeObject::SharedPtr TreeObject::GetParent() const
  {
    return ParentObject;
  }

  void TreeObject::SetParent(TreeObject::SharedPtr parentObject)
  {
    ParentObject = parentObject;
  }

  std::string TreeObject::GetObjectPath() const
  {
    if (ParentObject.get())
    {
      return ParentObject->GetObjectPath() + "/" + GetName();
    }
    return GetName();
  }
}

