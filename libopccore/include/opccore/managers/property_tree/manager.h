/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Manager of Property tree
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef manager_h_534d56ad_d0c9_46d9_8dcd_a5ac4c0a807b
#define manager_h_534d56ad_d0c9_46d9_8dcd_a5ac4c0a807b

#include <opccore/common/noncopyable.h>
#include <opccore/managers/property_tree/property_tree.h>


namespace PropertyTree
{
  class Manager : private Common::NonCopyable 
  {
  public:
    DEFINE_CLASS_POINTERS(Manager);

  public: 
    virtual Gefest::PropertyTree::SharedPtr GetPropertyTree() const = 0;
  };
}

#endif // manager_h_534d56ad_d0c9_46d9_8dcd_a5ac4c0a807b
