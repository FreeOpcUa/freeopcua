/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#pragma once

#include <opc/ua/protocol/channel.h>
#include <boost/asio/streambuf.hpp>

namespace OpcUa
{

class InputFromStreamBuffer : public OpcUa::InputChannel
{
public:
  InputFromStreamBuffer(boost::asio::streambuf & buf);

  virtual std::size_t Receive(char * data, std::size_t size) override;

  size_t GetRemainSize() const;

  virtual void Stop() override {}

private:
		boost::asio::streambuf & Buffer;
};

}
