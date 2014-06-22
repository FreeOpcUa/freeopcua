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
#include <sstream>
#include <cstring>

std::string OpcUa::ToString(const NodeID& id)
{
  std::stringstream stream;

  if (id.HasServerIndex())
  {
    stream << "srv=" << id.ServerIndex << ";";
  }
  {
  if (id.HasNamespaceURI())
    stream << "nsu=" << id.NamespaceURI << ";";
  }

  stream << "ns=" << id.GetNamespaceIndex() << ";";
  if (id.IsInteger())
  {
    stream << "i=" << id.GetIntegerIdentifier() << ";";
  }
#ifndef __ENABLE_EMBEDDED_PROFILE__
  else if(id.IsString())
  {
    stream << "s=" << id.GetStringIdentifier() << ";";
  }
  else if (id.IsGuid())
  {
    stream << "g=" << ToString(id.GetGuidIdentifier()) << ";";
  }
#endif

  return stream.str();
}


std::string OpcUa::ToString(const OpcUa::Guid& guid)
{
  char buf[36] = {0};
  sprintf(buf, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return buf;
}

OpcUa::Guid OpcUa::ToGuid(const std::string& str)
{
  if (str.size() != 35)
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
  const int parts = sscanf(str.c_str(), "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
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
  std::string GetNodeField(const std::string& data, const char* field)
  {
    std::size_t colon = 0;
    do
    {
      // if something foud at previous cycle
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
    while(data[colon - 1] != ';');

    colon += std::strlen(field);
    const std::size_t semicolon = data.find(";", colon);
    if (semicolon == std::string::npos)
    {
      data.substr(colon);
    }
    return data.substr(colon, semicolon - colon);
  }

  uint32_t GetInteger(const std::string&value)
  {
    if (value.empty())
    {
      return 0;
    }
    return std::stoul(value);
  }

}

OpcUa::NodeID OpcUa::ToNodeID(const std::string& data, uint32_t defaultNamespace)
{
  OpcUa::NodeID result;

  uint32_t ns = defaultNamespace;

  const std::string nsString = GetNodeField(data, "ns=");
  if (!nsString.empty())
  {
    ns = GetInteger(nsString);
  }

  const std::string integer = GetNodeField(data, "i=");
  if (!integer.empty())
  {
    result = OpcUa::NumericNodeID(GetInteger(integer), ns);
  }

  const std::string str = GetNodeField(data, "s=");
  if (!str.empty())
  {
    result = OpcUa::StringNodeID(str, ns);
  }

  const std::string g = GetNodeField(data, "g=");
  if (!g.empty())
  {
    result = OpcUa::GuidNodeID(ToGuid(g), ns);
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

  return result;
}

OpcUa::QualifiedName OpcUa::ToQualifiedName(const std::string& str, uint16_t default_ns)
{
  std::size_t found = str.find(":");
  if (found != std::string::npos)
  {
    uint16_t ns = std::stoi(str.substr(0, found));
    std::string name = str.substr(found+1, str.length() - found);
    return QualifiedName(ns, name);
  }

  return QualifiedName(default_ns, str);
}
