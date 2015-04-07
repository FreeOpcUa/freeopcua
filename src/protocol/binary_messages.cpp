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
#include <sstream>
#include <chrono>

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
      , ChannelId(0)
    {
      ResetSize();
    }

    SecureHeader::SecureHeader(MessageType type, ChunkType chunk, uint32_t channelId)
      : Type(type)
      , Chunk(chunk)
      , Size(0)
      , ChannelId(channelId)
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
      , RequestId(0)
    {
    }

    SymmetricAlgorithmHeader::SymmetricAlgorithmHeader()
      : TokenId(0)
    {
    }

  } // namespace Binary


  DateTime DateTime::FromTimeT(time_t t, unsigned usec)
  {
    const int64_t daysBetween1601And1970 = 134774;
    const int64_t secsFrom1601To1970 = daysBetween1601And1970 * 24 * 3600LL;
    int64_t t1 = t + secsFrom1601To1970;
    t1 = t1 * 10000000LL;
    t1 += usec * 10;
    return DateTime(t1);
  }

  DateTime DateTime::Current()
  {
    using namespace std::chrono;
    const auto t = time_point<high_resolution_clock>(high_resolution_clock::now());
    const auto us = duration_cast<microseconds>(t.time_since_epoch());
    const auto n = us.count();
    return DateTime::FromTimeT(n / 1000000, n % 1000000);
  }

  time_t DateTime::ToTimeT(DateTime dateTime)
  {
    const int64_t daysBetween1601And1970 = 134774;
    const int64_t secsFrom1601To1970 = daysBetween1601And1970 * 24 * 3600LL;
    if (dateTime.Value < secsFrom1601To1970)
    {
      std::stringstream stream;
      stream << "OpcUa date time cannot be less than " << secsFrom1601To1970;
      throw std::invalid_argument(stream.str());
    }
    const int64_t secsFrom1970 = dateTime.Value / 10000000LL - secsFrom1601To1970;
    return secsFrom1970;
  }


  RequestHeader::RequestHeader()
  {
    SessionAuthenticationToken.Encoding = EV_TWO_BYTE;
    SessionAuthenticationToken.TwoByteData.Identifier = 0;
    UtcTime = DateTime::Current();
    RequestHandle = 0;
    ReturnDiagnostics = 0;
    AuditEntryId = "";
    Timeout = 0; // in miliseconds
    Additional.TypeId.Encoding = EV_TWO_BYTE;
    Additional.TypeId.TwoByteData.Identifier = 0;
  }

  OpenSecureChannelParameters::OpenSecureChannelParameters()
    : ClientProtocolVersion(0)
    , RequestType(SecurityTokenRequestType::Issue)
    , SecurityMode(MessageSecurityMode::None)
    , RequestLifeTime(300000)
  {
  }

  OpenSecureChannelRequest::OpenSecureChannelRequest()
    : TypeId(OPEN_SECURE_CHANNEL_REQUEST)
  {
  }

  ResponseHeader::ResponseHeader()
    : Timestamp(DateTime::Current())
    , RequestHandle(0)
    , ServiceResult(StatusCode::Good)
  {
  }

  OpenSecureChannelResponse::OpenSecureChannelResponse()
    : TypeId(OPEN_SECURE_CHANNEL_RESPONSE)
    , ServerProtocolVersion(0)
  {
  }

  CloseSecureChannelRequest::CloseSecureChannelRequest()
    : TypeId(OpcUa::CLOSE_SECURE_CHANNEL_REQUEST)
  {
  }

} // namespace OpcUa

