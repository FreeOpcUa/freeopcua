#pragma once

#include <opc/ua/protocol/protocol_auto.h>

namespace OpcUa
{
  ReadValueId inline ToReadValueId(NodeId id, AttributeId attr)
  {
    ReadValueId rv;
    rv.NodeId = id;
    rv.AttributeId = attr;
    return rv;
  }
}
