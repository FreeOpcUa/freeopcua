#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/server/addons/address_space_registry.h>

namespace OpcUa
{
  namespace UaServer
  {
    void CreateStandardNamespace(Remote::NodeManagementServices::SharedPtr Service);
  }
}

