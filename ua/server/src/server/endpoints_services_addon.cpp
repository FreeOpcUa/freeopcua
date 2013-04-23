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
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/endpoints.h>
#include <opccore/common/addons_core/addon_manager.h>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;
  using namespace OpcUa::Remote;

  class EndpointsImpl : public EndpointServices
  {
  public:
    virtual std::vector<ApplicationDescription> FindServers(const ApplicationFilter& filter) const
    {
      return std::vector<ApplicationDescription>();
    }

    virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      return Endpoints;
    }

    virtual void RegisterServer(const OpcUa::Remote::ServerParameters& parameters)
    {
    }

  public:
    void AddEndpoints(std::vector<EndpointDescription> endpoints)
    {
      Endpoints.insert(begin(Endpoints), begin(endpoints), end(endpoints));
    }

  private:
    std::vector<EndpointDescription> Endpoints;
  };


  class EndpointsAddon : public EndpointsServicesAddon
  {
  public:
    EndpointsAddon()
    {
    }

    virtual ~EndpointsAddon()
    {
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Services.reset(new EndpointsImpl());
      InternalComputer = Common::GetAddon<InternalComputerAddon>(addons, InternalComputerAddonID);
      InternalComputer->RegisterEndpointsServices(Services);
    }

    virtual void Stop()
    {
      InternalComputer->UnregisterEndpointsServices();
      InternalComputer.reset();
      Services.reset();
    }

    virtual void AddEndpoints(const std::vector<EndpointDescription>& endpoints)
    {
      Services->AddEndpoints(endpoints);
    }

  private:
    std::shared_ptr<EndpointsImpl> Services;
    std::shared_ptr<InternalComputerAddon> InternalComputer;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new EndpointsAddon());
}

