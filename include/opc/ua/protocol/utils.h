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

#include <algorithm>
#include <sstream>
#include <vector>

namespace OpcUa
{

inline std::string ToHexDump(const std::vector<char> & buf, std::size_t size)
{
  std::stringstream result;
  size = std::min(size, buf.size());
  unsigned pos = 0;
  result << "Data size: " << size << std::endl;

  while (pos < size)
    {
      if (pos)
        { printf((pos % 16 == 0) ? "\n" : " "); }

      const char letter = buf[pos];
      printf("%02x", (unsigned)letter & 0x000000FF);

      if (letter > ' ')
        { result << "(" << letter << ")"; }

      else
        { result << "   "; }

      ++pos;
    }

  result << std::endl << std::flush;
  return result.str();
}

inline std::string ToHexDump(const std::vector<char> & buf)
{
  return ToHexDump(buf, buf.size());
}


}
