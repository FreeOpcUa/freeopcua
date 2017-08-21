
#include "standard_address_space_parts.h"

#include <opc/common/logger.h>
#include <opc/ua/server/standard_address_space.h>

#include <opc/ua/services/node_management.h>


namespace OpcUa
{
namespace Server
{

void FillStandardNamespace(OpcUa::NodeManagementServices & registry, const Common::Logger::SharedPtr & /*logger*/)
{
  OpcUa::CreateAddressSpacePart3(registry);
  OpcUa::CreateAddressSpacePart4(registry);
  OpcUa::CreateAddressSpacePart5(registry);
  OpcUa::CreateAddressSpacePart8(registry);
  OpcUa::CreateAddressSpacePart9(registry);
  OpcUa::CreateAddressSpacePart10(registry);
  OpcUa::CreateAddressSpacePart11(registry);
  OpcUa::CreateAddressSpacePart13(registry);
}

} // namespace UaServer
} // namespace OpcUa



