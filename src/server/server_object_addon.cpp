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

#include "server_object.h"
#include "server_object_addon.h"

#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/asio_addon.h>
#include <opc/ua/node.h>

namespace
{

  class ServerObjectAddon : public Common::Addon
  {
  public:
    void Initialize(Common::AddonsManager& manager, const Common::AddonParameters& parameters) override
    {
      for (const Common::Parameter& param : parameters.Parameters)
      {
        if (param.Name == "debug")
          Debug = param.Value == "false" || param.Value == "0" ? false : true;
      }

      OpcUa::Server::ServicesRegistry::SharedPtr registry = manager.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);
      OpcUa::Server::AsioAddon::SharedPtr asio = manager.GetAddon<OpcUa::Server::AsioAddon>(OpcUa::Server::AsioAddonId);
      OpcUa::Services::SharedPtr services = registry->GetServer();
      Object.reset(new OpcUa::Server::ServerObject(services, asio->GetIoService(), Debug));
    }

    void Stop() override
    {
      Object.reset();
    }

  private:
    bool Debug = false;
    OpcUa::Server::ServerObject::UniquePtr Object;
  };

} // namespace


namespace OpcUa
{
  namespace Server
  {

    Common::Addon::UniquePtr ServerObjectFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new ServerObjectAddon());
    }

  } // namespace UaServer
} // namespace OpcUa
