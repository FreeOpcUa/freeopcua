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

#include <opc/common/addons_core/addon_parameters.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/server/endpoints_services.h>


namespace OpcUa
{

  std::vector<UaServer::ApplicationData> ParseEndpointsParameters(const std::vector<Common::ParametersGroup>& rootGroup, bool debug);

}
