/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc bbinary stream.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __OPC_UA_MAPPING_SERIALIZE_H__
#define __OPC_UA_MAPPING_SERIALIZE_H__

#include <opc/ua/channel.h>
#include <memory>
#include <vector>

#define OPCUA_DEFAULT_BUFFER_SIZE 4096

namespace OpcUa
{
  namespace Binary
  {

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

    class OStream
    {
    public:
      explicit OStream(std::shared_ptr<OutputChannel> channel, std::size_t defBufferSize = OPCUA_DEFAULT_BUFFER_SIZE)
        : Out(channel)
      {
        Buffer.reserve(defBufferSize);
      }

      virtual ~OStream()
      {
      }

      template <typename T>
      OStream& operator<<(const T& value)
      {
        Serialize<T>(value);
        return *this;
      }

      OStream& operator<< (OStream& (*pf)(OStream& out))
      {
        return pf(*this);
      }

      void Flush()
      {
        Out->Send(&Buffer[0], Buffer.size());
        Buffer.clear();
      }

    private:
      template<typename T>
      void Serialize(const T& value);

    private:
      std::shared_ptr<OutputChannel> Out;
      std::vector<char> Buffer;
    };

    OStream& flush(OStream& os);

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


    class IStream
    {
    public:
      explicit IStream(std::shared_ptr<InputChannel> channel)
        : In(channel)
      {
      }

      virtual ~IStream()
      {
      }

      template <typename T>
      IStream& operator>>(T& value)
      {
        Deserialize<T>(value);
        return *this;
      }

    private:
      template <typename T>
      void Deserialize(T&);

    private:
      std::shared_ptr<InputChannel> In;
    };

    class IOStream : public IStream, public OStream
    {
    public:
      IOStream(std::shared_ptr<IOChannel> channel)
        : IStream(channel)
        , OStream(channel)
      {
      }
    };

  } // namespace Binary
} // namespace OpcUa

#endif // __OPC_UA_MAPPING_SERIALIZE_H__

