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

#include <iomanip>
#include <sstream>
#include <vector>

namespace OpcUa
{

inline std::string ToHexDump(const char * buf, std::size_t size)
{
  std::stringstream result;
  std::stringstream lineEnd;
  unsigned pos = 0;
  result << "size: " << size << "(0x" << std::hex << size << ")";

  while (pos < size)
    {
      if ((pos % 16) == 0)
        {
          result << std::endl << std::setfill('0') << std::setw(4) << (pos & 0xFFFF);
          lineEnd.str(std::string());
        }
      if ((pos % 8) == 0)
        {
          result << " ";
          lineEnd << " ";
        }

      const char c = buf[pos];
      result << " " << std::setw(2) << (c & 0xFF);
      lineEnd << ((c > ' ' && c < 0x7f) ? c : '.');

      if ((pos % 16) == 15)
        {
          result << " " << lineEnd.str();
        }
      ++pos;
    }

  result << std::endl << std::flush;
  return result.str();
}

template <typename T> inline std::ostream & ToHexDump(std::ostream & os, const std::vector<T> & buf, std::size_t size)
{
  std::stringstream lineEnd;
  size = std::min(size, buf.size());
  unsigned pos = 0;
  os << "size: " << size << "(0x" << std::hex << size << ")";

  while (pos < size)
    {
      if ((pos % 16) == 0)
        {
          os << std::endl << std::setfill('0') << std::setw(4) << (pos & 0xFFFF);
          lineEnd.str(std::string());
        }
      if ((pos % 8) == 0)
        {
          os << " ";
          lineEnd << " ";
        }

      const char c = buf[pos];
      os << " " << std::setw(2) << (c & 0xFF);
      lineEnd << ((c > ' ' && c < 0x7f) ? c : '.');

      if ((pos % 16) == 15)
        {
          os << " " << lineEnd.str();
        }
      ++pos;
    }

  os << std::endl;
  return os;
}

template <typename T> inline std::ostream & ToHexDump(std::ostream & os, const std::vector<T> & buf)
{
    return ToHexDump(os, buf, buf.size());
}

inline std::string ToHexDump(const std::vector<char> & buf, std::size_t size)
{
  std::stringstream result;
  ToHexDump(result, buf, size);
  return result.str();
}

inline std::string ToHexDump(const std::vector<char> & buf)
{
  return ToHexDump(buf, buf.size());
}


}
