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

#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/event.h>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <limits>


std::string OpcUa::ToString(const OpcUa::BrowseDirection & direction)
{
  switch (direction)
    {
    case OpcUa::BrowseDirection::Forward:
      return "forward";

    case OpcUa::BrowseDirection::Inverse:
      return "inverse";

    case OpcUa::BrowseDirection::Both:
      return "both";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const OpcUa::DateTime & t)
{
  std::time_t st = OpcUa::DateTime::ToTimeT(t);
  return std::ctime(&st);
}

std::string OpcUa::ToString(const OpcUa::DataChangeTrigger & value)
{
  switch (value)
    {
    case OpcUa::DataChangeTrigger::Status:
      return "Status";

    case OpcUa::DataChangeTrigger::StatusValue:
      return "StatusValue";

    case OpcUa::DataChangeTrigger::StatusValueTimestamp:
      return "StatusValueTimestamp";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const OpcUa::DeadbandType & value)
{
  switch (value)
    {
    case OpcUa::DeadbandType::None:
      return "None";

    case OpcUa::DeadbandType::Absolute:
      return "Absolute";

    case OpcUa::DeadbandType::Percent:
      return "Percent";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const OpcUa::Event & value, bool showAll)
{
  std::stringstream result;
  ToStream(result, value, -1, showAll);
  return result.str();
}


std::string OpcUa::ToString(const OpcUa::ExtensionObjectEncoding & value)
{
  switch (value)
    {
    case OpcUa::ExtensionObjectEncoding::NONE:
      return "NONE";

    case OpcUa::ExtensionObjectEncoding::HAS_BINARY_BODY:
      return "HAS_BINARY_BODY";

    case OpcUa::ExtensionObjectEncoding::HAS_XML_BODY:
      return "HAS_XML_BODY";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const OpcUa::FilterOperator & value)
{
  switch (value)
    {
    case OpcUa::FilterOperator::Equals:
      return "Equals";

    case OpcUa::FilterOperator::IsNull:
      return "IsNull";

    case OpcUa::FilterOperator::GreaterThan:
      return "GreaterThan";

    case OpcUa::FilterOperator::LessThan:
      return "LessThan";

    case OpcUa::FilterOperator::GreaterThanOrEqual:
      return "GreaterThanOrEqual";

    case OpcUa::FilterOperator::LessThanOrEqual:
      return "LessThanOrEqual";

    case OpcUa::FilterOperator::Like:
      return "Like";

    case OpcUa::FilterOperator::Not:
      return "Not";

    case OpcUa::FilterOperator::Between:
      return "Between";

    case OpcUa::FilterOperator::InList:
      return "InList";

    case OpcUa::FilterOperator::And:
      return "And";

    case OpcUa::FilterOperator::Or:
      return "Or";

    case OpcUa::FilterOperator::Cast:
      return "Cast";

    case OpcUa::FilterOperator::InView:
      return "InView";

    case OpcUa::FilterOperator::OfType:
      return "OfType";

    case OpcUa::FilterOperator::RelatedTo:
      return "RelatedTo";

    case OpcUa::FilterOperator::BitwiseAnd:
      return "BitwiseAnd";

    case OpcUa::FilterOperator::BitwiseOr:
      return "BitwiseOr";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const OpcUa::LocalizedText & t)
{
  return t.Text;
}

std::string OpcUa::ToString(const OpcUa::Guid & guid)
{
  char buf[37] = {0};
  sprintf(buf, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return buf;
}

std::string OpcUa::ToString(const OpcUa::MonitoringMode & value)
{
  switch (value)
    {
    case OpcUa::MonitoringMode::Disabled:
      return "Disabled";

    case OpcUa::MonitoringMode::Sampling:
      return "Sampling";

    case OpcUa::MonitoringMode::Reporting:
      return "Reporting";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const NodeId & id, bool addObjectIdName)
{
  std::stringstream result;
  ToStream(result, id, addObjectIdName);
  return result.str();
}

std::string OpcUa::ToString(const OpcUa::TimestampsToReturn & value)
{
  switch (value)
    {
    case OpcUa::TimestampsToReturn::Source:
      return "Source";

    case OpcUa::TimestampsToReturn::Server:
      return "Server";

    case OpcUa::TimestampsToReturn::Both:
      return "Both";

    case OpcUa::TimestampsToReturn::Neither:
      return "Neither";

    default:
      return "unknown";
    }
}

std::string OpcUa::ToString(const OpcUa::VariantType & value)
{
  switch (value)
    {
    case OpcUa::VariantType::NUL:
      return "NUL";

    case OpcUa::VariantType::BOOLEAN:
      return "BOOLEAN";

    case OpcUa::VariantType::SBYTE:
      return "SBYTE";

    case OpcUa::VariantType::BYTE:
      return "BYTE";

    case OpcUa::VariantType::INT16:
      return "INT16";

    case OpcUa::VariantType::UINT16:
      return "UINT16";

    case OpcUa::VariantType::INT32:
      return "INT32";

    case OpcUa::VariantType::UINT32:
      return "UINT32";

    case OpcUa::VariantType::INT64:
      return "INT64";

    case OpcUa::VariantType::UINT64:
      return "UINT64";

    case OpcUa::VariantType::FLOAT:
      return "FLOAT";

    case OpcUa::VariantType::DOUBLE:
      return "DOUBLE";

    case OpcUa::VariantType::STRING:
      return "STRING";

    case OpcUa::VariantType::DATE_TIME:
      return "DATE_TIME";

    case OpcUa::VariantType::GUId:
      return "GUId";

    case OpcUa::VariantType::BYTE_STRING:
      return "BYTE_STRING";

    case OpcUa::VariantType::XML_ELEMENT:
      return "XML_ELEMENT";

    case OpcUa::VariantType::NODE_Id:
      return "NODE_Id";

    case OpcUa::VariantType::EXPANDED_NODE_Id:
      return "EXPANDED_NODE_Id";

    case OpcUa::VariantType::STATUS_CODE:
      return "STATUS_CODE";

    case OpcUa::VariantType::QUALIFIED_NAME:
      return "QUALIFIED_NAME";

    case OpcUa::VariantType::LOCALIZED_TEXT:
      return "LOCALIZED_TEXT";

    case OpcUa::VariantType::EXTENSION_OBJECT:
      return "EXTENSION_OBJECT";

    case OpcUa::VariantType::DATA_VALUE:
      return "DATA_VALUE";

    case OpcUa::VariantType::VARIANT:
      return "VARIANT";

    case OpcUa::VariantType::DIAGNOSTIC_INFO:
      return "DIAGNOSTIC_INFO";

    default:
      return "unknown";
    }
}

std::ostream & OpcUa::indent(std::ostream & os, int indent, bool noSeparator)
{
  if (indent >= 0)
    {
      os << std::endl;
    }
  else if (!noSeparator)
    {
      os << ", ";
    }
  while (indent > 0)
    {
      os << "  ";
      --indent;
    }
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::AggregateFilter & value, int indentLevel)
{
  os << "AggregateFilter(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;
  indent(os, subIndentLevel, true);
  os << "...";
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::ContentFilterElement & value, int indentLevel)
{
  os << "ContentFilterElement(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "Operator: " << ToString(value.Operator);

  indent(os, subIndentLevel);
  os << "FilterOperands: ";
  ToStream(os, value.FilterOperands, subIndentLevel);

  indent(os, subIndentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::DataChangeFilter & value, int indentLevel)
{
  os << "DataChangeFilter(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "Trigger: " << ToString(value.Trigger);

  indent(os, subIndentLevel);
  os << "Deadband: " << ToString(value.Deadband);

  indent(os, subIndentLevel);
  os << "DeadbandValue: " << value.DeadbandValue;

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::ExtensionObjectHeader & value, int indentLevel)
{
  os << "ExtensionObjectHeader(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "TypeId: ";
  ToStream(os, value.TypeId, true);

  indent(os, subIndentLevel);
  os << "Encoding: " << value.Encoding;

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::Event & value, int indentLevel, bool showAll)
{
  os << "Event(";
  int subIndentLevel = (indentLevel < 0)
  ? indentLevel
  : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "EventType: ";
  ToStream(os, value.EventType, true);

  indent(os, subIndentLevel);
  os << "Time: " << value.Time;

  if (showAll)
    {
      indent(os, subIndentLevel);
      os << "LocalTime: " << value.LocalTime;

      indent(os, subIndentLevel);
      os << "ReceiveTime: " << value.ReceiveTime;

      indent(os, subIndentLevel);
      os << "EventId: " << value.EventId;

      indent(os, subIndentLevel);
      os << "SourceName: " << value.SourceName;
    }

  indent(os, subIndentLevel);
  os << "SourceNode: ";
  ToStream(os, value.SourceNode);

  indent(os, subIndentLevel);
  os << "Severity: " << value.Severity;

  indent(os, subIndentLevel);
  os << "Message: " << value.Message;

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::EventFilter & value, int indentLevel)
{
  os << "EventFilter(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "SelectClauses: ";
  ToStream(os, value.SelectClauses, subIndentLevel);

  indent(os, subIndentLevel);
  os << "WhereClause: ";
  ToStream(os, value.WhereClause, subIndentLevel);

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::FilterOperand & value, int indentLevel)
{
  os << "FilterOperand(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "Header: ";
  ToStream(os, value.Header, subIndentLevel);

  indent(os, subIndentLevel);
  os << "Element: ...";
//  ToStream(os, value.Element, subIndentLevel);

  indent(os, subIndentLevel);
  os << "Literal: ...";
//  ToStream(os, value.Literal, subIndentLevel);

  indent(os, subIndentLevel);
  os << "Attribute: ";
//  ToStream(os, value.Attribute, subIndentLevel);

  indent(os, subIndentLevel);
  os << "SimpleAttribute: ";
  ToStream(os, value.SimpleAttribute, subIndentLevel);

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::MonitoredItemCreateRequest & value, int indentLevel)
{
  os << "MonitoredItemCreateRequest(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "ItemToMonitor: ";
  ToStream(os, value.ItemToMonitor, subIndentLevel);

  indent(os, subIndentLevel);
  os << "MonitoringMode: " << value.MonitoringMode;

  indent(os, subIndentLevel);
  os << "RequestedParameters: ";
  ToStream(os, value.RequestedParameters, subIndentLevel);

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::MonitoredItemsParameters & value, int indentLevel)
{
  os << "MonitoredItemsParameters(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "SubscriptionId: " << value.SubscriptionId;

  indent(os, subIndentLevel);
  os << "TimestampsToReturn: " << value.TimestampsToReturn;

  indent(os, subIndentLevel);
  os << "ItemsToCreate: ";
  ToStream(os, value.ItemsToCreate, subIndentLevel);

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::MonitoringParameters & value, int indentLevel)
{
  os << "MonitoringParameters(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "ClientHandle: " << value.ClientHandle;

  indent(os, subIndentLevel, true);
  os << "SamplingInterval: " << value.SamplingInterval;

  indent(os, subIndentLevel);
  os << "Filter: ";
  ToStream(os, value.Filter, subIndentLevel);

  indent(os, subIndentLevel, true);
  os << "QueueSize: " << value.QueueSize;

  indent(os, subIndentLevel, true);
  os << "DiscardOldest: " << value.DiscardOldest;

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::MonitoringFilter & value, int indentLevel)
{
  os << "MonitoringFilter(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "Header: ";
  ToStream(os, value.Header, subIndentLevel);

  indent(os, subIndentLevel);
  os << "DataChange: ";
  ToStream(os, value.DataChange, subIndentLevel);

  indent(os, subIndentLevel);
  os << "Event: ";
  ToStream(os, value.Event, subIndentLevel);

  indent(os, subIndentLevel);
  os << "Aggregate: ";
  ToStream(os, value.Aggregate, subIndentLevel);

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::NodeId & value, bool addObjectIdName)
{
  if (value.HasServerIndex())
    {
      os << "srv=" << value.ServerIndex << ";";
    }

  if (value.HasNamespaceURI())
    {
      os << "nsu=" << value.NamespaceURI << ";";
    }

  os << "ns=" << value.GetNamespaceIndex() << ";";

  if (value.IsInteger())
    {
      int id = value.GetIntegerIdentifier();
      os << "i=" << id << ";";
      if (addObjectIdName)
        {
          os << " (" << ToString(ObjectId(id)) << ")";
        }
    }

#ifndef __ENABLE_EMBEDDED_PROFILE__

  else if (value.IsString())
    {
      os << "s=" << value.GetStringIdentifier() << ";";
    }

  else if (value.IsGuid())
    {
      os << "g=" << ToString(value.GetGuidIdentifier()) << ";";
    }

#endif
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const std::vector<OpcUa::QualifiedName> & value)
{
  bool isFirst = true;
  for (auto element : value)
    {
      if (isFirst)
        {
          isFirst = false;
        }
      else
        {
          os << "/";
        }
      os << element.NamespaceIndex << ":" << element.Name;
    }
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const std::string & value, int /*indentLevel*/)
{
  return os << value;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::ReadValueId & value, int indentLevel)
{
  os << "ReadValueId(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;

  indent(os, subIndentLevel, true);
  os << "NodeId: " << value.NodeId;

  indent(os, subIndentLevel);
  os << "AttributeId: " << value.AttributeId;

  indent(os, subIndentLevel);
  os << "IndexRange: " << value.IndexRange;

  indent(os, subIndentLevel);
  os << "DataEncoding: " << value.DataEncoding;

  indent(os, indentLevel, true);
  os << ")";
  return os;
}

std::ostream & OpcUa::ToStream(std::ostream & os, const OpcUa::SimpleAttributeOperand & value, int indentLevel)
{
  os << "SimpleAttributeOperand(";
  int subIndentLevel = (indentLevel < 0)
    ? indentLevel
    : indentLevel + 1;
  indent(os, subIndentLevel, true);
  os << "TypeId: ";
  ToStream(os, value.TypeId, true);
  indent(os, subIndentLevel);
  os << "BrowsePath: ";
  ToStream(os, value.BrowsePath);
  indent(os, subIndentLevel);
  os << "AttributeId: " << ToString(value.Attribute);
  indent(os, subIndentLevel);
  os << "IndexRange: ";
  ToStream(os, value.IndexRange, -1);
  indent(os, indentLevel, true);
  os << ")";
  return os;
}

OpcUa::Guid OpcUa::ToGuid(const std::string & str)
{
  if (str.size() != 36)
    {
      return OpcUa::Guid();
    }

  Guid guid;

  unsigned data1 = 0;
  unsigned data2 = 0;
  unsigned data3 = 0;
  unsigned data4 = 0;
  unsigned data5 = 0;
  unsigned data6 = 0;
  unsigned data7 = 0;
  unsigned data8 = 0;
  unsigned data9 = 0;
  unsigned data10 = 0;
  unsigned data11 = 0;
  const int parts = sscanf(str.c_str(), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                           &data1, &data2, &data3, &data4, &data5, &data6, &data7, &data8, &data9, &data10, &data11);

  guid.Data1    = static_cast<uint32_t>(data1);
  guid.Data2    = static_cast<uint16_t>(data2);
  guid.Data3    = static_cast<uint16_t>(data3);
  guid.Data4[0] = static_cast<uint8_t>(data4);
  guid.Data4[1] = static_cast<uint8_t>(data5);
  guid.Data4[2] = static_cast<uint8_t>(data6);
  guid.Data4[3] = static_cast<uint8_t>(data7);
  guid.Data4[4] = static_cast<uint8_t>(data8);
  guid.Data4[5] = static_cast<uint8_t>(data9);
  guid.Data4[6] = static_cast<uint8_t>(data10);
  guid.Data4[7] = static_cast<uint8_t>(data11);

  if (parts != 11)
    {
      return OpcUa::Guid();
    }

  return guid;
}


namespace
{
std::string GetNodeField(const std::string & data, const char * field)
{
  std::size_t colon = 0;

  do
    {
      // if something found at previous cycle
      if (colon)
        {
          ++colon;
        }

      // find field string
      colon = data.find(field, colon);

      // if found nothing
      if (colon == std::string::npos)
        {
          return std::string();
        }

      // if found at the begin
      if (colon == 0)
        {
          break;
        }
    }

  // if finding 's=' in the 'ns=1;i-3;' will be found field 'ns=1;'
  // check that previous character is the ';' if not then search next.
  while (data[colon - 1] != ';');

  colon += std::strlen(field);
  const std::size_t semicolon = data.find(";", colon);

  if (semicolon == std::string::npos)
    {
      data.substr(colon);
    }

  return data.substr(colon, semicolon - colon);
}

uint32_t GetInteger(const std::string & value)
{
  if (value.empty())
    {
      return 0;
    }

  return std::stoul(value);
}

}

OpcUa::NodeId OpcUa::ToNodeId(const std::string & data, uint32_t defaultNamespace)
{
  OpcUa::NodeId result;
  uint32_t ns = defaultNamespace;

  const std::string nsString = GetNodeField(data, "ns=");

  if (nsString.empty())
    {
      if (ns == std::numeric_limits<uint32_t>::max())
        {
          throw (std::runtime_error("Namespace index coult not be parsed from string and not default index specified in string: " + data));
        }
    }

  else
    {
      ns = GetInteger(nsString);
    }

  const std::string srv = GetNodeField(data, "srv=");

  if (!srv.empty())
    {
      result.SetServerIndex(GetInteger(srv));
    }

  const std::string nsu = GetNodeField(data, "nsu=");

  if (!nsu.empty())
    {
      result.SetNamespaceURI(nsu);
    }

  const std::string integer = GetNodeField(data, "i=");

  if (!integer.empty())
    {
      return OpcUa::NumericNodeId(GetInteger(integer), ns);
    }

  const std::string str = GetNodeField(data, "s=");

  if (!str.empty())
    {
      return OpcUa::StringNodeId(str, ns);
    }

  const std::string g = GetNodeField(data, "g=");

  if (!g.empty())
    {
      return OpcUa::GuidNodeId(ToGuid(g), ns);
    }

  throw (std::runtime_error("No identifier found in string: '" + data + "'"));
}

OpcUa::QualifiedName OpcUa::ToQualifiedName(const std::string & str, uint32_t default_ns)
{
  std::size_t found = str.find(":");

  if (found != std::string::npos)
    {
      uint16_t ns = std::stoi(str.substr(0, found));
      std::string name = str.substr(found + 1, str.length() - found);
      return QualifiedName(ns, name);
    }

  if (default_ns == std::numeric_limits<uint32_t>::max())
    {
      throw (std::runtime_error("Namespace index coult not be parsed from string and not default index specified in string: " + str));
    }

  return QualifiedName(default_ns, str);
}

