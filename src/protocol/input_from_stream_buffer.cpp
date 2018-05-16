/******************************************************************************
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

#include <opc/ua/protocol/input_from_stream_buffer.h>

#include <algorithm>

namespace OpcUa
{

InputFromStreamBuffer::InputFromStreamBuffer(boost::asio::streambuf & buf) : Buffer(buf) 
{

}

std::size_t InputFromStreamBuffer::Receive(char * data, std::size_t size)
{
  size_t bufferSize = GetRemainSize();
  if (bufferSize == 0)
  {
    return 0;
  }

  const std::size_t sizeToRead = std::min(bufferSize, size);
  Buffer.sgetn(data, sizeToRead);
  return sizeToRead;
}

size_t InputFromStreamBuffer::GetRemainSize() const {
  return Buffer.size();
}

}
