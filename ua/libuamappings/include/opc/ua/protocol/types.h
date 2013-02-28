/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief C++ types of binary protocol.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_MAPPING_TYPES_H__
#define __OPC_UA_MAPPING_TYPES_H__

#include <opc/ua/protocol/datetime.h>
#include <opc/ua/message_identifiers.h>
#include <opc/ua/object_ids.h>
#include <opc/ua/extension_identifiers.h>

#include <stdint.h>
#include <string>
#include <vector>

namespace OpcUa
{

  typedef uint32_t StatusCode;

  struct Guid
  {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];

    Guid()
      : Data1(0)
      , Data2(0)
      , Data3(0)
    {
      for (unsigned i = 0; i < 8; ++i)
      {
        Data4[i] = 0;
      }
    }
  };

  inline bool operator== (const Guid& l, const Guid&r)
  {
    for (unsigned i = 0; i < 8; ++i)
    {
      if (l.Data4[i] != r.Data4[i])
      {
        return false;
      }
    }

    return l.Data1 == r.Data1 && l.Data2 == r.Data2 && l.Data3 == r.Data3;
  }

  struct QualifiedName
  {
    uint16_t NamespaceIndex;
    std::string Name;

    QualifiedName()
      : NamespaceIndex(0)
    {
    }
  };

  // LocalizedText encoding mask
  const uint8_t HAS_LOCALE = 1;
  const uint8_t HAS_TEXT = 2;

  struct LocalizedText
  {
    uint8_t Encoding;
    std::string Locale;
    std::string Text;
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

} // namespace OpcUa

#endif // __OPC_UA_MAPPING_TYPES_H__

