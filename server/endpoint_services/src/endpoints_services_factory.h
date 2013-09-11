/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints_services_addon.h"


namespace OpcUa
{
  namespace Impl
  {

     class EndpointsAddonFactory : public Common::AddonFactory
     {
     public:
       /// @brief Create instance of addon
       virtual Common::Addon::UniquePtr CreateAddon()
       {
         return Common::Addon::UniquePtr(new OpcUa::Impl::EndpointsAddon());
       }
		 };

  } // namespace Impl
} // namespace OpcUa

