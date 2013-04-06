/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Manager of Property tree
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/id.h>
#include <opccore/managers/property_tree/property_tree.h>
#include <opccore/managers/property_tree/register.h>

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>

namespace
{
  class PropertyTreeManagerImpl
    : public PropertyTree::Manager
    , public Common::Addon
  {
  public:
    PropertyTreeManagerImpl()
      : Tree(Gefest::PropertyTree::Create())
    {
    }
  
  public: // Addon interface
    virtual void Initialize()
    {
    }

    virtual void Stop()
    {
    }

  public: // PropertyTree::Manager
    virtual Gefest::PropertyTree::SharedPtr GetPropertyTree() const
    {
      return Tree;
    }

  private:
    Gefest::PropertyTree::SharedPtr Tree;
  };

  class PropertyTreeManagerFactory : public Common::AddonFactory
  {
  public:
    virtual Common::Addon::UniquePtr CreateAddon()
    {
      return Common::Addon::UniquePtr(new PropertyTreeManagerImpl());
    }
  };

} // unnamed namespace


void PropertyTree::RegisterPropertyTreeAddon(Common::AddonsManager& addonsManager)
{
  addonsManager.Register(ManagerID, Common::AddonFactory::UniquePtr(new PropertyTreeManagerFactory()));
}

