/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_MESSAGE_IDENTIFIERS
#define __OPC_UA_BINARY_MESSAGE_IDENTIFIERS

namespace OpcUa
{
  enum MessageID
  {
    INVALID = 0,

    ACTIVATE_SESSION_REQUEST   = 0x1d3, //467;
    ACTIVATE_SESSION_RESPONSE = 0x1d6, //470;

    BROWSE_REQUEST  = 0x20f, // 527;
    BROWSE_RESPONSE = 0x212, //530;

    BROWSE_NEXT_REQUEST  = 0x215, // 533
    BROWSE_NEXT_RESPONSE = 0x218, // 536

    CLOSE_SECURE_CHANNEL_REQUEST = 0x1c4, // 452

    CLOSE_SESSION_REQUEST  = 0x1d9, // 473;
    CLOSE_SESSION_RESPONSE = 0x1dc, // 476;

    // Session services
    CREATE_SESSION_REQUEST  = 0x1cd, // 461;
    CREATE_SESSION_RESPONSE = 0x1d0, // 464;

    // Endpoints services
    GET_ENDPOINTS_REQUEST  = 0x1ac, // 428
    GET_ENDPOINTS_RESPONSE = 0x1af, // 431

    // Secure channel services
    OPEN_SECURE_CHANNEL_REQUEST  = 0x1be, // 446
    OPEN_SECURE_CHANNEL_RESPONSE = 0x1c1, // 449

    READ_REQUEST  = 0x277, // 631
    READ_RESPONSE = 0x27A, // 634

    WRITE_REQUEST  = 0x2A1, //673
    WRITE_RESPONSE = 0x2A4, // 676

  };

  struct NodeID;

  MessageID GetMessageID(const NodeID& id);

}

#endif // __OPC_UA_BINARY_MESSAGE_IDENTIFIERS
