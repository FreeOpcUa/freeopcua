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

#include <opc/ua/protocol/enums.h>
#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/nodeid.h>
#include <opc/ua/protocol/datetime.h>
#include <opc/ua/protocol/status_codes.h>
#include <opc/ua/protocol/reference_ids.h>

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include <atomic>


namespace OpcUa
{

  typedef std::string LocaleId;

  struct ByteString
  {
    std::vector<uint8_t> Data;

    ByteString()
    {
    }

    explicit ByteString(const std::vector<uint8_t>& val)
      : Data(val)
    {
    }

    bool operator== (const ByteString& str) const
    {
      return Data == str.Data;
    }
  };

  class IntegerId
  {
  public:
    IntegerId();
    IntegerId(const IntegerId& id);
    explicit IntegerId(uint32_t num);
    IntegerId& operator= (const IntegerId& id);
    IntegerId& operator= (uint32_t value);
    operator uint32_t() const;

  private:
    uint32_t Value;
  };

  struct QualifiedName
  {
    uint16_t NamespaceIndex;
    std::string Name; // TODO rename to Text

    QualifiedName()
      : NamespaceIndex(0)
    {
    }

    QualifiedName(uint16_t nameSpace, const std::string& name)
      : NamespaceIndex(nameSpace)
      , Name(name)
    {
    }

    QualifiedName(const std::string& name, uint16_t nameSpace)
      : NamespaceIndex(nameSpace)
      , Name(name)
    {
    }

    explicit QualifiedName(const std::string& name)
      : NamespaceIndex(0)
      , Name(name)
    {
    }

    bool operator== (const QualifiedName& name) const
    {
      return NamespaceIndex == name.NamespaceIndex && Name == name.Name;
    }

    bool operator < (const QualifiedName& name) const
    {
      if (NamespaceIndex != name.NamespaceIndex)
      {
        return NamespaceIndex < name.NamespaceIndex;
      }
      return Name < name.Name;
    }
  };

  struct RelativePathElement
  {
    NodeId ReferenceTypeId;
    bool IsInverse = false;
    bool IncludeSubtypes = false;
    QualifiedName TargetName;
  };

  struct RelativePath
  {
    std::vector<RelativePathElement> Elements;
  };

  // LocalizedText encoding mask
  const uint8_t HAS_LOCALE = 1;
  const uint8_t HAS_TEXT = 2;

  struct LocalizedText
  {
    uint8_t Encoding;
    std::string Locale;
    std::string Text; // TODO rename to Data

    LocalizedText()
      : Encoding(0)
    {
    }

    explicit LocalizedText(const std::string& text)
      : Encoding(HAS_TEXT)
      , Text(text)
    {
    }

    explicit LocalizedText(const char* text)
      : Encoding(HAS_TEXT)
      , Text(text)
    {
    }

    LocalizedText(const std::string& text, const std::string& locale)
      : Encoding(HAS_TEXT | HAS_LOCALE)
      , Locale(locale)
      , Text(text)
    {
    }

    LocalizedText(const char* text, char* locale)
      : Encoding(HAS_TEXT | HAS_LOCALE)
      , Locale(locale)
      , Text(text)
    {
    }

    bool operator== (const LocalizedText& text) const
    {
      return Encoding == text.Encoding && Locale == text.Locale && Text == text.Text;
    }
  };

  struct AdditionalHeader
  {
    ExpandedNodeId TypeId;
    uint8_t Encoding;

    AdditionalHeader()
      : Encoding(0)
    {
    }
  };

  struct RequestHeader
  {
    ExpandedNodeId SessionAuthenticationToken;
    DateTime UtcTime;
    uint32_t RequestHandle = 0;
    uint32_t ReturnDiagnostics = 0;
    std::string AuditEntryId;
    uint32_t Timeout = 0; // in miliseconds
    AdditionalHeader Additional;

    RequestHeader();
  };


  enum DiagnosticInfoMask : uint8_t
  {
    DIM_NONE                  = 0,
    DIM_SYMBOLIC_Id           = 0x1,
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
    int32_t SymbolicId;
    int32_t NamespaceURI;
    int32_t LocalizedText;
    int32_t Locale;
    std::string AdditionalInfo;
    StatusCode InnerStatusCode;
    std::shared_ptr<DiagnosticInfo> InnerDiagnostics;

    DiagnosticInfo()
      : EncodingMask(DiagnosticInfoMask::DIM_NONE)
      , SymbolicId(0)
      , NamespaceURI(0)
      , LocalizedText(0)
      , Locale(0)
      , InnerStatusCode(StatusCode::Good)
    {
    }

    bool operator== (const DiagnosticInfo& info) const
    {
      if (
        EncodingMask == info.EncodingMask &&
        SymbolicId == info.SymbolicId &&
        NamespaceURI == info.NamespaceURI &&
        LocalizedText == info.LocalizedText &&
        Locale == info.Locale &&
        InnerStatusCode == info.InnerStatusCode)
      {
        if (InnerDiagnostics && info.InnerDiagnostics)
          return *InnerDiagnostics == *info.InnerDiagnostics;

        return !InnerDiagnostics && !info.InnerDiagnostics;
      }
      return false;
    }
  };

  typedef std::vector<DiagnosticInfo> DiagnosticInfoList;

  struct ResponseHeader
  {
    DateTime Timestamp;
    uint32_t RequestHandle = 0;
    StatusCode ServiceResult = StatusCode::Good;
    DiagnosticInfo InnerDiagnostics;
    std::vector<std::string> StringTable;
    AdditionalHeader Additional;

    ResponseHeader();
  };

  typedef std::vector<uint8_t> CertificateData;

  // TODO Serialization, RawSize
  struct SignatureData
  {
    std::vector<uint8_t> Signature;
    std::string Algorithm;
  };

  struct ApplicationDescription
  {
    std::string URI;
    std::string ProductURI;
    LocalizedText Name;
    ApplicationType Type = ApplicationType::Client;
    std::string GatewayServerURI;
    std::string DiscoveryProfileURI;
    std::vector<std::string> DiscoveryURLs;

    ApplicationDescription();
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
    std::string PolicyId;
    UserIdentifyTokenType TokenType = UserIdentifyTokenType::ANONYMOUS;
    std::string IssuedTokenType;
    std::string IssuerEndpointURL;
    std::string SecurityPolicyURI;
  };

  struct EndpointDescription
  {
    std::string EndpointURL;
    ApplicationDescription ServerDescription;
    CertificateData ServerCertificate;
    MessageSecurityMode SecurityMode = MessageSecurityMode::None;
    std::string SecurityPolicyURI;
    std::vector<UserTokenPolicy> UserIdentifyTokens;
    std::string TransportProfileURI;
    uint8_t SecurityLevel = 0;
  };


  enum ExtensionObjectEncoding : uint8_t
  {
    NONE = 0,
    HAS_BINARY_BODY = 1,
    HAS_XML_BODY    = 2,
  };

  //TODO serialization tests
  struct ExtensionObjectHeader
  {
    ExpandedNodeId TypeId;
    ExtensionObjectEncoding Encoding;

    ExtensionObjectHeader();
    ExtensionObjectHeader(ExtensionObjectId objectId, ExtensionObjectEncoding encoding);
  };

} // namespace OpcUa

#endif // __OPC_UA_MAPPING_TYPES_H__

