/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Server wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/protocol/endpoints.h>

namespace OpcUa
{
  namespace Impl
  {

    class ServicesRegistry : public OpcUa::UaServer::ServicesRegistryAddon
    {
    public:
      ServicesRegistry();
      ~ServicesRegistry(){std::cout << "Service registry is deleted" << std::endl;}

      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    public: // InternalServerAddon
      virtual std::shared_ptr<OpcUa::Remote::Server> GetServer() const;
      virtual void RegisterEndpointsServices(std::shared_ptr<OpcUa::Remote::EndpointServices> endpoints) override;
      virtual void UnregisterEndpointsServices()  override;
      virtual void RegisterViewServices(std::shared_ptr<OpcUa::Remote::ViewServices> views) override;
      virtual void UnregisterViewServices() override;
      virtual void RegisterNodeManagementServices(std::shared_ptr<OpcUa::Remote::NodeManagementServices> addr) override;
      virtual void UnregisterNodeManagementServices() override;
      virtual void RegisterAttributeServices(std::shared_ptr<OpcUa::Remote::AttributeServices> attributes) override;
      virtual void UnregisterAttributeServices() override;

    private:
      class InternalServer;
      std::shared_ptr<InternalServer> Comp;
    };

  } // namespace Impl
} // namespace OpcUa
