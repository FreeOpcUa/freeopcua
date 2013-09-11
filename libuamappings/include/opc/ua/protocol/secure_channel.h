/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Secure channel service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_MESSAGES_SECURE_CHANNEL_H__
#define __OPC_UA_MESSAGES_SECURE_CHANNEL_H__

#include <opc/ua/protocol/types.h>

namespace OpcUa
{
  // OpenSecureChannel
  // os << SecureHeader 
  //    << AssymmetricAlgoripthmHeader
  //    << SequenceHeader
  //    << NodeID << ExpandedNodeId // if present
  //    << RequestHeader
  //    << OpenSecureChannel

  struct OpenSecureChannelRequest
  {
    NodeID TypeID;
    RequestHeader Header;

    uint32_t ClientProtocolVersion;
    SecurityTokenRequestType RequestType;
    MessageSecurityMode SecurityMode;
    std::vector<uint8_t> ClientNonce;
    uint32_t RequestLifeTime;

    OpenSecureChannelRequest();
  };

  
  // OpenSecureChannelResponse
  // is >> SecureHeader 
  //    >> AsymmetricAlgorithmHeader 
  //    >> SequenceHeader 
  //    >> ResponseHeader
  //    >> GetEndpointsResponse

  struct SecurityToken
  {
    uint32_t SecureChannelID;
    uint32_t TokenID;
    DateTime CreatedAt;
    int32_t RevisedLifetime;

    SecurityToken()
      : SecureChannelID(0)
      , TokenID(0)
      , CreatedAt(0)
      , RevisedLifetime(0)
    {
    }
  };

  struct OpenSecureChannelResponse
  {
    NodeID TypeID;
    ResponseHeader Header;

    uint32_t ServerProtocolVersion;
    SecurityToken ChannelSecurityToken;
    std::vector<uint8_t> ServerNonce;

    OpenSecureChannelResponse();
  };

  struct CloseSecureChannelRequest
  {
    NodeID TypeID;
    RequestHeader Header;

    CloseSecureChannelRequest();
  };

} // namespace OpcUa

#endif // __OPC_UA_MESSAGES_SECURE_CHANNEL_H__

