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

#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/endpoints_services.h>
#include <src/server/endpoints_parameters.h>

#include <iostream>

using namespace OpcUa::Impl;


class OpcUa::Impl::BufferedInput : public OpcUa::InputChannel
{
public:
  explicit BufferedInput(bool debug);
  virtual std::size_t Receive(char* data, std::size_t size);
  void AddBuffer(const char* buf, std::size_t size);
  void Stop();

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
    BufferedIO(const char* channelId, std::weak_ptr<InputChannel> input, std::weak_ptr<BufferedInput> output, bool debug)
      : Input(input)
      , Output(output)
      , Id(channelId)
      , Debug(debug)
    {
    }

    virtual std::size_t Receive(char* data, std::size_t size)
    {
      if (Debug) std::clog << Id << ": receive data." << std::endl;

      if (std::shared_ptr<InputChannel> input = Input.lock())
      {
        return input->Receive(data, size);
      }
      return 0;
    }

    virtual void Send(const char* message, std::size_t size)
    {
      if (Debug) std::clog << Id << ": send data." << std::endl;

      if (std::shared_ptr<BufferedInput> output = Output.lock())
      {
        output->AddBuffer(message, size);
      }
    }

    virtual void Stop()
    {
      if (std::shared_ptr<BufferedInput> output = Output.lock())
        output->Stop();

      if (std::shared_ptr<InputChannel> input = Input.lock())
        return input->Stop();
    }

  private:
    std::weak_ptr<InputChannel> Input;
    std::weak_ptr<BufferedInput> Output;
    const std::string Id;
    bool Debug;
  };


  void Process(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor, std::shared_ptr<OpcUa::IOChannel> channel)
  {
    processor->Process(channel);
  }
}  // namespace



BuiltinServerAddon::BuiltinServerAddon()
  : Debug(false)
{
}

OpcUa::Services::SharedPtr BuiltinServerAddon::GetServices() const
{
  if (!ClientChannel)
  {
    throw std::logic_error("Cannot access builtin computer. No endpoints was created. You have to configure endpoints.");
  }

  OpcUa::SecureConnectionParams params;
  params.EndpointUrl = "opc.tcp://localhost:4841";
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
  return OpcUa::CreateBinaryClient(ClientChannel, params);
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

  const std::vector<OpcUa::Server::ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Debug);
  for (OpcUa::Server::ApplicationData d: applications) {
    std::cout << "Endpoint is: " << d.Endpoints.front().EndpointURL << std::endl;
  }

  std::vector<OpcUa::ApplicationDescription> applicationDescriptions;
  std::vector<OpcUa::EndpointDescription> endpointDescriptions;
  for (const OpcUa::Server::ApplicationData application : applications)
  {
    applicationDescriptions.push_back(application.Application);
    endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
  }

  OpcUa::Server::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::Server::EndpointsRegistry>(OpcUa::Server::EndpointsRegistryAddonId);
  if (!endpointsAddon)
  {
    std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
    return;
  }
  endpointsAddon->AddEndpoints(endpointDescriptions);
  endpointsAddon->AddApplications(applicationDescriptions);

  OpcUa::Server::ServicesRegistry::SharedPtr internalServer = addons.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);

  Protocol = OpcUa::Server::CreateOpcUaProtocol(*this, Debug);
  Protocol->StartEndpoints(endpointDescriptions, internalServer->GetServer());
}

void BuiltinServerAddon::Stop()
{
  Protocol.reset();
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

void BuiltinServerAddon::Listen(const OpcUa::Server::TcpParameters&, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor)
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

void BuiltinServerAddon::StopListen(const OpcUa::Server::TcpParameters&)
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

OpcUa::Server::TcpServer::UniquePtr OpcUa::Server::CreateTcpServer()
{
  return TcpServer::UniquePtr(new BuiltinServerAddon);
}

