/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include "../../services_registry/src/services_registry_factory.h"

#include <opc/common/addons_core/addon_manager.h>

namespace OpcUa
{
  namespace Test
  {

    void RegisterServicesRegistry(Common::AddonsManager& addons)
    {
      Common::AddonInformation services;
      services.Factory.reset(new OpcUa::Server::ServicesRegistryFactory());
      services.ID = OpcUa::Server::ServicesRegistryAddonID;
      addons.Register(services);
    }

  }
}


