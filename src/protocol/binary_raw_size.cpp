/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Sizes of structures serialized form.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <stdexcept>

namespace OpcUa
{
  namespace Binary
  {
    template<>
    std::size_t RawSize<bool>(const bool&)
    {
      return 1;
    }

    template<>
    std::size_t RawSize<uint8_t>(const uint8_t&)
    {
      return 1;
    }

    template<>
    std::size_t RawSize<int8_t>(const int8_t&)
    {
      return 1;
    }

    template<>
    std::size_t RawSize<uint16_t>(const uint16_t&)
    {
      return 2;
    }

    template<>
    std::size_t RawSize<int16_t>(const int16_t&)
    {
      return 2;
    }

    template<>
    std::size_t RawSize<uint32_t>(const uint32_t&)
    {
      return 4;
    }

    template<>
    std::size_t RawSize<int32_t>(const int32_t&)
    {
      return 4;
    }


    template<>
    std::size_t RawSize<uint64_t>(const uint64_t&)
    {
      return 8;
    }

    template<>
    std::size_t RawSize<int64_t>(const int64_t&)
    {
      return 8;
    }

    template<>
    std::size_t RawSize<float>(const float&)
    {
      return 4;
    }

    template<>
    std::size_t RawSize<double>(const double&)
    {
      return 8;
    }

    template<>
    std::size_t RawSize<Guid>(const Guid&)
    {
      return 16;
    }

    template<>
    std::size_t RawSize<StatusCode>(const StatusCode&)
    {
      return 4;
    }

    template<>
    std::size_t RawSize<std::string>(const std::string& str)
    {
      const std::size_t headerSize = 4;
      return headerSize + str.size();
    }

    template<>
    std::size_t RawSize<ByteString>(const ByteString& str)
    {
      const std::size_t headerSize = 4;
      return headerSize + str.Data.size();
    }

    template<>
    std::size_t RawSize<DateTime>(const DateTime& date)
    {
      return RawSize(date.Value);
    }

    template<>
    std::size_t RawSize<std::vector<uint8_t>>(const std::vector<uint8_t>& vec)
    {
      const std::size_t headerSize = 4;
      return headerSize + vec.size();
    }

    template<>
    std::size_t RawSize<std::vector<std::vector<uint8_t>>>(const std::vector<std::vector<uint8_t>>& vec)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(vec.begin(), vec.end(), [&] (const std::vector<uint8_t>& v) {totalSize += RawSize(v);});
      return totalSize;
    }


