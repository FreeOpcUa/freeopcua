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

#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/utils.h>
#include <opc/ua/protocol/view.h>
#include <ostream>
#include <limits>

namespace OpcUa
{

std::string ToString(const NodeId & id);
std::string ToString(const Guid & guid);
std::string ToString(const DateTime & t);
std::string ToString(const BrowseDirection & direction);
std::string ToString(const StatusCode & code);
std::string ToString(const LocalizedText & code);
std::string ToString(const AttributeId & value);
std::string ToString(const ObjectId & value);

Guid ToGuid(const std::string & str);
NodeId ToNodeId(const std::string & str, uint32_t defaultNamespace = 0);
QualifiedName ToQualifiedName(const std::string & str, uint32_t default_ns = 0);

inline std::ostream & operator<<(std::ostream & os, const OpcUa::NodeId & nodeid)
{
  os << OpcUa::ToString(nodeid).c_str();
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::LocalizedText & t)
{
  os << OpcUa::ToString(t).c_str();
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::QualifiedName & qn)
{
  os << "QualifiedName(" << qn.NamespaceIndex << ":" << qn.Name.c_str() << ")";
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::BrowseDirection & direction)
{
  os << OpcUa::ToString(direction);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::Guid & guid)
{
  os << "{" << ToString(guid) << "}";
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ByteString & value)
{
  return ToHexDump(os, value.Data);
}
}

