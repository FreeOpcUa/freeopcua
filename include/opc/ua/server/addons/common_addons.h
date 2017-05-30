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

#pragma once

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/protocol.h>

namespace OpcUa
{
namespace Server
{

struct Parameters
{
  /// @Endpoint configuration.
  /// listen parameters will be taked from EndpointUrl.
  /// Example:
  /// opc.tcp://localhost:4841
  /// opc.tcp://10.250.1.1:4841
  /// opc.tcp://opcua.server.com:4841
  EndpointDescription Endpoint;
  unsigned ThreadsCount = 1;
  bool Debug = false;
};

/// @brief parameters of server.
/// can be used at embedded.
void RegisterCommonAddons(const Parameters & params, Common::AddonsManager & addons);

/// @brief Load parameters from configuration files.
/// This function will enumerate '*.config' files in the directory.
/// configuration file can load third party dynamic addons.
void LoadConfiguration(const std::string & configDirectoryPath, Common::AddonsManager & addons);

Common::AddonInformation CreateServicesRegistryAddon();
Common::AddonInformation CreateAddressSpaceAddon();
Common::AddonInformation CreateStandardNamespaceAddon();
Common::AddonInformation CreateEndpointsRegistryAddon();
Common::AddonInformation CreateBinaryServerAddon();
Common::AddonInformation CreateOpcTcpAsyncAddon();
Common::AddonInformation CreateServerObjectAddon();
Common::AddonInformation CreateAsioAddon();
Common::AddonInformation CreateSubscriptionServiceAddon();


}
}