    template<>
    std::size_t RawSize<std::vector<std::string>>(const std::vector<std::string>& vec)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(vec.begin(), vec.end(), [&] (const std::string& str) {totalSize += RawSize(str);});
      return totalSize;
    }


    template<>
    std::size_t RawSize<Header>(const Header&)
    {
      const std::size_t sizeofType = 3;  // 'H', 'E', 'L'
      const std::size_t sizeofChunk = 1; // 'F'
      const std::size_t sizeofSize = 4;
      return sizeofType + sizeofChunk + sizeofSize;
    }

    template<>
    std::size_t RawSize<SecureHeader>(const SecureHeader&)
    {
      const std::size_t sizeofType = 3;  // 'H', 'E', 'L'
      const std::size_t sizeofChunk = 1; // 'F'
      const std::size_t sizeofSize = 4;
      const std::size_t sizeofChannelId = 4;
      return sizeofType + sizeofChunk + sizeofSize + sizeofChannelId;
    }

    template<>
    std::size_t RawSize<Hello>(const Hello& hello)
    {
      const std::size_t sizeOfProtocolVersion = 4;
      const std::size_t sizeOfReceiveBufferSize = 4;
      const std::size_t sizeOfSendBufferSize = 4;
      const std::size_t sizeOfMaxMessageSize = 4;
      const std::size_t sizeOfMaxChunkCount = 4;

      return sizeOfProtocolVersion +
             sizeOfReceiveBufferSize +
             sizeOfSendBufferSize +
             sizeOfMaxMessageSize +
             sizeOfMaxChunkCount +
             RawSize(hello.EndpointUrl);
    }

    template<>
    std::size_t RawSize<Acknowledge>(const Acknowledge&)
    {
      const std::size_t sizeofProtocolVersion = 4;
      const std::size_t sizeofReceiveBufferSize = 4;
      const std::size_t sizeofSendBufferSize = 4;
      const std::size_t sizeofMaxMessageSize = 4;
      const std::size_t sizeofMaxChunkCount = 4;

      return sizeofProtocolVersion + sizeofReceiveBufferSize + sizeofSendBufferSize + sizeofMaxMessageSize + sizeofMaxChunkCount;
    }

    template<>
    std::size_t RawSize<Error>(const Error& err)
    {
      const std::size_t sizeofCode = 4;
      return sizeofCode + RawSize(err.Reason);
    }

    template<>
    std::size_t RawSize<AsymmetricAlgorithmHeader>(const AsymmetricAlgorithmHeader& ack)
    {
      const std::size_t sizeofUri = RawSize(ack.SecurityPolicyURI);
      const std::size_t sizeofCertificate = RawSize(ack.SenderCertificate);
      const std::size_t sizeofThumbprint = RawSize(ack.ReceiverCertificateThumbPrint);
      return sizeofUri + sizeofCertificate +  sizeofThumbprint;
    }

    template<>
    std::size_t RawSize<SequenceHeader>(const SequenceHeader&)
    {
      const std::size_t sizeofSequenceNumber = 4;
      const std::size_t sizeofRequestId = 4;

      return sizeofSequenceNumber + sizeofRequestId;
    }

    template<>
    std::size_t RawSize<AdditionalHeader>(const AdditionalHeader& header)
    {
      const std::size_t sizeofTypeId = RawSize(header.TypeId);
      const std::size_t sizeofEncoding = 1;
      return sizeofTypeId + sizeofEncoding;
    }

    template<>
    std::size_t RawSize<RequestHeader>(const RequestHeader& header)
    {
      const std::size_t sizeofSessionAuthenticationToken = RawSize(header.SessionAuthenticationToken);
      const std::size_t sizeofUtcTime = 8;
      const std::size_t sizeofRequestHandle = 4;
      const std::size_t sizeofReturnDiagnostics = 4;
      const std::size_t sizeofAuditEntryId = 4 + header.AuditEntryId.size();
      const std::size_t sizeofTimeout = 4;
      const std::size_t sizeofAdditional = RawSize(header.Additional);

      return sizeofSessionAuthenticationToken + sizeofUtcTime + sizeofRequestHandle + sizeofReturnDiagnostics + sizeofAuditEntryId + sizeofTimeout + sizeofAdditional;
    }

    template<>
    std::size_t RawSize<OpenSecureChannelRequest>(const OpenSecureChannelRequest& request)
    {
      const std::size_t sizeofTypeId = RawSize(request.TypeId);
      const std::size_t sizeofHeader = RawSize(request.Header);
      const std::size_t sizeofClientProtocolVersion = 4;
      const std::size_t sizeofRequestType = 4;
      const std::size_t sizeofSecurityMode = 4;
      const std::size_t sizeofClientNonce = 4 + request.Parameters.ClientNonce.size();
      const std::size_t sizeofRequestLifeTime = 4;

      return sizeofTypeId + sizeofHeader + sizeofClientProtocolVersion + sizeofRequestType + sizeofSecurityMode + sizeofClientNonce + sizeofRequestLifeTime;
    };

    template<>
    std::size_t RawSize<SymmetricAlgorithmHeader>(const SymmetricAlgorithmHeader& header)
    {
      const std::size_t sizeofTokenId = 4;
      return sizeofTokenId;
    }

    template<>
    std::size_t RawSize<DiagnosticInfo>(const DiagnosticInfo& info)
    {
      const std::size_t sizeofEncodingMask = 1;
      size_t size = sizeofEncodingMask;

      if (info.EncodingMask & DIM_SYMBOLIC_Id)
      {
        const std::size_t sizeofSymbolicId = 4;
        size += sizeofSymbolicId;
      }
      if (info.EncodingMask & DIM_NAMESPACE)
      {
        const std::size_t sizeofNamespace = 4;
        size += sizeofNamespace;
      }
      if (info.EncodingMask & DIM_LOCALIZED_TEXT)
      {
        const std::size_t sizeofLocalizedText = 4;
        size += sizeofLocalizedText;
      }
      if (info.EncodingMask & DIM_LOCALE)
      {
        const std::size_t sizeofLocale = 4;
        size += sizeofLocale;
      }
      if (info.EncodingMask & DIM_ADDITIONAL_INFO)
      {
        const std::size_t sizeofAdditionalInfo = 4 + info.AdditionalInfo.size();
        size += sizeofAdditionalInfo;
      }
      if (info.EncodingMask & DIM_INNER_STATUS_CODE)
      {
        const std::size_t sizeofInnerStatusCode= 4;
        size += sizeofInnerStatusCode;
      }
      if ((info.EncodingMask & DIM_INNER_DIAGNOSTIC_INFO) && info.InnerDiagnostics)
      {
        size += RawSize(*info.InnerDiagnostics);
      }

      return size;
    }

    template<>
    std::size_t RawSize<DiagnosticInfoList>(const DiagnosticInfoList& infos)
    {
      return RawSizeContainer(infos);
    }

    template<>
    std::size_t RawSize<ResponseHeader>(const ResponseHeader& header)
    {
      const std::size_t sizeofTimestamp = 8;
      const std::size_t sizeofRequestHandle = 4;
      const std::size_t sizeofServiceResult = 4;

      std::size_t sizeofDiagnostics = RawSize(header.InnerDiagnostics);
      std::size_t sizeofStringTable = 4;
      std::for_each(header.StringTable.begin(), header.StringTable.end(), [&] (const std::string& str) {sizeofStringTable += RawSize(str);});

      const std::size_t sizeofAdditional = RawSize(header.Additional);
      return sizeofTimestamp + sizeofRequestHandle + sizeofServiceResult + sizeofDiagnostics + sizeofStringTable + sizeofAdditional;
    }

    template<>
    std::size_t RawSize<SecurityToken>(const SecurityToken&)
    {
      const std::size_t sizeofSecureChannelId = 4;
      const std::size_t sizeofTokenId = 4;
      const std::size_t sizeofCreatedAt = 8;
      const std::size_t sizeofRevisedLifetime = 4;

      return sizeofSecureChannelId + sizeofTokenId + sizeofCreatedAt + sizeofRevisedLifetime;
    };

    template<>
    std::size_t RawSize<OpenSecureChannelResponse>(const OpenSecureChannelResponse& response)
    {
      const std::size_t sizeofTypeId = RawSize(response.TypeId);
      const std::size_t sizeofHeader = RawSize(response.Header);
      const std::size_t sizeofServerProtocolVersion = 4;
      const std::size_t sizeofChannelSecurityToken = RawSize(response.ChannelSecurityToken);
      const std::size_t sizeofServerNonce = 4 + response.ServerNonce.size();

      return sizeofTypeId + sizeofHeader + sizeofServerProtocolVersion + sizeofChannelSecurityToken + sizeofServerNonce;
    };

    template<>
    std::size_t RawSize<CloseSecureChannelRequest>(const CloseSecureChannelRequest& request)
    {
      const std::size_t sizeofTypeId = RawSize(request.TypeId);
      const std::size_t sizeofHeader = RawSize(request.Header);
      return sizeofTypeId + sizeofHeader;
    }


    template<>
    std::size_t RawSize<LocalizedText>(const LocalizedText& text)
    {
      std::size_t size = RawSize(text.Encoding);
      if (text.Encoding & HAS_LOCALE)
      {
        size += RawSize(text.Locale);
      }
      if (text.Encoding & HAS_TEXT)
      {
        size += RawSize(text.Text);
      }
      return size;
    };

    template<>
    std::size_t RawSize<SignatureData>(const SignatureData& s)
    {
      return RawSize(s.Signature) + RawSize(s.Algorithm);
    };

    template<>
    std::size_t RawSize<ExtensionObjectHeader>(const ExtensionObjectHeader& header)
    {
      const std::size_t sizeofEncoding = 1;
      return RawSize(header.TypeId) + sizeofEncoding;
    };

    template<>
    std::size_t RawSize<QualifiedName>(const QualifiedName& name)
    {
      return RawSize(name.NamespaceIndex) + RawSize(name.Name);
    };
    template<>

    std::size_t RawSize<IntegerId>(const IntegerId&)
    {
      return 4;
    };

  }
}

