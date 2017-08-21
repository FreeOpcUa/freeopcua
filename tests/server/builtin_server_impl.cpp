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
  explicit BufferedInput(const Common::Logger::SharedPtr & logger);
  virtual std::size_t Receive(char * data, std::size_t size);
  void AddBuffer(const char * buf, std::size_t size);
  void Stop();

private:
  void ThrowIfStopped();

private:
  std::vector<char> Buffer;
  std::atomic<bool> Running;
  std::mutex BufferMutex;
  std::condition_variable DataReady;
  Common::Logger::SharedPtr Logger;
};


BufferedInput::BufferedInput(const Common::Logger::SharedPtr & logger)
  : Running(true)
  , Logger(logger)
{
  Buffer.reserve(4096);
}

std::size_t BufferedInput::Receive(char * data, std::size_t size)
{
  LOG_DEBUG(Logger, "consuming {} bytes of data", size);

  ThrowIfStopped();


  std::size_t totalConsumedSize = 0;

  while (totalConsumedSize < size)
    {
      std::unique_lock<std::mutex> event(BufferMutex);

      if (Buffer.empty())
        {
          LOG_DEBUG(Logger, "waiting for client data");

          DataReady.wait(event);
        }

      else if (!event.owns_lock())
        {
          event.lock();
        }

      LOG_DEBUG(Logger, "buffer contains client data");

      ThrowIfStopped();

      LOG_DEBUG(Logger, "client sent data");

      ThrowIfStopped();

      if (Buffer.empty())
        {
          LOG_DEBUG(Logger, "buffer is empty");
          continue;
        }

      const std::size_t sizeToConsume = std::min(size - totalConsumedSize, Buffer.size());

      LOG_DEBUG(Logger, "consuming {} bytes of data", sizeToConsume);

      auto endIt = Buffer.begin() + sizeToConsume;
      std::copy(begin(Buffer), endIt, data + totalConsumedSize);
      Buffer.erase(Buffer.begin(), endIt); // TODO make behavior with round buffer to avoid this.
      totalConsumedSize += sizeToConsume;
    }

  return totalConsumedSize;
}


void BufferedInput::AddBuffer(const char * buf, std::size_t size)
{
  ThrowIfStopped();

  LOG_DEBUG(Logger, "client wants to send {} bytes of data", size);

  std::lock_guard<std::mutex> lock(BufferMutex);
  ThrowIfStopped();

  Buffer.insert(Buffer.end(), buf, buf + size);

  LOG_DEBUG(Logger, "size of buffer is {} bytes", Buffer.size());

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
      throw std::logic_error("conversation through connection stopped");
    }
}


namespace
{

class BufferedIO : public OpcUa::IOChannel
{
public:
  BufferedIO(const char * channelId, std::weak_ptr<InputChannel> input, std::weak_ptr<BufferedInput> output, const Common::Logger::SharedPtr & logger)
    : Input(input)
    , Output(output)
    , Id(channelId)
    , Logger(logger)
  {
  }

  virtual std::size_t Receive(char * data, std::size_t size)
  {
    LOG_DEBUG(Logger, "{}: receive data", Id);

    if (std::shared_ptr<InputChannel> input = Input.lock())
      {
        return input->Receive(data, size);
      }

    return 0;
  }

  virtual void Send(const char * message, std::size_t size)
  {
    LOG_DEBUG(Logger, "{}: send data", Id);

    if (std::shared_ptr<BufferedInput> output = Output.lock())
      {
        output->AddBuffer(message, size);
      }
  }

  virtual void Stop()
  {
    if (std::shared_ptr<BufferedInput> output = Output.lock())
      { output->Stop(); }

    if (std::shared_ptr<InputChannel> input = Input.lock())
      { return input->Stop(); }
  }

private:
  std::weak_ptr<InputChannel> Input;
  std::weak_ptr<BufferedInput> Output;
  const std::string Id;
  Common::Logger::SharedPtr Logger;
};


void Process(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor, std::shared_ptr<OpcUa::IOChannel> channel)
{
  processor->Process(channel);
}
}  // namespace


BuiltinServerAddon::BuiltinServerAddon(const Common::Logger::SharedPtr & logger)
  : TcpServer(logger)
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

void BuiltinServerAddon::Initialize(Common::AddonsManager & addons, const Common::AddonParameters & params)
{
  Logger = addons.GetLogger();

  for (const Common::Parameter parameter : params.Parameters)
    {
      /*
      if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
        {
          Debug = true;
        }
       */
    }

  const std::vector<OpcUa::Server::ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Logger);

  for (OpcUa::Server::ApplicationData d : applications)
    {
      LOG_INFO(Logger, "endpoint is: {}", d.Endpoints.front().EndpointUrl);
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
      LOG_ERROR(Logger, "cannot store endpoints information, endpoints service addon has not been registered");
      return;
    }

  endpointsAddon->AddEndpoints(endpointDescriptions);
  endpointsAddon->AddApplications(applicationDescriptions);

  OpcUa::Server::ServicesRegistry::SharedPtr internalServer = addons.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);

  Protocol = OpcUa::Server::CreateOpcUaProtocol(*this, Logger);
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

void BuiltinServerAddon::Listen(const OpcUa::Server::TcpParameters &, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor)
{
  if (Thread)
    {
      throw std::logic_error("unable to serve more then one binary connection");
    }

  ServerInput.reset(new BufferedInput(Logger));
  ClientInput.reset(new BufferedInput(Logger));

  ClientChannel.reset(new BufferedIO("Client", ClientInput, ServerInput, Logger));
  ServerChannel.reset(new BufferedIO("Server", ServerInput, ClientInput, Logger));

  Thread.reset(new Common::Thread(std::bind(Process, processor, ServerChannel), this));
}

void BuiltinServerAddon::StopListen(const OpcUa::Server::TcpParameters &)
{
  Stop();
}

void BuiltinServerAddon::OnSuccess()
{
  ClientInput->Stop();

  LOG_DEBUG(Logger, "server thread exited successfully");
}

void BuiltinServerAddon::OnError(const std::exception & exc)
{
  ClientInput->Stop();

  LOG_ERROR(Logger, "server thread exited with error: {}", exc.what());
}

OpcUa::Server::TcpServer::UniquePtr OpcUa::Server::CreateTcpServer(const Common::Logger::SharedPtr & logger)
{
  return TcpServer::UniquePtr(new BuiltinServerAddon(logger));
}

