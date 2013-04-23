/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/protocol/endpoints.h>

namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class DefaultEndpoints : public EndpointServices
  {
  public:
    virtual std::vector<OpcUa::ApplicationDescription> FindServers(const ApplicationFilter& filter) const
    {
      return std::vector<OpcUa::ApplicationDescription>();
    }
    virtual std::vector<OpcUa::EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      return std::vector<OpcUa::EndpointDescription>();
    }
    virtual void RegisterServer(const ServerParameters& parameters)
    {
    }
  };

  std::shared_ptr<EndpointServices> CreateDefaultEndpoints()
  {
    return std::shared_ptr<EndpointServices>(new DefaultEndpoints());
  }

  class InternalComputer : public Computer
  {
  public:
    InternalComputer()
      : EndpointsServices(CreateDefaultEndpoints())
    {
    }

    virtual void CreateSession(const SessionParameters& parameters)
    {
    }

    virtual void ActivateSession()
    {
    }

    virtual void CloseSession()
    {
    }

    virtual std::shared_ptr<EndpointServices> Endpoints() const
    {
      return EndpointsServices;
    }

    virtual std::shared_ptr<ViewServices> Views() const
    {
      return std::shared_ptr<ViewServices>();
    }

    virtual std::shared_ptr<AttributeServices> Attributes() const
    {
      return std::shared_ptr<AttributeServices>();
    }

  public:
    void SetEndpoints(std::shared_ptr<EndpointServices> endpoints)
    {
      EndpointsServices = endpoints ? endpoints : CreateDefaultEndpoints();
    }

  public:
    std::shared_ptr<EndpointServices> EndpointsServices;
  };

  class RequestProcessor
    : public OpcUa::Server::InternalComputerAddon
  {
  public:
    RequestProcessor()
      : Comp(new InternalComputer())
    {
    }
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
    }

    virtual void Stop()
    {
    }

  public: // InternalComputerAddon
    virtual std::shared_ptr<Computer> GetComputer() const
    {
      return Comp;
    }

    virtual void RegisterEndpointsServices(std::shared_ptr<EndpointServices> endpoints)
    {
      Comp->SetEndpoints(endpoints);
    }

    virtual void UnregisterEndpointsServices()
    {
      Comp->SetEndpoints(std::shared_ptr<EndpointServices>());
    }

  private:
    std::shared_ptr<InternalComputer> Comp;
  };
 
}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new RequestProcessor());
}

