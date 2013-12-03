/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary messages.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <stdexcept>
#include <sys/time.h>

namespace OpcUa
{
  // TODO move all in binary namespace to the binary_common.h
  namespace Binary
  {
    Header::Header()
      : Type(MT_INVALID)
      , Chunk(CHT_INVALID)
      , Size(0)
    {
      ResetSize();
    }

    Header::Header(MessageType type, ChunkType chunk)
      : Type(type)
      , Chunk(chunk)
      , Size(0)
    {
      ResetSize();
    }

    std::size_t Header::AddSize(std::size_t size)
    {
      Size += size;
      return Size;
    };


    std::size_t Header::MessageSize() const
    {
      return Size - RawSize(*this);
    }

    void Header::ResetSize()
    {
      Size = RawSize(*this);
    }

    SecureHeader::SecureHeader()
      : Type(MT_INVALID)
      , Chunk(CHT_INVALID)
      , Size(0)
      , ChannelID(0)
    {
      ResetSize();
    }

    SecureHeader::SecureHeader(MessageType type, ChunkType chunk, uint32_t channelID)
      : Type(type)
      , Chunk(chunk)
      , Size(0)
      , ChannelID(channelID)
    {
      ResetSize();
    }

    std::size_t SecureHeader::AddSize(std::size_t size)
    {
      Size += size;
      return Size;
    };


    std::size_t SecureHeader::MessageSize() const
    {
      return Size - RawSize(*this);
    }

    void SecureHeader::ResetSize()
    {
      Size = RawSize(*this);
    }


    Hello::Hello()
      : ProtocolVersion(0)
      , ReceiveBufferSize(0)
      , SendBufferSize(0)
      , MaxMessageSize(0)
      , MaxChunkCount(0)
    {
    }


    Acknowledge::Acknowledge()
      : ProtocolVersion(0)
      , ReceiveBufferSize(0)
      , SendBufferSize(0)
      , MaxMessageSize(0)
      , MaxChunkCount(0)
    {
    }

    Error::Error()
      : Code(0)
    {
    }

    SequenceHeader::SequenceHeader()
      : SequenceNumber(0)
      , RequestID(0)
    {
    }

    SymmetricAlgorithmHeader::SymmetricAlgorithmHeader()
      : TokenID(0)
    {
    }

  } // namespace Binary

  DateTime ToDateTime(time_t t, unsigned usec)
  {
    const int64_t secsFrom1600To1970 = 11676096000LL;
    int64_t t1 = t + secsFrom1600To1970;
    t1 = t1 * 10000000LL;
    t1 += usec * 10;
    return DateTime(t1);
  }

  // TODO move to separate file with time utils.
  DateTime CurrentDateTime()
  {
    timeval tv;
    gettimeofday(&tv, 0);
    return ToDateTime(tv.tv_sec, tv.tv_usec);
  }

  time_t ToTimeT(DateTime dateTime)
  {
    const int64_t secsFrom1600To1970 = 11676096000LL;
    const int64_t secsFrom1970 = dateTime.Value / 10000000LL - secsFrom1600To1970;
    return secsFrom1970;
  }

  std::string ToString(const Guid& guid)
  {
    char buf[32] = {0};
    sprintf(buf, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return buf;
  }

  Guid ToGuid(const std::string& str)
  {
    if (str.size() != 35)
    {
      return OpcUa::Guid();
    }
    Guid guid;
    const int parts = sscanf(str.c_str(), "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X", &guid.Data1, &guid.Data2, &guid.Data3, &guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3], &guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);
    if (parts != 11)
    {
      return OpcUa::Guid();
    }
    return guid;
  }

  RequestHeader::RequestHeader()
  {
    SessionAuthenticationToken.Encoding = EV_TWO_BYTE;
    SessionAuthenticationToken.TwoByteData.Identifier = 0;
    UtcTime = CurrentDateTime();
    RequestHandle = 0;
    ReturnDiagnostics = 0;
    AuditEntryID = "";
    Timeout = 0; // in miliseconds
    Additional.TypeID.Encoding = EV_TWO_BYTE;
    Additional.TypeID.TwoByteData.Identifier = 0;
  }

  OpenSecureChannelRequest::OpenSecureChannelRequest()
    : TypeID(OPEN_SECURE_CHANNEL_REQUEST)
    , ClientProtocolVersion(0)
    , RequestType(STR_ISSUE)
    , SecurityMode(MSM_INVALID)
    , RequestLifeTime(0)
  {
  }

  ResponseHeader::ResponseHeader()
    : Timestamp(CurrentDateTime())
    , RequestHandle(0)
    , ServiceResult(StatusCode::Good)
  {
  }

  OpenSecureChannelResponse::OpenSecureChannelResponse()
    : TypeID(OPEN_SECURE_CHANNEL_RESPONSE)
    , ServerProtocolVersion(0)
  {
  }

  CloseSecureChannelRequest::CloseSecureChannelRequest()
    : TypeID(OpcUa::CLOSE_SECURE_CHANNEL_REQUEST)
  {
  }

} // namespace OpcUa

