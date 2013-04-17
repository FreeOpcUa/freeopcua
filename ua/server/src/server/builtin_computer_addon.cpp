/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/common/addons_core/addon.h>
#include <opc/ua/client/binary_computer.h>
#include <opc/ua/server/addons/builtin_computer.h>
#include <opc/ua/server/addons/endpoints.h>
#include <opc/ua/server/server.h>

#include <internal/thread.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace
{
  class BufferedInput : public OpcUa::InputChannel
  {
  public:
    BufferedInput()
      : MaxBufferSize(4096)
      , Buffer(MaxBufferSize)
      , Stopped(false)
    {
      Buffer.reserve(MaxBufferSize);
    }

    virtual std::size_t Receive(char* data, std::size_t size)
    {
      ThrowIfStopped();

      std::size_t totalConsumedSize = 0;
      while (totalConsumedSize < size)
      {
        std::unique_lock<std::mutex> lock(BufferMutex);
        NotEmpty.wait(lock, [&Stopped, &Buffer, &MaxBufferSize](){return !(Stopped || Buffer.size() != 0);});
        ThrowIfStopped();

        const std::size_t sizeToConsume = std::min(size - totalConsumedSize, Buffer.size());
        auto endIt = Buffer.begin() + sizeToConsume;
        std::copy(begin(Buffer), endIt, data + totalConsumedSize);
        Buffer.erase(Buffer.begin(), endIt); // TODO make behavoior with round buffer to avoid this.
        totalConsumedSize += sizeToConsume;
      }

      if (totalConsumedSize)
      {
        NotFull.notify_one();
      }
      return totalConsumedSize;
    }
  

    void AddBuffer(const char* buf, std::size_t size)
    {
      ThrowIfStopped();

      std::size_t totalSendedSize = 0;
      while(totalSendedSize != size)
      {
        std::unique_lock<std::mutex> lock(BufferMutex);
        NotFull.wait(lock, [&Stopped, &Buffer, &MaxBufferSize]() { return !(Stopped || Buffer.size() <= MaxBufferSize); });
        ThrowIfStopped();

        const std::size_t sizeToSend = std::min(MaxBufferSize - Buffer.size(), size - totalSendedSize); 
        Buffer.insert(Buffer.end(), buf + totalSendedSize, buf + sizeToSend);
        totalSendedSize += sizeToSend;
        NotEmpty.notify_one();
      }
    }

    void Stop()
    {
      Stopped = true;
      NotEmpty.notify_all();
      NotFull.notify_all();
    }

  private:
    void ThrowIfStopped()
    {
      if (Stopped)
      {
        throw std::logic_error("Conversation through connection stopped.");
      }
    }

  private:
    const std::size_t MaxBufferSize;
    std::vector<char> Buffer;
    std::atomic<bool> Stopped;
    std::mutex BufferMutex;
    std::condition_variable NotEmpty;
    std::condition_variable NotFull;
  };


  class BufferedIO : public OpcUa::IOChannel
  {
  public:
    BufferedIO(std::weak_ptr<InputChannel> input, std::weak_ptr<BufferedInput> output)
      : Input(input)
      , Output(output)
    {
    }

    virtual std::size_t Receive(char* data, std::size_t size)
    {
      std::shared_ptr<InputChannel> input = Input.lock();
      if (input)
      {
        return input->Receive(data, size);
      }
      return 0;
    }

    virtual void Send(const char* message, std::size_t size)
    {
      std::shared_ptr<BufferedInput> output = Output.lock();
      if (output)
      {
        output->AddBuffer(message, size);
      }
    }

  private:
    std::weak_ptr<InputChannel> Input;
    std::weak_ptr<BufferedInput> Output;
  };


  void Process(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor, std::shared_ptr<OpcUa::IOChannel> channel)
  {
    processor->Process(channel);
  }


  class BuiltinComputerAddon : public OpcUa::Server::BuiltinComputerAddon
  {
  public:
    virtual std::shared_ptr<OpcUa::Remote::Computer> GetComputer() const
    {
      return OpcUa::Remote::CreateBinaryComputer(ClientChannel);
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& addons)
    {
      ServerInput.reset(new BufferedInput());
      ClientInput.reset(new BufferedInput());


      ClientChannel.reset(new BufferedIO(ClientInput, ServerInput));
      ServerChannel.reset(new BufferedIO(ServerInput, ClientInput));

      std::shared_ptr<OpcUa::Server::EndpointsAddon> endpoints = Common::GetAddon<OpcUa::Server::EndpointsAddon>(addons, OpcUa::Server::EndpointsAddonID);
      std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor = endpoints->GetProcessor();
      Thread.reset(new OpcUa::Internal::Thread(std::bind(Process, processor, ServerChannel)));
    }

    virtual void Stop()
    {
      ClientInput->Stop();
      ServerInput->Stop();

      if (Thread.get())
      {
        Thread->Join();
        Thread.reset();
      }

      ClientInput.reset();
      ServerInput.reset();
    }

  private:
    std::shared_ptr<BufferedInput> ClientInput;
    std::shared_ptr<BufferedInput> ServerInput;

    std::shared_ptr<OpcUa::IOChannel> ClientChannel;
    std::shared_ptr<OpcUa::IOChannel> ServerChannel;
    std::unique_ptr<OpcUa::Internal::Thread> Thread;
  };

}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::BuiltinComputerAddon());
}

