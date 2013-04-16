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

#include <thread>

namespace
{
  class BufferedInput : public OpcUa::InputChannel
  {
  public:
    BufferedInput()
      : Stopped(false)
    {
    }
  
    virtual std::size_t Receive(char* data, std::size_t size)
    {
      DataEvent.wait();
      std::unique_lock<std::mutex> lock(BufferMutex);
      if (Stopped)
      {
        return 0;
      }

      std::copy(begin(Buffer), end(Buffer), data)
      Buffer.erase(Buffer.begin(), end);
    }

    void AddBuffer(char buf, std::size_t size)
    {
      std::unique_lock<std::mutex> lock(BufferMutex);
      if (Stopped)
      {
        return;
      }

      Buffer.insert(Buffer.begin(), buf, buf + size);
      DataEvent.notify_one();
    }

    void Stop()
    {
      Stopped = true;
    }

  private:
    std::atomic<bool> Stopped;
  };


  class BufferedIO : public IOChannel
  {
  public:
    BufferedIO(std::weak_ptr<InputChannel> input, std::weak_ptr<BufferedInput> output)
      : Input(input)
      , Ouput(output)
    {
    }

    virtual std::size_t Receive(char* data, std::size_t size)
    {
      std::shared_ptr<InputChannel> input = Input.lock();
      if (input)
      {
        return input->Receive(data, size);
      }
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

      ClientChannel.reset(new BufferedIO(*ClientInput, *ServerInput));
      ServerChannel.reset(new BufferedIO(*ServerInput, *ClientInput));

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
      ClientINput.reset();
      SlientINput.reset();
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

