/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary stream. 
/// Stream classes perform Serialization/Deserialization of opc ua structures.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef __OPC_UA_MAPPING_SERIALIZE_H__
#define __OPC_UA_MAPPING_SERIALIZE_H__

#include <opc/ua/protocol/binary/common.h>

#include <memory>
#include <vector>

#define OPCUA_DEFAULT_BUFFER_SIZE 4096

namespace OpcUa
{
  namespace Binary
  {

    class DataSerializer
    {
    public:
      explicit DataSerializer(std::size_t defBufferSize = OPCUA_DEFAULT_BUFFER_SIZE)
      {
        Buffer.reserve(defBufferSize);
      }

      template <typename T>
      DataSerializer& operator<<(const T& value)
      {
        Serialize<T>(value);
        return *this;
      }

      template <typename Acceptor>
      void Flush(Acceptor& aceptor)
      {
        aceptor.Send(&Buffer[0], Buffer.size());
        Buffer.clear();
      }

      template<typename T>
      void Serialize(const T& value);

    private:
      std::vector<char> Buffer;
    };

    class DataSupplier
    {
    public:
      virtual size_t Read(char* buffer, size_t size) = 0;
    };

    class DataDeserializer
    {
    public:
      DataDeserializer(DataSupplier& supplier)
        : In(supplier)
      {
      }

      virtual ~DataDeserializer()
      {
      }

      template <typename T>
      DataDeserializer& operator>>(T& value)
      {
        Deserialize<T>(value);
        return *this;
      }

      template <typename T>
      void Deserialize(T&);

    private:
      DataSupplier& In;
    };


    struct RawMessage
    {
      const char* Data;
      std::size_t Size;

      RawMessage(const char* data, std::size_t size)
        : Data(data)
        , Size(size)
      {
      }
    };


    template <typename OutputChannelType>
    class OStream
    {
    public:
      explicit OStream(std::shared_ptr<OutputChannelType> channel)
        : Out(*channel)
        , Holder(channel)
      {
      }

      explicit OStream(OutputChannelType& channel)
        : Out(channel)
      {
      }

      virtual ~OStream()
      {
      }

      template <typename T>
      OStream& operator<<(const T& value)
      {
        Serializer << value;
        return *this;
      }

      OStream& operator<< (OStream& (*pf)(OStream& out))
      {
        return pf(*this);
      }

      void Flush()
      {
        Serializer.Flush(Out);
      }

    private:
      OutputChannelType& Out;
      std::shared_ptr<OutputChannelType> Holder;
      DataSerializer Serializer;
    };

    template <typename ChannelType>
    inline OStream<ChannelType>& flush(OStream<ChannelType>& os)
    {
      os.Flush();
      return os;
    }

    struct RawBuffer
    {
      char* Data;
      std::size_t Size;

      RawBuffer(char* data, std::size_t size)
        : Data(data)
        , Size(size)
      {
      }
    };


    template <typename InputChannelType>
    class IStream : private DataSupplier
    {
    public:
      explicit IStream(std::shared_ptr<InputChannelType> channel)
        : In(*channel)
        , Holder(channel)
        , Deserializer(*this)
      {
      }

      explicit IStream(InputChannelType& channel)
        : In(channel)
        , Deserializer(*this)
      {
      }

      virtual ~IStream()
      {
      }

      template <typename T>
      IStream& operator>>(T& value)
      {
        Deserializer >> value;
        return *this;
      }

    private:
      virtual size_t Read(char* buffer, size_t size)
      {
        return In.Receive(buffer, size);
      }

    private:
      InputChannelType& In;
      std::shared_ptr<InputChannelType> Holder;
      DataDeserializer Deserializer;
    };


    template <typename IOChannelType>
    class IOStream : public IStream<IOChannelType>, public OStream<IOChannelType>
    {
    public:
      IOStream(std::shared_ptr<IOChannelType> channel)
        : IStream<IOChannelType>(channel)
        , OStream<IOChannelType>(channel)
      {
      }
    };

    template <typename InputChannelType, typename OutputChannelType>
    class InputOutputStream : public IStream<InputChannelType>, public OStream<OutputChannelType>
    {
    public:
      InputOutputStream(std::shared_ptr<InputChannelType> input, std::shared_ptr<OutputChannelType> output)
        : IStream<InputChannelType>(input)
        , OStream<OutputChannelType>(output)
      {
      }
    };

  } // namespace Binary
} // namespace OpcUa

#endif // __OPC_UA_MAPPING_SERIALIZE_H__

