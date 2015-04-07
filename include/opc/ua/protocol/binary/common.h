/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary messages.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_COMMON_H__
#define __OPC_UA_BINARY_COMMON_H__

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>

#include <algorithm>
#include <stdint.h>
#include <string>
#include <vector>

namespace OpcUa
{
  namespace Binary
  {
    template<typename T>
    std::size_t RawSize(const T& obj);

    template<typename T>
    std::size_t RawSizeContainer(const T& container)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(container.begin(), container.end(), [&] (const typename T::value_type& val) {totalSize += RawSize(val);});
      return totalSize;
    }


    enum MessageType
    {
      MT_INVALID = 0,
      MT_HELLO = 1,
      MT_ACKNOWLEDGE,
      MT_ERROR,
      MT_SECURE_OPEN,
      MT_SECURE_CLOSE,
      MT_SECURE_MESSAGE,
    };

    enum ChunkType
    {
      CHT_INVALID = 0,
      CHT_SINGLE = 1,
      CHT_INTERMEDIATE,
      CHT_FINAL,
    };

    struct Header
    {
      MessageType Type;
      ChunkType Chunk;
      uint32_t Size;

      Header();
      explicit Header(MessageType type, ChunkType chunk);
      std::size_t AddSize(std::size_t size);
      std::size_t MessageSize() const;
      void ResetSize();
    };


    // Hello
    // os << Header << Hello << flush
    // is >> Header >> Acknowledge;

    struct Hello
    {
      uint32_t ProtocolVersion;
      uint32_t ReceiveBufferSize;
      uint32_t SendBufferSize;
      uint32_t MaxMessageSize;
      uint32_t MaxChunkCount;
      std::string EndpointUrl;

      Hello();
    };

    struct Acknowledge
    {
      uint32_t ProtocolVersion;
      uint32_t ReceiveBufferSize;
      uint32_t SendBufferSize;
      uint32_t MaxMessageSize;
      uint32_t MaxChunkCount;

      Acknowledge();
    };

    struct Error
    {
      uint32_t Code;
      std::string Reason;

      Error();
    };

    struct SecureHeader
    {
      MessageType Type;
      ChunkType Chunk;
      uint32_t Size;
      uint32_t ChannelId;

      SecureHeader();
      explicit SecureHeader(MessageType type, ChunkType chunk, uint32_t channelId);

      std::size_t AddSize(std::size_t size);
      std::size_t MessageSize() const;
      void ResetSize();
    };

    struct AsymmetricAlgorithmHeader
    {
      std::string SecurityPolicyURI;
      std::vector<uint8_t> SenderCertificate;
      std::vector<uint8_t> ReceiverCertificateThumbPrint;
    };

    struct SymmetricAlgorithmHeader
    {
      uint32_t TokenId;

      SymmetricAlgorithmHeader();
    };

    struct SequenceHeader
    {
      uint32_t SequenceNumber;
      uint32_t RequestId;

      SequenceHeader();
    };

  } // namespace Binary
} // namespace OpcUa


#endif  // __OPC_UA_BINARY_COMMON_H__

