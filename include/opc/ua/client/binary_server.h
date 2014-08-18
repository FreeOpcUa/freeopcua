/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua server interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_BINARY_server_H
#define OPC_UA_BINARY_server_H

#include <opc/ua/protocol/channel.h>
#include <opc/ua/server.h>


#include <memory>

namespace OpcUa
{
  namespace Remote
  {

    struct SecureConnectionParams
    {
      std::string EndpointUrl;
      std::string SecurePolicy;
      std::vector<uint8_t> SenderCertificate;
      std::vector<uint8_t> ReceiverCertificateThumbPrint;
      uint32_t SecureChannelID;

      SecureConnectionParams()
        : SecureChannelID(0)
      {
      }
    };

    /// @brief Create server based on opc ua binary protocol.
    /// @param channel channel wich will be used for sending requests data.
    Server::SharedPtr CreateBinaryServer(IOChannel::SharedPtr channel, const SecureConnectionParams& params, bool debug = false);

  } // namespace Remote
} // namespace OpcUa

#endif //  OPC_UA_BINARY_server_H

