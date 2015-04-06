/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/binary/stream.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <netinet/in.h>


namespace
{

  float float_htonl(float value){
    std::cout << "sizez: " << sizeof(float) << "  " << sizeof(uint32_t) << std::endl;
    union v {
        float       f;
        uint32_t    i;
    };
     
    union v val;
     
    val.f = value;
    val.i = htonl(val.i);
                   
    return val.f;
  }

  float float_ntohl(float value){
    union v {
        float       f;
        uint32_t    i;
    };
     
    union v val;
     
    val.f = value;
    val.i = ntohl(val.i);
                   
    return val.f;
  }


  template <typename Integer16>
  inline int8_t LoByte(Integer16 value)
  {
    return value & 0xFF;
  }

  template <typename Integer16>
  inline int8_t HiByte(Integer16 value)
  {
    return (value & 0xFF00) >> 8;
  }

  template <typename Integer32>
  inline uint16_t LoWord(Integer32 value)
  {
    return value & 0xFFFF;
  }

  template <typename Integer32>
  inline uint16_t HiWord(Integer32 value)
  {
    return (value & 0xFFFF0000) >> 16;
  }

  template <typename Integer64>
  inline uint32_t LoDWord(Integer64 value)
  {
    return value & 0xFFFFFFFF;
  }

  template <typename Integer64>
  inline uint32_t HiDWord(Integer64 value)
  {
    return (value & 0xFFFFFFFF00000000) >> 32;
  }

  template <typename Integer16>
  inline Integer16  MakeWord(int8_t loByte, int8_t hiByte)
  {
    const Integer16 word = hiByte;
    return (word << 8) | (0x00FF & loByte);
  }

  template<typename Integer>
  inline Integer MakeNumber(char* data)
  {
    unsigned size = sizeof(Integer);
    Integer i = 0;
    while(size)
    {
      i = (i << 8) | (0x00FF & data[--size]);
    }
    return i;
  }


  const size_t MESSAGE_TYPE_SIZE = 3;
  const char MESSAGE_TYPE_HELLO[MESSAGE_TYPE_SIZE]       = {'H', 'E', 'L'};
  const char MESSAGE_TYPE_ACKNOWLEDGE[MESSAGE_TYPE_SIZE] = {'A', 'C', 'K'};
  const char MESSAGE_TYPE_ERROR[MESSAGE_TYPE_SIZE]       = {'E', 'R', 'R'};
  const char MESSAGE_TYPE_MESSAGE[MESSAGE_TYPE_SIZE]     = {'M', 'S', 'G'};
  const char MESSAGE_TYPE_OPEN[MESSAGE_TYPE_SIZE]        = {'O', 'P', 'N'};
  const char MESSAGE_TYPE_CLOSE[MESSAGE_TYPE_SIZE]       = {'C', 'L', 'O'};

  inline void ThrowReceivedNotEnoughData()
  {
    throw std::logic_error("Not enough data was received from channel.");
  }


  template <typename ChannelType>
  inline void GetData(ChannelType& channel, char* data, std::size_t size)
  {
    size_t recv = channel.Read(data, size);
    if (recv != size)
    {
      std::cout << "expecting " << size << "  received: " << recv << std::endl;
      ThrowReceivedNotEnoughData();
    }
  }


} // namespace


namespace OpcUa
{
  ExtensionObjectHeader::ExtensionObjectHeader()
    : Encoding(ExtensionObjectEncoding::NONE)
  {
  }

  /// TODO move to apropriate file
  ExtensionObjectHeader::ExtensionObjectHeader(ExtensionObjectId objectId, ExtensionObjectEncoding encoding)
    : Encoding(encoding)
  {
    TypeId.Encoding = EV_FOUR_BYTE;
    TypeId.FourByteData.Identifier = objectId;
  }
  ///////////////////////////////////////////////////////
  // IntegerId
  ///////////////////////////////////////////////////////

  IntegerId::IntegerId()
    : Value(1)
  {
  }

  IntegerId::IntegerId(const IntegerId& id)
    : Value(id.Value)
  {
  }

