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
#include <opc/ua/protocol/protocol_auto.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/utils.h>
#include <opc/ua/protocol/types_manual.h>
#include <opc/ua/protocol/view.h>
#include <ostream>
#include <limits>

namespace OpcUa
{

class Event;
  
std::string ToString(const AttributeId & value);
std::string ToString(const BrowseDirection & direction);
std::string ToString(const DateTime & t);
std::string ToString(const DataChangeTrigger & value);
std::string ToString(const DeadbandType & value);
std::string ToString(const Event & value, bool showAll = false);
std::string ToString(const ExtensionObjectEncoding & value);
std::string ToString(const FilterOperator & value);
std::string ToString(const Guid & guid);
std::string ToString(const LocalizedText & code);
std::string ToString(const MonitoringMode & value);
std::string ToString(const NodeId & id, bool addObjectIdName = false);
std::string ToString(const ObjectId & value);
std::string ToString(const StatusCode & code);
std::string ToString(const TimestampsToReturn & value);
std::string ToString(const VariantType & value);

std::ostream & indent(std::ostream & os, int level, bool noSeparator = false);
std::ostream & ToStream(std::ostream & os, const AggregateFilter & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const ContentFilterElement & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const DataChangeFilter & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const ExtensionObjectHeader & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const Event & value, int indentLevel = 0, bool showAll = false);
std::ostream & ToStream(std::ostream & os, const EventFilter & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const FilterOperand & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const MonitoringFilter & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const MonitoringParameters & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const MonitoredItemCreateRequest & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const MonitoredItemsParameters & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const NodeId & value, bool addObjectIdName = false);
std::ostream & ToStream(std::ostream & os, const std::string & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const std::vector<QualifiedName> & value);
std::ostream & ToStream(std::ostream & os, const ReadValueId & value, int indentLevel = 0);
std::ostream & ToStream(std::ostream & os, const SimpleAttributeOperand & value, int indentLevel = 0);

template <typename T>
std::ostream & ToStream(std::ostream & os, const std::vector<T> & value, int indentLevel = 0)
{
  os << "{";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;
  bool isFirst = true;
  for (auto & element : value)
    {
      indent(os, subIndentLevel, isFirst);
      ToStream(os, element, subIndentLevel);
      isFirst = false;
    }
  indent(os, indentLevel, true);
  os << "}";
  return os;
}

template <typename T>
std::string ToString(const T & value)
{
  std::stringstream result;
  ToStream(result, value);
  return result.str();
}

Guid ToGuid(const std::string & str);
NodeId ToNodeId(const std::string & str, uint32_t defaultNamespace = 0);
QualifiedName ToQualifiedName(const std::string & str, uint32_t default_ns = 0);

inline std::ostream & operator<<(std::ostream & os, const OpcUa::AggregateFilter & value)
{
  return OpcUa::ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::AttributeId & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::BrowseDirection & direction)
{
  os << OpcUa::ToString(direction);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ByteString & value)
{
  return ToHexDump(os, value.Data);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ContentFilterElement & value)
{
  return OpcUa::ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::DataChangeTrigger & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::DataChangeFilter & value)
{
  return OpcUa::ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::DateTime & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::DeadbandType & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ExtensionObjectEncoding & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ExtensionObjectHeader & value)
{
  return OpcUa::ToStream(os, value);
}
  
inline std::ostream & operator<<(std::ostream & os, const Event & value)
{
  return OpcUa::ToStream(os, value, -1, false);
}
  
inline std::ostream & operator<<(std::ostream & os, const OpcUa::EventFilter & value)
{
  return OpcUa::ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::FilterOperand & value)
{
  return OpcUa::ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::FilterOperator & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::Guid & guid)
{
  os << "{" << ToString(guid) << "}";
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::LocalizedText & t)
{
  os << OpcUa::ToString(t).c_str();
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::MonitoringFilter & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::MonitoringMode & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::MonitoringParameters & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::MonitoredItemCreateRequest & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::MonitoredItemsParameters & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::NodeId & nodeid)
{
  os << OpcUa::ToString(nodeid).c_str();
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ObjectId & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::QualifiedName & qn)
{
  os << "QualifiedName(" << qn.NamespaceIndex << ":" << qn.Name.c_str() << ")";
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const std::vector<OpcUa::QualifiedName> & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::ReadValueId & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::SimpleAttributeOperand & value)
{
  return ToStream(os, value);
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::StatusCode & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::TimestampsToReturn & value)
{
  os << OpcUa::ToString(value);
  return os;
}

inline std::ostream & operator<<(std::ostream & os, const OpcUa::VariantType & value)
{
  os << OpcUa::ToString(value);
  return os;
}
}

