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

#include <opc/ua/server/addons/builtin_server.h>
#include <opc/ua/server/builtin_server.h>
#include <opc/common/addons_core/addon_manager.h>

namespace OpcUa
{
  namespace Test
  {

    void RegisterBuiltinServerAddon(Common::AddonsManager& addons)
    {
      Common::AddonInformation config;
      config.Factory.reset(new OpcUa::UaServer::BuiltingServerFactory());
      config.ID = OpcUa::UaServer::TcpServerAddonID;
      addons.Register(config);
    }

  } // namespace Test
} // namespace OpcUa


