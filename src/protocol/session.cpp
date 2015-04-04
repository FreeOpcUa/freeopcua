#include <opc/ua/protocol/session.h>

namespace OpcUa
{
  ExpandedNodeId GenerateSessionId()
  { 
    static uint32_t sessionId;
    return ExpandedNodeId(NumericNodeId(++sessionId + 5, 0));
  };
}
  


