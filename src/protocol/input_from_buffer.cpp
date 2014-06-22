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

#include <opc/ua/protocol/input_from_buffer.h>

#include <stdexcept>
#include <cstring>

namespace OpcUa
{

  InputFromBuffer::InputFromBuffer(const char* buf, std::size_t bufSize)
    : Buffer(buf)
    , Size(bufSize)
  {
    if (!Buffer) throw std::invalid_argument("InputFromBuffer: Input buffer cannot be empty.");
    if (!bufSize) throw std::invalid_argument("InputFromBuffer: Invalid size of buffer");
  }

  std::size_t InputFromBuffer::Receive(char* data, std::size_t size)
  {
    if (Size == 0)
    {
      return 0;
    }

    const std::size_t sizeToRead = std::min(Size, size);
    memcpy(data, Buffer, sizeToRead);
    Size -= sizeToRead;
    Buffer += sizeToRead;
    return sizeToRead;
  }

  size_t InputFromBuffer::GetRemainSize() const
  {
    return Size;
  }

}
