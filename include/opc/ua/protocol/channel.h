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

  class BreakableChannel
  {
  public:
    virtual ~BreakableChannel(){}
    BreakableChannel(){}
    BreakableChannel(const BreakableChannel&) = delete;
    BreakableChannel(BreakableChannel&&) = delete;
    BreakableChannel& operator=(const BreakableChannel&) = delete;

  public:
    virtual void Stop() = 0;
  };


  class InputChannel : public virtual BreakableChannel
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


  class OutputChannel : public virtual BreakableChannel
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


  class IOChannel :
    public InputChannel,
    public OutputChannel
  {
  public:
    typedef std::shared_ptr<IOChannel> SharedPtr;
    typedef std::unique_ptr<IOChannel> UniquePtr;
  };

}

