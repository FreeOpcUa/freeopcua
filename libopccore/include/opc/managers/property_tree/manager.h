/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Manager of Property tree
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef manager_h_534d56ad_d0c9_46d9_8dcd_a5ac4c0a807b
#define manager_h_534d56ad_d0c9_46d9_8dcd_a5ac4c0a807b

#include <opc/common/interface.h>
#include <opc/managers/property_tree/property_tree.h>


namespace PropertyTree
{
  class Manager : private Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(Manager);

  public: 
    virtual Gefest::PropertyTree::SharedPtr GetPropertyTree() const = 0;
  };
}

#endif // manager_h_534d56ad_d0c9_46d9_8dcd_a5ac4c0a807b
