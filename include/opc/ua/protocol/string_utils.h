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

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/view.h>
#include <ostream>
#include <limits>

namespace OpcUa
{

  std::string ToString(const NodeID& id);
  std::string ToString(const Guid& guid);
  std::string ToString(const DateTime& t);
  std::string ToString(const BrowseDirection& direction);
  std::string ToString(const StatusCode& code);

  Guid ToGuid(const std::string& str);
  NodeID ToNodeID(const std::string& str, uint32_t defaultNamespace=std::numeric_limits<uint32_t>::max());
  QualifiedName ToQualifiedName(const std::string& str, uint32_t default_ns=std::numeric_limits<uint32_t>::max());

  inline std::ostream& operator<<(std::ostream& os, const OpcUa::NodeID& nodeid)
  {
    os << OpcUa::ToString(nodeid).c_str();
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const OpcUa::QualifiedName& qn)
  {
     os << "QualifiedName(" << qn.NamespaceIndex << ":" << qn.Name.c_str() << ")";
     return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const OpcUa::BrowseDirection& direction)
  {
     os << OpcUa::ToString(direction);
     return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const OpcUa::Guid& guid)
  {
     os << "{" << ToString(guid) << "}";
     return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const OpcUa::ByteString& buf)
  {
    const std::size_t size = buf.Data.size();
    unsigned pos = 0;
    os << "Data size: " << size << std::endl;
    while (pos < size)
    {
      if (pos)
        printf((pos % 16 == 0) ? "\n" : " ");

      const char letter = buf.Data[pos];
      printf("%02x", (unsigned)letter & 0x000000FF);

      if (letter > ' ')
        os << "(" << letter << ")";
      else
        os << "   ";

      ++pos;
    }

    os << std::endl;
    return os;
  }
}

