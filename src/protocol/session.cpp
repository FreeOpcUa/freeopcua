#include <opc/ua/protocol/session.h>

namespace OpcUa
{
  ExpandedNodeID GenerateSessionId()
  { 
    static uint32_t sessionId;
    return ExpandedNodeID(NumericNodeID(++sessionId + 5, 0));
  };
}
  