  IntegerId::IntegerId(uint32_t num)
    : Value(num)
  {
    if (!Value)
    {
      throw std::invalid_argument("IntegerId cannot be zero");
    }
  }

  IntegerId& IntegerId::operator= (const IntegerId& id)
  {
    Value = id.Value;
    return *this;
  }

  IntegerId& IntegerId::operator= (uint32_t value)
  {
    if (!Value)
    {
      throw std::invalid_argument("IntegerId cannot be zero");
    }

    Value = value;
    return *this;
  }

  IntegerId::operator uint32_t() const
  {
    return Value;
  }

  ///////////////////////////////////////////////////////

  namespace Binary
  {

    template<>
    void DataSerializer::Serialize<int8_t>(const int8_t& value)
    {
      Buffer.push_back(value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<int8_t>>(const std::vector<int8_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<uint8_t>(const uint8_t& value)
    {
      Buffer.push_back(value);
    }

    template<>
    void DataDeserializer::Deserialize<uint8_t>(uint8_t& value)
    {
      char data = 0;
      GetData(In, &data, 1);
      value = static_cast<uint8_t>(data);
    }

    template<>
    void DataDeserializer::Deserialize<int8_t>(int8_t& value)
    {
      char data = 0;
      GetData(In, &data, 1);
      value = data;
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<int8_t>>(std::vector<int8_t>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<int16_t>(const int16_t& value)
    {
      Buffer.push_back(LoByte(value));
      Buffer.push_back(HiByte(value));
    }

    template<>
    void DataSerializer::Serialize<uint16_t>(const uint16_t& value)
    {
      Buffer.push_back(LoByte(value));
      Buffer.push_back(HiByte(value));
    }

    template<>
    void DataDeserializer::Deserialize<uint16_t>(uint16_t& value)
    {
      char data[2] = {0};
      GetData(In, data, 2);
      value = MakeWord<uint16_t>(data[0], data[1]);
    }

    template<>
    void DataDeserializer::Deserialize<int16_t>(int16_t& value)
    {
      char data[2] = {0};
      GetData(In, data, 2);
      value = MakeWord<int16_t>(data[0], data[1]);
    }

    template<>
    void DataSerializer::Serialize<int32_t>(const int32_t& value)
    {
      Serialize(LoWord(value));
      Serialize(HiWord(value));
    }

    template<>
    void DataSerializer::Serialize<uint32_t>(const uint32_t& value)
    {
      Serialize(LoWord(value));
      Serialize(HiWord(value));
    }

    template<>
    void DataDeserializer::Deserialize<uint32_t>(uint32_t& value)
    {
      char data[4] = {0};
      GetData(In, data, 4);
      value = MakeNumber<uint32_t>(data);
    }

    template<>
    void DataDeserializer::Deserialize<int32_t>(int32_t& value)
    {
      char data[4] = {0};
      GetData(In, data, 4);
      value = MakeNumber<int32_t>(data);
    }

    template<>
    void DataSerializer::Serialize<int64_t>(const int64_t& value)
    {
      Serialize(LoDWord(value));
      Serialize(HiDWord(value));
    }

    template<>
    void DataSerializer::Serialize<uint64_t>(const uint64_t& value)
    {
      Serialize(LoDWord(value));
      Serialize(HiDWord(value));
    }

    template<>
    void DataDeserializer::Deserialize<uint64_t>(uint64_t& value)
    {
      char data[8] = {0};
      GetData(In, data, 8);
      value = MakeNumber<uint64_t>(data);
    }

    template<>
    void DataDeserializer::Deserialize<int64_t>(int64_t& value)
    {
      char data[8] = {0};
      GetData(In, data, 8);
      value = MakeNumber<int64_t>(data);
    }

    template<>
    void DataSerializer::Serialize<bool>(const bool& value)
    {
      Serialize(static_cast<uint8_t>(value));
    }

    template<>
    void DataSerializer::Serialize<std::vector<bool>>(const std::vector<bool>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<bool>(bool& value)
    {
      uint8_t tmp = 0;
      *this >> tmp;
      value = (tmp != 0);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<bool>>(std::vector<bool>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<float>(const float& value)
    {
      //float network_value = float_htonl(value); 
      const uint8_t* data = reinterpret_cast<const uint8_t*>(&value);
      for (int i = 0; i < 4; ++i)
      {
        Serialize(data[i]);
      }
    }

    template<>
    void DataDeserializer::Deserialize<float>(float& value)
    {
      //float network_value = float_htonl(value); 
      uint8_t data[4] = {0};
      for (int i = 0; i < 4; ++i)
      {
        *this >> data[i];
      }
      value = *reinterpret_cast<const float*>(data); // FIXME: probably broken on ARM
     }

    template<>
    void DataSerializer::Serialize<double>(const double& value)
    {
      const uint8_t* data = reinterpret_cast<const uint8_t*>(&value);
      for (int i = 0; i < 8; ++i)
      {
        Serialize(data[i]);
      }
    }

    template<>
    void DataDeserializer::Deserialize<double>(double& value)
    {
      uint8_t data[8] = {0};
      for (int i = 0; i < 8; ++i)
      {
        *this >> data[i];
      }
      value = *reinterpret_cast<const double*>(data); //FIXME: probably broken on ARM
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Guid>(const OpcUa::Guid& value)
    {
      *this << value.Data1 << value.Data2 << value.Data3;
      Buffer.insert(Buffer.end(), value.Data4, value.Data4 + 8);
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Guid>(OpcUa::Guid& value)
    {
      *this >> value.Data1 >> value.Data2 >> value.Data3;
      char data[8] = {0};
      GetData(In, data, 8);
      std::copy(data, data + 8, value.Data4);
    }

    template<>
    void DataSerializer::Serialize<std::string>(const std::string& value)
    {
      if (value.empty())
      {
        Serialize(~uint32_t());
        return;
      }
      Serialize(static_cast<uint32_t>(value.size()));
      Buffer.insert(Buffer.end(), value.begin(), value.end());
    }

    template<>
    void DataDeserializer::Deserialize<std::string>(std::string& value)
    {
      uint32_t stringSize = 0;
      *this >> stringSize;
      if (stringSize != ~uint32_t())
      {
        value.resize(stringSize);
        GetData(In, &value[0], stringSize);
        return;
      }

      value.clear();
      return;
     // TODO standard says that 0xff*4 - is the zero byte string , actually it seems that it is an empty string
/*
      while(true)
      {
        uint8_t val = 0;
        *this >> val;
        if (val == 0)
        {
          return;
        }
        value.push_back(val);
      }
*/
    }


    template<>
    void DataSerializer::Serialize<OpcUa::DateTime>(const OpcUa::DateTime& date)
    {
      *this << date.Value;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::DateTime>(OpcUa::DateTime& date)
    {
      *this >> date.Value;
    }

    template<>
    void DataSerializer::Serialize<std::vector<OpcUa::DateTime>>(const std::vector<OpcUa::DateTime>& date)
    {
      SerializeContainer(*this, date);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<OpcUa::DateTime>>(std::vector<OpcUa::DateTime>& date)
    {
      DeserializeContainer(*this, date);
    }

    template<>
    void DataSerializer::Serialize<ByteString>(const ByteString& value)
    {
      if (value.Data.empty())
      {
        Serialize(~uint32_t());
        return;
      }
      Serialize(static_cast<uint32_t>(value.Data.size()));
      Buffer.insert(Buffer.end(), value.Data.begin(), value.Data.end());
    }

    template<>
    void DataDeserializer::Deserialize<ByteString>(ByteString& value)
    {
      uint32_t stringSize = 0;
      *this >> stringSize;
      if (stringSize != ~uint32_t())
      {
        value.Data.resize(stringSize);
        GetData(In, reinterpret_cast<char*>(&value.Data[0]), stringSize);
        return;
      }

      value.Data.clear();
      return;
    }


    template<>
    void DataSerializer::Serialize<std::vector<ByteString>>(const std::vector<ByteString>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<ByteString>>(std::vector<ByteString>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<std::vector<uint8_t>>(const std::vector<uint8_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<uint8_t>>(std::vector<uint8_t>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<uint16_t>>(const std::vector<uint16_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<uint16_t>>(std::vector<uint16_t>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<int16_t>>(const std::vector<int16_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<int16_t>>(std::vector<int16_t>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<std::vector<uint32_t>>(const std::vector<uint32_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<uint32_t>>(std::vector<uint32_t>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<int32_t>>(const std::vector<int32_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<int32_t>>(std::vector<int32_t>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<int64_t>>(const std::vector<int64_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<int64_t>>(std::vector<int64_t>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<uint64_t>>(const std::vector<uint64_t>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<uint64_t>>(std::vector<uint64_t>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<std::vector<float>>(const std::vector<float>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<double>>(std::vector<double>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<double>>(const std::vector<double>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<float>>(std::vector<float>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<std::vector<Guid>>(const std::vector<Guid>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<Guid>>(std::vector<Guid>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<NodeId>>(const std::vector<NodeId>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<NodeId>>(std::vector<NodeId>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<std::string>>(const std::vector<std::string>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<std::string>>(std::vector<std::string>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<std::vector<uint8_t>>>(const std::vector<std::vector<uint8_t>>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<std::vector<uint8_t>>>(std::vector<std::vector<uint8_t>>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::MessageType>(const OpcUa::Binary::MessageType& value)
    {
      const char* typeName = nullptr;
      switch(value)
      {
        case MT_HELLO:
          typeName = MESSAGE_TYPE_HELLO;
          break;
        case MT_ACKNOWLEDGE:
          typeName = MESSAGE_TYPE_ACKNOWLEDGE;
          break;
        case MT_ERROR:
          typeName = MESSAGE_TYPE_ERROR;
          break;
        case MT_SECURE_OPEN:
          typeName = MESSAGE_TYPE_OPEN;
          break;
        case MT_SECURE_CLOSE:
          typeName = MESSAGE_TYPE_CLOSE;
          break;
        case MT_SECURE_MESSAGE:
          typeName = MESSAGE_TYPE_MESSAGE;
          break;
        default:
          throw std::logic_error("Invalid message type.");
      }
      Buffer.insert(Buffer.end(), typeName, typeName + MESSAGE_TYPE_SIZE);
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::MessageType>(OpcUa::Binary::MessageType& value)
    {
      char data[MESSAGE_TYPE_SIZE] = {0};
      GetData(In, data, MESSAGE_TYPE_SIZE);
      if (std::equal(data, data + MESSAGE_TYPE_SIZE, MESSAGE_TYPE_HELLO))
      {
        value = OpcUa::Binary::MT_HELLO;
      }
      else if (std::equal(data, data + MESSAGE_TYPE_SIZE, MESSAGE_TYPE_ACKNOWLEDGE))
      {
        value = OpcUa::Binary::MT_ACKNOWLEDGE;
      }
      else if (std::equal(data, data + MESSAGE_TYPE_SIZE, MESSAGE_TYPE_ERROR))
      {
        value = OpcUa::Binary::MT_ERROR;
      }
      else if (std::equal(data, data + MESSAGE_TYPE_SIZE, MESSAGE_TYPE_MESSAGE))
      {
        value = OpcUa::Binary::MT_SECURE_MESSAGE;
      }
      else if (std::equal(data, data + MESSAGE_TYPE_SIZE, MESSAGE_TYPE_OPEN))
      {
        value = OpcUa::Binary::MT_SECURE_OPEN;
      }
      else if (std::equal(data, data + MESSAGE_TYPE_SIZE, MESSAGE_TYPE_CLOSE))
      {
        value = OpcUa::Binary::MT_SECURE_CLOSE;
      }
      else
      {
        std::string msg("Cannot deserialize Unknown message type [");
        msg += std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]);
        msg += "] received.";
        throw std::logic_error(msg);
      }
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::ChunkType>(const OpcUa::Binary::ChunkType& value)
    {
      switch (value)
      {
        case CHT_SINGLE:
          Buffer.push_back('F');
          break;
        case CHT_INTERMEDIATE:
          Buffer.push_back('C');
          break;
        case CHT_FINAL:
          Buffer.push_back('A');
          break;
        default:
          throw std::logic_error("Invalid Chunk Type");
      }
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::ChunkType>(OpcUa::Binary::ChunkType& value)
    {
      char data = 0;
      GetData(In, &data, 1);
      switch (data)
      {
        case 'F':
          value = CHT_SINGLE;
          break;

        case 'C':
          value = CHT_INTERMEDIATE;
          break;

        case 'A':
          value = CHT_FINAL;
          break;

        default:
          throw std::logic_error("Invalid chunk type received.");
      };
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::Header>(const OpcUa::Binary::Header& header)
    {
      *this << header.Type;
      *this << header.Chunk;
      *this << header.Size;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::Header>(OpcUa::Binary::Header& header)
    {
      *this >> header.Type;
      *this >> header.Chunk;
      *this >> header.Size;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::Hello>(const OpcUa::Binary::Hello& message)
    {
      *this << message.ProtocolVersion;
      *this << message.ReceiveBufferSize;
      *this << message.SendBufferSize;
      *this << message.MaxMessageSize;
      *this << message.MaxChunkCount;
      *this << message.EndpointUrl;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::Hello>(OpcUa::Binary::Hello& message)
    {
      *this >> message.ProtocolVersion;
      *this >> message.ReceiveBufferSize;
      *this >> message.SendBufferSize;
      *this >> message.MaxMessageSize;
      *this >> message.MaxChunkCount;
      *this >> message.EndpointUrl;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::Acknowledge>(const OpcUa::Binary::Acknowledge& message)
    {
      *this << message.ProtocolVersion;
      *this << message.ReceiveBufferSize;
      *this << message.SendBufferSize;
      *this << message.MaxMessageSize;
      *this << message.MaxChunkCount;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::Acknowledge>(OpcUa::Binary::Acknowledge& message)
    {
      *this >> message.ProtocolVersion;
      *this >> message.ReceiveBufferSize;
      *this >> message.SendBufferSize;
      *this >> message.MaxMessageSize;
      *this >> message.MaxChunkCount;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::Error>(const OpcUa::Binary::Error& message)
    {
      *this << message.Code;
      *this << message.Reason;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::Error>(OpcUa::Binary::Error& message)
    {
      *this >> message.Code;
      *this >> message.Reason;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::SecureHeader>(const OpcUa::Binary::SecureHeader& header)
    {
      *this << header.Type;
      *this << header.Chunk;
      *this << header.Size;
      *this << header.ChannelId;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::SecureHeader>(OpcUa::Binary::SecureHeader& header)
    {
      *this >> header.Type;
      *this >> header.Chunk;
      *this >> header.Size;
      *this >> header.ChannelId;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::AsymmetricAlgorithmHeader>(const OpcUa::Binary::AsymmetricAlgorithmHeader& header)
    {
      *this << header.SecurityPolicyURI;
      *this << header.SenderCertificate;
      *this << header.ReceiverCertificateThumbPrint;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::AsymmetricAlgorithmHeader>(OpcUa::Binary::AsymmetricAlgorithmHeader& header)
    {
      *this >> header.SecurityPolicyURI;
      *this >> header.SenderCertificate;
      *this >> header.ReceiverCertificateThumbPrint;
    };

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::SymmetricAlgorithmHeader>(const OpcUa::Binary::SymmetricAlgorithmHeader& header)
    {
      *this << header.TokenId;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::SymmetricAlgorithmHeader>(OpcUa::Binary::SymmetricAlgorithmHeader& header)
    {
      *this >> header.TokenId;
    };


    template<>
    void DataSerializer::Serialize<OpcUa::Binary::SequenceHeader>(const OpcUa::Binary::SequenceHeader& header)
    {
      *this << header.SequenceNumber;
      *this << header.RequestId;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::SequenceHeader>(OpcUa::Binary::SequenceHeader& header)
    {
      *this >> header.SequenceNumber;
      *this >> header.RequestId;
    };


    template<>
    void DataSerializer::Serialize<OpcUa::AdditionalHeader>(const OpcUa::AdditionalHeader& header)
    {
      *this << header.TypeId;
      *this << header.Encoding;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::AdditionalHeader>(OpcUa::AdditionalHeader& header)
    {
      *this >> header.TypeId;
      *this >> header.Encoding;
    };


    template<>
    void DataSerializer::Serialize<OpcUa::RequestHeader>(const OpcUa::RequestHeader& header)
    {
      *this << header.SessionAuthenticationToken;
      *this << header.UtcTime;
      *this << header.RequestHandle;
      *this << header.ReturnDiagnostics;
      *this << header.AuditEntryId;
      *this << header.Timeout; // in miliseconds
      *this << header.Additional;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::RequestHeader>(OpcUa::RequestHeader& header)
    {
      *this >> header.SessionAuthenticationToken;
      *this >> header.UtcTime;
      *this >> header.RequestHandle;
      *this >> header.ReturnDiagnostics;
      *this >> header.AuditEntryId;
      *this >> header.Timeout; // in miliseconds
      *this >> header.Additional;
    };

    template<>
    void DataSerializer::Serialize<DiagnosticInfoMask>(const DiagnosticInfoMask& value)
    {
      *this << static_cast<uint8_t>(value);
    }

    template<>
    void DataDeserializer::Deserialize<DiagnosticInfoMask>(DiagnosticInfoMask& value)
    {
      uint8_t tmp = 0;
      *this >> tmp;
      value = static_cast<DiagnosticInfoMask>(tmp);
    }

    template<>
    void DataSerializer::Serialize<OpcUa::DiagnosticInfo>(const OpcUa::DiagnosticInfo& info)
    {
      *this << info.EncodingMask;

      if (info.EncodingMask & DIM_SYMBOLIC_Id)
      {
        *this << info.SymbolicId;
      }
      if (info.EncodingMask & DIM_NAMESPACE)
      {
        *this << info.NamespaceURI;
      }
      if (info.EncodingMask & DIM_LOCALIZED_TEXT)
      {
        *this << info.LocalizedText;
      }
      if (info.EncodingMask & DIM_LOCALE)
      {
        *this << info.Locale;
      }
      if (info.EncodingMask & DIM_ADDITIONAL_INFO)
      {
        *this << info.AdditionalInfo;
      }
      if (info.EncodingMask & DIM_INNER_STATUS_CODE)
      {
        *this << info.InnerStatusCode;
      }
      if ((info.EncodingMask & DIM_INNER_DIAGNOSTIC_INFO) && info.InnerDiagnostics)
      {
        *this << *info.InnerDiagnostics;
      }
    }


    template<>
    void DataDeserializer::Deserialize<OpcUa::DiagnosticInfo>(OpcUa::DiagnosticInfo& info)
    {
      *this >> info.EncodingMask;

      if (info.EncodingMask & DIM_SYMBOLIC_Id)
      {
        *this >> info.SymbolicId;
      }
      if (info.EncodingMask & DIM_NAMESPACE)
      {
        *this >> info.NamespaceURI;
      }
      if (info.EncodingMask & DIM_LOCALIZED_TEXT)
      {
        *this >> info.LocalizedText;
      }
      if (info.EncodingMask & DIM_LOCALE)
      {
        *this >> info.Locale;
      }
      if (info.EncodingMask & DIM_ADDITIONAL_INFO)
      {
        *this >> info.AdditionalInfo;
      }
      if (info.EncodingMask & DIM_INNER_STATUS_CODE)
      {
        *this >> info.InnerStatusCode;
      }
      if (info.EncodingMask & DIM_INNER_DIAGNOSTIC_INFO)
      {
        std::shared_ptr<DiagnosticInfo> tmp(new DiagnosticInfo);
        *this >> *tmp;
        info.InnerDiagnostics = tmp;
      };
    };

    template<>
    void DataSerializer::Serialize<OpcUa::DiagnosticInfoList>(const OpcUa::DiagnosticInfoList& infos)
    {
      SerializeContainer(*this, infos, 0);
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::DiagnosticInfoList>(OpcUa::DiagnosticInfoList& infos)
    {
      DeserializeContainer(*this, infos);
    }

    template<>
    void DataSerializer::Serialize<OpcUa::ResponseHeader>(const OpcUa::ResponseHeader& header)
    {
      *this << header.Timestamp;
      *this << header.RequestHandle;
      *this << header.ServiceResult;
      *this << header.InnerDiagnostics;
      SerializeContainer(*this, header.StringTable);
      *this << header.Additional;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::ResponseHeader>(OpcUa::ResponseHeader& header)
    {
      *this >> header.Timestamp;
      *this >> header.RequestHandle;
      *this >> header.ServiceResult;
      *this >> header.InnerDiagnostics;
      DeserializeContainer(*this, header.StringTable);
      *this >> header.Additional;
    };


    template<>
    void DataSerializer::Serialize<OpcUa::OpenSecureChannelRequest>(const OpcUa::OpenSecureChannelRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
      *this << request.Parameters.ClientProtocolVersion;
      *this << (uint32_t)request.Parameters.RequestType;
      *this << (uint32_t)request.Parameters.SecurityMode;
      SerializeContainer(*this, request.Parameters.ClientNonce);
      *this << request.Parameters.RequestLifeTime;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::OpenSecureChannelRequest>(OpcUa::OpenSecureChannelRequest& request)
    {
      *this >> request.TypeId;
      *this >> request.Header;

      *this >> request.Parameters.ClientProtocolVersion;

      uint32_t tmp = 0;
      *this >> tmp;
      request.Parameters.RequestType = static_cast<SecurityTokenRequestType>(tmp);

      uint32_t tmp2 = 0;
      *this >> tmp2;
      request.Parameters.SecurityMode = static_cast<MessageSecurityMode>(tmp2);

      DeserializeContainer(*this, request.Parameters.ClientNonce);
      *this >> request.Parameters.RequestLifeTime;
    };


    template<>
    void DataSerializer::Serialize<OpcUa::SecurityToken>(const OpcUa::SecurityToken& token)
    {
      *this << token.SecureChannelId;
      *this << token.TokenId;
      *this << token.CreatedAt;
      *this << token.RevisedLifetime;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::SecurityToken>(OpcUa::SecurityToken& token)
    {
      *this >> token.SecureChannelId;
      *this >> token.TokenId;
      *this >> token.CreatedAt;
      *this >> token.RevisedLifetime;
    };


    template<>
    void DataSerializer::Serialize<OpcUa::OpenSecureChannelResponse>(const OpcUa::OpenSecureChannelResponse& response)
    {
      *this << response.TypeId;
      *this << response.Header;
      *this << response.ServerProtocolVersion;
      *this << response.ChannelSecurityToken;
      SerializeContainer(*this, response.ServerNonce);
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::OpenSecureChannelResponse>(OpcUa::OpenSecureChannelResponse& response)
    {
      *this >> response.TypeId;
      *this >> response.Header;
      *this >> response.ServerProtocolVersion;
      *this >> response.ChannelSecurityToken;
      DeserializeContainer(*this, response.ServerNonce);
    };

    template<>
    void DataSerializer::Serialize<OpcUa::Binary::RawMessage>(const OpcUa::Binary::RawMessage& raw)
    {
      Buffer.insert(Buffer.end(), raw.Data, raw.Data + raw.Size);
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::Binary::RawBuffer>(OpcUa::Binary::RawBuffer& raw)
    {
      GetData(In, raw.Data, raw.Size);
    };

    template<>
    void DataSerializer::Serialize<OpcUa::CloseSecureChannelRequest>(const OpcUa::CloseSecureChannelRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::CloseSecureChannelRequest>(OpcUa::CloseSecureChannelRequest& request)
    {
      *this >> request.TypeId;
      *this >> request.Header;
    };

    template<>
    void DataSerializer::Serialize<OpcUa::LocalizedText>(const OpcUa::LocalizedText& lt)
    {
      *this << lt.Encoding;
      if (lt.Encoding & HAS_LOCALE)
      {
        *this << lt.Locale;
      }
      if (lt.Encoding & HAS_TEXT)
      {
        *this << lt.Text;
      }
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::LocalizedText>(OpcUa::LocalizedText& lt)
    {
      *this >> lt.Encoding;
      if (lt.Encoding & HAS_LOCALE)
      {
        *this >> lt.Locale;
      }
      if (lt.Encoding & HAS_TEXT)
      {
        *this >> lt.Text;
      }
    };


    template<>
    void DataSerializer::Serialize<std::vector<LocalizedText>>(const std::vector<LocalizedText>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<LocalizedText>>(std::vector<LocalizedText>& value)
    {
      DeserializeContainer(*this, value);
    }


    template<>
    void DataSerializer::Serialize<SignatureData>(const SignatureData& value)
    {
      *this << value.Signature;
      *this << value.Algorithm;
    }

    template<>
    void DataDeserializer::Deserialize<SignatureData>(SignatureData& value)
    {
      *this >> value.Signature;
      *this >> value.Algorithm;
    }

    template<>
    void DataSerializer::Serialize<ExtensionObjectHeader>(const ExtensionObjectHeader& value)
    {
      *this << value.TypeId;
      *this << static_cast<uint8_t>(value.Encoding);
    }

    template<>
    void DataDeserializer::Deserialize<ExtensionObjectHeader>(ExtensionObjectHeader& value)
    {
      *this >> value.TypeId;
      uint8_t tmp = 0;
      *this >> tmp;
      value.Encoding = static_cast<ExtensionObjectEncoding>(tmp);
    }

    template<>
    void DataSerializer::Serialize<QualifiedName>(const QualifiedName& name)
    {
      *this << name.NamespaceIndex;
      *this << name.Name;
    }

    template<>
    void DataDeserializer::Deserialize<QualifiedName>(QualifiedName&  name)
    {
      *this >> name.NamespaceIndex;
      *this >> name.Name;
    }

    ////////////////////////////////////////////////////////////////////
    // IntegerId
    ////////////////////////////////////////////////////////////////////

    template<>
    void DataSerializer::Serialize<IntegerId>(const IntegerId& id)
    {
      *this << static_cast<uint32_t>(id);
    }

    template<>
    void DataDeserializer::Deserialize<IntegerId>(IntegerId&  id)
    {
      uint32_t value = 0;
      *this >> value;
      id = value;
    }

    template<>
    void DataSerializer::Serialize<std::vector<IntegerId>>(const std::vector<IntegerId>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<IntegerId>>(std::vector<IntegerId>& targets)
    {
      DeserializeContainer(*this, targets);
    }

    ////////////////////////////////////////////////////////////////////
    // StatusCode
    ////////////////////////////////////////////////////////////////////

    template<>
    void DataSerializer::Serialize<StatusCode>(const StatusCode& status)
    {
      *this << static_cast<uint32_t>(status);
    }

    template<>
    void DataDeserializer::Deserialize<StatusCode>(StatusCode&  status)
    {
      uint32_t value = 0;
      *this >> value;
      status = static_cast<StatusCode>(value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<StatusCode>>(const std::vector<StatusCode>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<StatusCode>>(std::vector<StatusCode>& value)
    {
      DeserializeContainer(*this, value);
    }

    template<>
    void DataSerializer::Serialize<std::vector<QualifiedName>>(const std::vector<QualifiedName>& value)
    {
      SerializeContainer(*this, value);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<QualifiedName>>(std::vector<QualifiedName>& value)
    {
      DeserializeContainer(*this, value);
    }


  } // namespace Binary
} // namespace OpcUa

