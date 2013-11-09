/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>

#include "discovery_service.h"
#include "endpoint_service.h"
#include "soap_service.h"
#include "soap_server.h"

namespace OpcUa
{
  namespace Soap
  {

    class SoapAddon : public Common::Addon
    {
    public:
      SoapAddon();

      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    private:
      void ApplyAddonParameters(const Common::AddonParameters& params);

    private:
      bool Debug;
      std::unique_ptr<Server> ServerInstance;
    };

  } // namespace Soap
} // namespace OpcUa

