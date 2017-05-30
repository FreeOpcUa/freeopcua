/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Secure channel service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_SESSIONS_H__
#define __OPC_UA_BINARY_SESSIONS_H__

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/protocol.h>

namespace OpcUa
{
//-------------------------------------------------
// CloseSessionRequest
//-------------------------------------------------

struct CloseSessionRequest
{
  NodeId TypeId;
  RequestHeader Header;

  bool DeleteSubscriptions;

  CloseSessionRequest();
};

struct CloseSessionResponse
{
  NodeId TypeId;
  ResponseHeader Header;

  CloseSessionResponse();
};

ExpandedNodeId GenerateSessionId();

struct ServiceFaultResponse
{
  NodeId TypeId;
  ResponseHeader Header;

  ServiceFaultResponse();
};


} // namespace OpcUa

#endif // __OPC_UA_BINARY_SESSIONS_H__
