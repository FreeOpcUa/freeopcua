/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Input/output channels interfaces.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <system_error>

namespace OpcUa
{

  class InputChannel
  {
  public:
    typedef std::shared_ptr<InputChannel> SharedPtr;
    typedef std::unique_ptr<InputChannel> UniquePtr;

  public:
    virtual ~InputChannel(){}

    InputChannel(){}
    InputChannel(const InputChannel&) = delete;
    InputChannel(InputChannel&&) = delete;
    InputChannel& operator=(const InputChannel&) = delete;

  public:
    /// @brief Receive data
    /// @param data data for send
    /// @param size size of data
    /// @return size of received data
    virtual std::size_t Receive(char* data, std::size_t size) = 0;
  };


  typedef std::function<void(const char* buffer, const std::system_error& err)> ReceiveCallback;

  class AsyncInputChannel
  {
  public:
    typedef std::shared_ptr<InputChannel> SharedPtr;
    typedef std::unique_ptr<InputChannel> UniquePtr;

  public:
    virtual ~AsyncInputChannel(){}
    AsyncInputChannel(){}
    AsyncInputChannel(const AsyncInputChannel&) = delete;
    AsyncInputChannel(AsyncInputChannel&&) = delete;
    AsyncInputChannel& operator=(const AsyncInputChannel&) = delete;

    virtual void Receive(std::size_t size, ReceiveCallback) = 0;
  };


  class OutputChannel
  {
  public:
    typedef std::shared_ptr<OutputChannel> SharedPtr;
    typedef std::unique_ptr<OutputChannel> UniquePtr;

  public:
    virtual ~OutputChannel(){}

    OutputChannel(){}
    OutputChannel(const OutputChannel&) = delete;
    OutputChannel(OutputChannel&&) = delete;
    OutputChannel& operator=(const OutputChannel&) = delete;

   public:
    virtual void Send(const char* message, std::size_t size) = 0;
  };


  typedef std::function<void(const std::system_error& err)> SendDataCallback;

  class AsyncOutputChannel
  {
  public:
    typedef std::shared_ptr<AsyncOutputChannel> SharedPtr;
    typedef std::unique_ptr<AsyncOutputChannel> UniquePtr;

  public:
    virtual ~AsyncOutputChannel(){}

    AsyncOutputChannel(){}
    AsyncOutputChannel(const OutputChannel&) = delete;
    AsyncOutputChannel(OutputChannel&&) = delete;
    AsyncOutputChannel& operator=(const OutputChannel&) = delete;

   public:
    virtual void Send(const char* message, std::size_t size, SendDataCallback callback) = 0;
  };


  class IOChannel :
    public InputChannel,
    public OutputChannel
  {
  public:
    typedef std::shared_ptr<IOChannel> SharedPtr;
    typedef std::unique_ptr<IOChannel> UniquePtr;
  };

  class AsyncIO :
    public AsyncInputChannel,
    public AsyncOutputChannel
  {
  public:
    typedef std::shared_ptr<AsyncIO> SharedPtr;
    typedef std::unique_ptr<AsyncIO> UniquePtr;
  };

}

