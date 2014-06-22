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
#include <memory>


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
    /// @brief Wait for Data
    /// @return 1 if data, 0 if timetout and <0 if error
    virtual int WaitForData(float second) = 0;
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

  class IOChannel :
    public InputChannel,
    public OutputChannel
  {
  public:
    typedef std::shared_ptr<IOChannel> SharedPtr;
    typedef std::unique_ptr<IOChannel> UniquePtr;
  };

}

