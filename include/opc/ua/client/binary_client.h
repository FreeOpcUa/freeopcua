/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua server interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/protocol/channel.h>
#include <opc/ua/services/services.h>


#include <memory>

namespace OpcUa
{

    struct SecureConnectionParams
    {
      std::string EndpointUrl;
      std::string SecurePolicy;
      std::vector<uint8_t> SenderCertificate;
      std::vector<uint8_t> ReceiverCertificateThumbPrint;
      uint32_t SecureChannelId;

      SecureConnectionParams()
        : SecureChannelId(0)
      {
      }
    };

    /// @brief Create server based on opc ua binary protocol.
    /// @param channel channel wich will be used for sending requests data.
    Services::SharedPtr CreateBinaryClient(IOChannel::SharedPtr channel, const SecureConnectionParams& params, bool debug = false);
    Services::SharedPtr CreateBinaryClient(const std::string& endpointUrl, bool debug = false);

} // namespace OpcUa
