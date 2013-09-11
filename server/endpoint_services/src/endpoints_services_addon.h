/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/endpoints_services.h>

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/endpoints.h>

namespace OpcUa
{
  namespace Impl
  {

		class EndpointsAddon : public Server::EndpointsServicesAddon
		{
		public:
			virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
			virtual void Stop();

			virtual void AddEndpoints(const std::vector<EndpointDescription>& endpoints);
      virtual void AddApplications(const std::vector<OpcUa::ApplicationDescription>& application);

		private:
      void ApplyAddonParameters(const Common::AddonParameters& addons);

		private:
			class EndpointsImpl;
			std::shared_ptr<EndpointsImpl> Services;
			std::shared_ptr<Server::ServicesRegistryAddon> InternalComputer;
			bool Debug;
		};

  } // namespace Impl
} // namespace OpcUa

