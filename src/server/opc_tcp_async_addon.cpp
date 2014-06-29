/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include "opc_tcp_async_parameters.h"
#include "endpoints_parameters.h"

#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/opc_tcp_async.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/opc_tcp_async.h>

#include <iostream>

namespace
{
  using namespace OpcUa::UaServer;

  class AsyncOpcTcpAddon : public Common::Addon
  {
  public:
    DEFINE_CLASS_POINTERS(AsyncOpcTcpAddon);

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params) override;
    virtual void Stop() override;

  public:
    void PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const;

  private:
    AsyncOpcTcp::UniquePtr OpcTcp;
    Common::Thread::UniquePtr ListenThread;
  };


  void AsyncOpcTcpAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& addonParams)
  {
    const AsyncOpcTcp::Parameters& params = GetOpcTcpParameters(addonParams);
    if (params.DebugMode)
    {
      std::cout << "Enabled debug mode at async opc tcp addon." << std::endl;
    }
    const std::vector<OpcUa::UaServer::ApplicationData> applications = OpcUa::ParseEndpointsParameters(addonParams.Groups, params.DebugMode);
    if (params.DebugMode)
    {
      for (OpcUa::UaServer::ApplicationData d: applications)
      {
        std::cout << "Endpoint is: " << d.Endpoints.front().EndpointURL << std::endl;
      }
    }

    std::vector<OpcUa::ApplicationDescription> applicationDescriptions;
    std::vector<OpcUa::EndpointDescription> endpointDescriptions;
    for (const OpcUa::UaServer::ApplicationData application : applications)
    {
      applicationDescriptions.push_back(application.Application);
      endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
    }

    PublishApplicationsInformation(applicationDescriptions, endpointDescriptions, addons);
    OpcUa::UaServer::ServicesRegistry::SharedPtr internalServer = addons.GetAddon<OpcUa::UaServer::ServicesRegistry>(OpcUa::UaServer::ServicesRegistryAddonID);
    OpcTcp = CreateAsyncOpcTcp(params, internalServer->GetServer());

    ListenThread.reset(new Common::Thread([this](){
          OpcTcp->Listen();
     }));
  }

  void AsyncOpcTcpAddon::PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const
  {
    OpcUa::UaServer::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::UaServer::EndpointsRegistry>(OpcUa::UaServer::EndpointsRegistryAddonID);
    if (!endpointsAddon)
    {
      std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
      return;
    }
    endpointsAddon->AddEndpoints(endpoints);
    endpointsAddon->AddApplications(applications);
  }

  void AsyncOpcTcpAddon::Stop()
  {
    OpcTcp->Shutdown();
    ListenThread->Join();
    OpcTcp.reset();
  }

}

namespace OpcUa
{
  namespace UaServer
  {

    Common::Addon::UniquePtr AsyncOpcTcpAddonFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new AsyncOpcTcpAddon());
    }

  }
}
