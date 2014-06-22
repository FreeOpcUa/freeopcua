/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "builtin_server_impl.h"

#include <iostream>

using namespace OpcUa::Impl;


class OpcUa::Impl::BufferedInput : public OpcUa::InputChannel
{
public:
  explicit BufferedInput(bool debug);
  virtual std::size_t Receive(char* data, std::size_t size);
  void AddBuffer(const char* buf, std::size_t size);
  void Stop();

  virtual int WaitForData(float second)
  {
    return 1;
  }

private:
  void ThrowIfStopped();

private:
  std::vector<char> Buffer;
  std::atomic<bool> Running;
  std::mutex BufferMutex;
  std::condition_variable DataReady;
  bool Debug;
};


BufferedInput::BufferedInput(bool debug)
  : Running(true)
  , Debug(debug)
{
  Buffer.reserve(4096);
}

std::size_t BufferedInput::Receive(char* data, std::size_t size)
{
  if (Debug) std::clog << "Consuming " << size << " bytes of data." << std::endl;

  ThrowIfStopped();


  std::size_t totalConsumedSize = 0;
  while (totalConsumedSize < size)
  {
    std::unique_lock<std::mutex> event(BufferMutex);
    if (Buffer.empty())
    {
      if (Debug) std::clog << "Waiting data from client" << std::endl;
      DataReady.wait(event);
    }
    else if(!event.owns_lock())
    {
      event.lock();
    }
    if (Debug) std::clog << "Buffer contain data from client." << std::endl;
    ThrowIfStopped();
    if (Debug) std::clog << "Client sent data." << std::endl;

    ThrowIfStopped();
    if (Buffer.empty())
    {
      if (Debug) std::clog << "No data in buffer." << std::endl;
      continue;
    }

    const std::size_t sizeToConsume = std::min(size - totalConsumedSize, Buffer.size());
    if (Debug) std::clog << "Consuming " << sizeToConsume << " bytes of data." << std::endl;
    auto endIt = Buffer.begin() + sizeToConsume;
    std::copy(begin(Buffer), endIt, data + totalConsumedSize);
    Buffer.erase(Buffer.begin(), endIt); // TODO make behavior with round buffer to avoid this.
    totalConsumedSize += sizeToConsume;
  }

  return totalConsumedSize;
}


void BufferedInput::AddBuffer(const char* buf, std::size_t size)
{
  ThrowIfStopped();
  if (Debug) std::clog << "Client want to send " << size << " bytes of data" << std::endl;
  std::lock_guard<std::mutex> lock(BufferMutex);
  ThrowIfStopped();

  Buffer.insert(Buffer.end(), buf, buf + size);
  if (Debug) std::clog << "Size of buffer " << Buffer.size() << " bytes." << std::endl;
  DataReady.notify_all();
}

void BufferedInput::Stop()
{
  Running = false;
  DataReady.notify_all();
}

void BufferedInput::ThrowIfStopped()
{
  if (!Running)
  {
    throw std::logic_error("Conversation through connection stopped.");
  }
}


namespace
{

  class BufferedIO : public OpcUa::IOChannel
  {
  public:
    BufferedIO(const char* channelID, std::weak_ptr<InputChannel> input, std::weak_ptr<BufferedInput> output, bool debug)
      : Input(input)
      , Output(output)
      , ID(channelID)
      , Debug(debug)
    {
    }

    virtual std::size_t Receive(char* data, std::size_t size)
    {
      if (Debug) std::clog << ID << ": receive data." << std::endl;

      std::shared_ptr<InputChannel> input = Input.lock();
      if (input)
      {
        return input->Receive(data, size);
      }
      return 0;
    }

    virtual int WaitForData(float second)
    {
      std::shared_ptr<InputChannel> input = Input.lock();
      if (input)
      {
        return input->WaitForData(second);
      }
      return 0;
    }

    virtual void Send(const char* message, std::size_t size)
    {
      if (Debug) std::clog << ID << ": send data." << std::endl;

      std::shared_ptr<BufferedInput> output = Output.lock();
      if (output)
      {
        output->AddBuffer(message, size);
      }
    }

  private:
    std::weak_ptr<InputChannel> Input;
    std::weak_ptr<BufferedInput> Output;
    const std::string ID;
    bool Debug;
  };


  void Process(std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor, std::shared_ptr<OpcUa::IOChannel> channel)
  {
    processor->Process(channel);
  }
}  // namespace



BuiltinServerAddon::BuiltinServerAddon()
  : Debug(false)
{
}

std::shared_ptr<OpcUa::Remote::Server> BuiltinServerAddon::GetServer() const
{
  if (!ClientChannel)
  {
    throw std::logic_error("Cannot access builtin computer. No endpoints was created. You have to configure endpoints.");
  }

  OpcUa::Binary::SecureConnectionParams params;
  params.EndpointUrl = "opc.tcp://localhost:4841";
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
  std::shared_ptr<OpcUa::IOChannel> secureChannel = OpcUa::Binary::CreateSecureChannel(ClientChannel, params);
  return OpcUa::Remote::CreateBinaryServer(secureChannel);
}

BuiltinServerAddon::~BuiltinServerAddon()
{
  try
  {
    Stop();
  }
  catch (...)
  {
  }
}

void BuiltinServerAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
  for (const Common::Parameter parameter : params.Parameters)
  {
    if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
    {
      Debug = true;
    }
  }
}

void BuiltinServerAddon::Stop()
{
  if (ClientInput)
  {
    ClientInput->Stop();
    ServerInput->Stop();
  }

  if (Thread.get())
  {
    Thread->Join();
    Thread.reset();
  }

  ClientInput.reset();
  ServerInput.reset();
}

void BuiltinServerAddon::Listen(const OpcUa::UaServer::TcpParameters&, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor)
{
  if (Thread)
  {
    throw std::logic_error("Unable to start second thread. Builtin computer can listen only one binary connection.");
  }

  ServerInput.reset(new BufferedInput(Debug));
  ClientInput.reset(new BufferedInput(Debug));

  ClientChannel.reset(new BufferedIO("Client", ClientInput, ServerInput, Debug));
  ServerChannel.reset(new BufferedIO("Server", ServerInput, ClientInput, Debug));

  Thread.reset(new Common::Thread(std::bind(Process, processor, ServerChannel), this));
}

void BuiltinServerAddon::StopListen(const OpcUa::UaServer::TcpParameters&)
{
  Stop();
}

void BuiltinServerAddon::OnSuccess()
{
  ClientInput->Stop();
  if (Debug) std::clog  << "Server thread exited with success." << std::endl;
}

void BuiltinServerAddon::OnError(const std::exception& exc)
{
  ClientInput->Stop();
  if (Debug) std::clog  << "Server thread exited with error: " << exc.what() << std::endl;
}
