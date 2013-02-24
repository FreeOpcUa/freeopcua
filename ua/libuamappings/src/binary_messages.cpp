/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary messages.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/binary/protocol/common.h>
#include <opc/ua/binary/protocol/secure_channel.h>

#include <algorithm>
#include <stdexcept>
#include <sys/time.h>

namespace OpcUa
{
  namespace Binary
  {
    DateTime CurrentDateTime()
    {
      static const OpcUa::Binary::DateTime epochDiff =  11644473600LL;
      timeval tv;
      OpcUa::Binary::DateTime t = epochDiff;
      gettimeofday(&tv, 0);
      t += tv.tv_sec;
      t *= 10000000LL;
      t += tv.tv_usec * 10;
      return t;
    }

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

    NodeID::NodeID()
      : Encoding(EV_TWO_BYTE)
      , ServerIndex(0)
    {
    }

    NodeID::NodeID(MessageID messageID)
      : Encoding(EV_FOUR_BYTE)
      , ServerIndex(0)
    {
      FourByteData.Identifier = messageID;
    }


    SequenceHeader::SequenceHeader()
      : SequenceNumber(0)
      , RequestID(0)
    {
    }


    RequestHeader::RequestHeader()
    {
      SessionAuthenticationToken.Encoding = OpcUa::Binary::EV_TWO_BYTE;
      SessionAuthenticationToken.TwoByteData.Identifier = 0;
      UtcTime = CurrentDateTime();
      RequestHandle = 0;
      ReturnDiagnostics = 0;
      AuditEntryID = "";
      Timeout = 0; // in miliseconds
      Additional.TypeID.Encoding = OpcUa::Binary::EV_TWO_BYTE;
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

    SymmetricAlgorithmHeader::SymmetricAlgorithmHeader()
      : TokenID(0)
    {
    }

    ResponseHeader::ResponseHeader()
      : Timestamp(CurrentDateTime())
      , RequestHandle(0)
      , ServiceResult(0)
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


  } // namespace Binary
} // namespace OpcUa

