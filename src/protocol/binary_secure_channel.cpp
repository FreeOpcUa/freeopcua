/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary secure channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/binary/secure_connection.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/secure_channel.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  OpcUa::Binary::Acknowledge HelloServer(std::shared_ptr<OpcUa::IOChannel> channel, const SecureConnectionParams& params)
  {
    Hello hello;
    hello.ProtocolVersion = 0;
    hello.ReceiveBufferSize = 65536;
    hello.SendBufferSize = 65536;
    hello.MaxMessageSize = 65536;
    hello.MaxChunkCount = 256;
    hello.EndpointUrl = params.EndpointUrl;

    Header hdr(MT_HELLO, CHT_SINGLE);
    hdr.AddSize(RawSize(hello));  

    IOStream<IOChannel> io(channel);
    io << hdr << hello << flush;

    OpcUa::Binary::Header respHeader;
    io >> respHeader; // TODO add check for acknowledge header

    Acknowledge ack;
    io >> ack; // TODO check for connection parameters
    return ack;
  }

  class BufferInputChannel : public OpcUa::InputChannel
  {
   public:
     BufferInputChannel(const std::vector<char>& buffer)
       : Buffer(buffer)
       , Pos(0)
     {
       Reset();
     }

     virtual std::size_t Receive(char* data, std::size_t size)
     {
       if (Pos >= Buffer.size())
       {
         return 0;
       }

       size = std::min(size, Buffer.size() - Pos);
       std::copy(&Buffer[Pos], &Buffer[Pos + size], data);
       Pos += size;
       return size;
     }

     virtual int WaitForData(float second)
     {
       //FIXME implement if necessary
       return 1;
     }

     void Reset()
     {
       Pos = 0;
     }

  private:
    const std::vector<char>& Buffer;
    std::size_t Pos;
  };


  class SecureIOChannel : public OpcUa::IOChannel
  {
  public:
    SecureIOChannel(std::shared_ptr<OpcUa::IOChannel> remoteServer, const SecureConnectionParams& params)
      : RawChannel(remoteServer)
      , Params(params)
      , BufferInput(Buffer)
      , SequenceNumber(1)
      , RequestNumber(1)
    {
      Buffer.reserve(8192);
      const Acknowledge& ack = HelloServer(remoteServer, params);
      const OpenSecureChannelResponse& response = OpenChannel(remoteServer);
      ChannelSecurityToken = response.ChannelSecurityToken; 
    }

    virtual ~SecureIOChannel()
    {
      try
      {
        SecureHeader hdr(MT_SECURE_CLOSE, CHT_SINGLE, ChannelSecurityToken.SecureChannelID);

        const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
        hdr.AddSize(RawSize(algorithmHeader));

        const SequenceHeader sequence = CreateSequenceHeader();
        hdr.AddSize(RawSize(sequence));

        CloseSecureChannelRequest request;
        hdr.AddSize(RawSize(request));

        OStream<IOChannel> out(RawChannel);
        out << hdr << algorithmHeader << sequence << request;
        out.Flush();
      }
      catch (const std::exception& exc)
      {
        std::cerr << "Closing secure channel failed with error: " << exc.what() << std::endl;
      }
    }

    virtual std::size_t Receive(char* data, std::size_t size)
    {
      if (size == 0)
      {
        return 0;
      }

      std::size_t totalReceived = 0;
      while(totalReceived < size)
      {
        const std::size_t received = BufferInput.Receive(data, size);
        if (received != 0)
        {
          totalReceived += received;
          continue;
        }
        ReceiveNewData();
      }
      return totalReceived;
    }

    virtual int WaitForData(float second)
    {
      //FIXME implement if necessary
      return 1;
    }

    virtual void Send(const char* message, std::size_t size)
    {
      // TODO add support for breaking message into multiple chunks
      SecureHeader hdr(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelSecurityToken.SecureChannelID);
      const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
      hdr.AddSize(RawSize(algorithmHeader));

      const SequenceHeader sequence = CreateSequenceHeader();
      hdr.AddSize(RawSize(sequence));
      hdr.AddSize(size);

      OStream<IOChannel> out(RawChannel);
      out << hdr << algorithmHeader << sequence << OpcUa::Binary::RawMessage(message, size) << flush;
    }

  private:
    void ReceiveNewData()
    {
      // TODO add support for multiple chunks

      IStream<IOChannel> in(RawChannel);

      SecureHeader responseHeader;
      in >> responseHeader;

      SymmetricAlgorithmHeader responseAlgo;
      in >> responseAlgo;

      SequenceHeader responseSequence;
      in >> responseSequence; // TODO Check for request Number

      const std::size_t expectedHeaderSize = RawSize(responseHeader) + RawSize(responseAlgo) + RawSize(responseSequence);
      if (expectedHeaderSize >= responseHeader.Size)
      {
        std::stringstream stream;
        stream << "Size of received message " << responseHeader.Size << " bytes is invalid. Expected size " << expectedHeaderSize << " bytes";
        throw std::runtime_error(stream.str());
      }

      const std::size_t dataSize = responseHeader.Size - expectedHeaderSize;
      Buffer.resize(dataSize);
      RawBuffer raw(&Buffer[0], dataSize);
      in >> raw;

      BufferInput.Reset();
    }

    OpcUa::OpenSecureChannelResponse OpenChannel(std::shared_ptr<OpcUa::IOChannel> channel)
    {
      SecureHeader hdr(MT_SECURE_OPEN, CHT_SINGLE, 0);
      AsymmetricAlgorithmHeader algorithmHeader;
      algorithmHeader.SecurityPolicyURI = Params.SecurePolicy;
      algorithmHeader.SenderCertificate = Params.SenderCertificate;
      algorithmHeader.ReceiverCertificateThumbPrint = Params.ReceiverCertificateThumbPrint;
      hdr.AddSize(RawSize(algorithmHeader));

      const SequenceHeader sequence = CreateSequenceHeader();
      hdr.AddSize(RawSize(sequence));

      OpenSecureChannelRequest openChannel;
      openChannel.ClientProtocolVersion = 0;
      openChannel.RequestType = STR_ISSUE;
      openChannel.SecurityMode = MSM_NONE;
      openChannel.ClientNonce = std::vector<uint8_t>(1, 0);
      openChannel.RequestLifeTime = 300000;
      hdr.AddSize(RawSize(openChannel));

      IOStream<IOChannel> in(RawChannel);
      in << hdr << algorithmHeader << sequence << openChannel << flush;

      SecureHeader responseHeader;
      in >> responseHeader;
      AsymmetricAlgorithmHeader responseAlgo;
      in >> responseAlgo;

      SequenceHeader responseSequence;
      in >> responseSequence;
      OpenSecureChannelResponse response;
      in >> response;

      return response;
    }

    SymmetricAlgorithmHeader CreateAlgorithmHeader() const
    {
      SymmetricAlgorithmHeader algorithmHeader;
      algorithmHeader.TokenID = ChannelSecurityToken.TokenID;
      return algorithmHeader;
    }

    SequenceHeader CreateSequenceHeader() const
    {
      SequenceHeader sequence;
      sequence.SequenceNumber = ++SequenceNumber;
      sequence.RequestID = ++RequestNumber;
      return sequence;
    }

  private:
    std::shared_ptr<OpcUa::IOChannel> RawChannel;
    OpcUa::Binary::SecureConnectionParams Params;
    std::vector<char> Buffer;
    BufferInputChannel BufferInput;

    SecurityToken ChannelSecurityToken;
    mutable uint32_t SequenceNumber;
    mutable uint32_t RequestNumber;
  };
}

std::unique_ptr<OpcUa::IOChannel> OpcUa::Binary::CreateSecureChannel(std::shared_ptr<OpcUa::IOChannel> rawChannel, const OpcUa::Binary::SecureConnectionParams& params)
{
  return std::unique_ptr<OpcUa::IOChannel>(new SecureIOChannel(rawChannel, params));
}

