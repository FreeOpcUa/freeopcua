/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary secure channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_SECURE_CHANNEL
#define __OPC_UA_BINARY_SECURE_CHANNEL

#include <opc/ua/protocol/channel.h>

#include <memory>
#include <string>
#include <vector>

namespace OpcUa
{
  namespace Binary
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

    /// @brief Open secure channel with remote server
    /// @param rawChannel connection to remote server
    std::unique_ptr<IOChannel> CreateSecureChannel(std::shared_ptr<IOChannel> remoteConnection, const SecureConnectionParams& params);

  } // nmespace Bunary
} // namespace OpcUa

#endif // __OPC_UA_BINARY_SECURE_CHANNEL

