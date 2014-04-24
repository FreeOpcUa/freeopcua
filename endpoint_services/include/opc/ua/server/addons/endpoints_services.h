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

#include <opc/common/addons_core/addon.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/types.h>

namespace OpcUa
{
  namespace UaServer
  {

    class EndpointsServicesAddon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(EndpointsServicesAddon);

    public:
      virtual void AddEndpoints(const std::vector<OpcUa::EndpointDescription>& endpoints) = 0;
      virtual void AddApplications(const std::vector<OpcUa::ApplicationDescription>& application) = 0;
    };

    const char EndpointsServicesAddonID[] = "endpoints_services";

  } // namespace UaServer
} // nmespace OpcUa

