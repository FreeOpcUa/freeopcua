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

#include <opc/ua/binary/types.h>
#include <opc/ua/extension_identifiers.h>
#include <opc/ua/message_identifiers.h>

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
      uint32_t ChannelID;

      SecureHeader();
      explicit SecureHeader(MessageType type, ChunkType chunk, uint32_t channelID);

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
      uint32_t TokenID;

      SymmetricAlgorithmHeader();
    };

    struct SequenceHeader
    {
      uint32_t SequenceNumber;
      uint32_t RequestID;

      SequenceHeader();
    };

    enum NodeIDEncoding : uint8_t
    {
      EV_TWO_BYTE = 0,
      EV_FOUR_BYTE = 1,
      EV_NUMERIC = 2,
      EV_STRING = 3,
      EV_GUID = 4,
      EV_BYTE_STRING = 5,

      EV_SERVER_INDEX_FLAG = 0x40,
      EV_NAMESPACE_URI_FLAG = 0x80,
    };

    struct NodeID
    {
      NodeIDEncoding Encoding;
      std::string NamespaceURI;
      uint32_t ServerIndex;

      struct TwoByteDataType
      {
        uint8_t Identifier;

        TwoByteDataType()
          : Identifier(0)
        {
        }

      } TwoByteData;

      struct FourByteDataType
      {
        uint8_t NamespaceIndex;
        uint16_t Identifier;
 
        FourByteDataType()
          : NamespaceIndex(0)
          , Identifier(0)
        {
        }
      }FourByteData;

      struct NumericDataType
      {
        uint16_t NamespaceIndex;
        uint32_t Identifier;
 
        NumericDataType()
          : NamespaceIndex(0)
          , Identifier(0)
        {
        }
      }NumericData;


      struct StringDataType
      {
        uint16_t NamespaceIndex;
        std::string Identifier;

        StringDataType()
          : NamespaceIndex(0)
        {
        }

      }StringData;

      struct BinaryDataType
      {
        uint16_t NamespaceIndex;
        std::vector<uint8_t> Identifier;

        BinaryDataType()
          : NamespaceIndex(0)
        {
        }

      }BinaryData;

      struct GuidDataType
      {
        uint16_t NamespaceIndex;
        Guid Identifier;

        GuidDataType()
          : NamespaceIndex(0)
        {
        }

      }GuidData;

      NodeID();
      NodeID(MessageID messageID);
    };

    struct AdditionalHeader
    {
      NodeID TypeID;
      uint8_t Encoding;

      AdditionalHeader()
        : Encoding(0)
      {
      }
    };

    struct RequestHeader
    {
      NodeID SessionAuthenticationToken;
      DateTime UtcTime;
      uint32_t RequestHandle;
      uint32_t ReturnDiagnostics;
      std::string AuditEntryID;
      uint32_t Timeout; // in miliseconds
      AdditionalHeader Additional;

      RequestHeader();
    };


    enum DiagnosticInfoMask : uint8_t
    {
      DIM_NONE                  = 0,
      DIM_SYMBOLIC_ID           = 0x1,
      DIM_NAMESPACE             = 0x2,
      DIM_LOCALIZED_TEXT        = 0x4,
      DIM_LOCALE                = 0x8,
      DIM_ADDITIONAL_INFO       = 0x10,
      DIM_INNER_STATUS_CODE     = 0x20,
      DIM_INNER_DIAGNOSTIC_INFO = 0x40
    };

    struct DiagnosticInfo
    {
      DiagnosticInfoMask EncodingMask;
      int32_t SymbolicID;
      int32_t NamespaceURI;
      int32_t LocalizedText;
      int32_t Locale;
      std::string AdditionalInfo;
      StatusCode InnerStatusCode;

      DiagnosticInfo()
        : EncodingMask(DiagnosticInfoMask::DIM_NONE)
        , SymbolicID(0)
        , NamespaceURI(0)
        , LocalizedText(0)
        , Locale(0)
        , InnerStatusCode(0)
      {
      }
    };

    typedef std::vector<DiagnosticInfo> DiagnosticInfoList;

    struct ResponseHeader
    {
      DateTime Timestamp;
      uint32_t RequestHandle;
      StatusCode ServiceResult;
      DiagnosticInfoList InnerDiagnostics;
      std::vector<std::string> StringTable;
      AdditionalHeader Additional;

      ResponseHeader();
    };

    enum SecurityTokenRequestType : uint32_t
    {
      STR_ISSUE = 0,
      STR_RENEW = 1,
    };

    enum MessageSecurityMode : uint32_t
    {
      MSM_INVALID = 0,
      MSM_NONE = 1,
      MSM_SIGN = 2,
      MSM_SIGN_AND_ENCRYPT = 3,
    };

    typedef std::vector<uint8_t> CertificateData;

    // TODO Serialization, RawSize
    struct SignatureData
    {
      std::vector<uint8_t> Signature;
      std::string Algorithm;
    };

    enum class ApplicationType : uint32_t
    {
      SERVER = 0,
      CLIENT = 1,
      CLIENT_AND_SERVER = 2,
      DISCOVERY_SERVER = 3,
    };

    struct ApplicationDescription
    {
      std::string URI;
      std::string ProductURI;
      LocalizedText Name;
      ApplicationType Type;    
      std::string GatewayServerURI;
      std::string DiscoveryProfileURI;
      std::vector<std::string> DiscoveryURLs;
    };

    enum class UserIdentifyTokenType : uint32_t
    {
      ANONYMOUS    = 0, // no token required
      USERNAME     = 1, // username/password
      CERTIFICATE  = 2, // x509v3 certificate
      ISSUED_TOKEN = 3, // WS_Security token
    };

    struct UserTokenPolicy
    {
      std::string PolicyID;
      UserIdentifyTokenType TokenType;
      std::string IssuedTokenType;
      std::string IssuerEndpointURL;
      std::string SecurityPolicyURI;
    };

    struct EndpointDescription
    {
      std::string EndpointURL;
      ApplicationDescription ServerDescription;
      CertificateData ServerCertificate;
      MessageSecurityMode SecurityMode;
      std::string SecurityPolicyURI;
      std::vector<UserTokenPolicy> UserIdentifyTokens;
      std::string TransportProfileURI;
      uint8_t SecurityLevel;
    };


    enum ExtensionObjectEncoding : uint8_t
    {
      HAS_BINARY_BODY = 1,
      HAS_XML_BODY    = 2,
    };

    //TODO serialization tests
    struct ExtensionObjectHeader
    {
      NodeID TypeID;
      ExtensionObjectEncoding Encoding;

      ExtensionObjectHeader(ExtensionObjectID objectID, ExtensionObjectEncoding encoding);
    };

  } // namespace Binary
} // namespace OpcUa


#endif  // __OPC_UA_BINARY_COMMON_H__

