#include <opc/ua/protocol/session.h>

namespace OpcUa
{
  NodeID GenerateSessionId()
  { 
    static uint32_t sessionId;
    return NumericNodeID(++sessionId + 5, 0);
  };
}
  


