/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/ua/protocol/types.h>

#include <gtest/gtest.h>
#include "../src/serialization/serialize.h"
#include "../src/serialization/deserialize.h"

namespace OpcUa
{
  namespace Test
  {

    inline OpcUa::RequestHeader CreateRequestHeader()
    {
      OpcUa::RequestHeader header;
      header.Additional.Encoding = 1;
      header.Additional.TypeID = OpcUa::NumericNodeID(2);
      header.AuditEntryID = "audit";
      header.RequestHandle = 3;
      header.ReturnDiagnostics = 4;
      header.SessionAuthenticationToken = OpcUa::NumericNodeID(3);
      header.Timeout = 5;
      header.UtcTime = 6;
      return header;
    }

    inline void AssertRequestHeaderValid(ns3__RequestHeader* header)
    {
      ASSERT_NE(header, nullptr);
      ASSERT_NE(header->AdditionalHeader, nullptr);
      ASSERT_NE(header->AdditionalHeader->Body, nullptr);
      ASSERT_NE(header->AdditionalHeader->TypeId, nullptr);
      ASSERT_EQ(*header->AdditionalHeader->TypeId->Identifier, "ns=0;i=2;");

      ASSERT_NE(header, nullptr);
      ASSERT_NE(header->AuditEntryId, nullptr);
      ASSERT_EQ(*header->AuditEntryId, "audit");

      ASSERT_EQ(header->RequestHandle, 3);
      ASSERT_EQ(header->ReturnDiagnostics, 4);

      ASSERT_NE(header->AuthenticationToken, nullptr);
      ASSERT_NE(header->AuthenticationToken->Identifier, nullptr);
      ASSERT_EQ(*header->AuthenticationToken->Identifier, "ns=0;i=3;");

      ASSERT_EQ(header->TimeoutHint, 5);
      ASSERT_EQ(header->Timestamp, 6);
    }

    inline void AssertRequestHeaderEq(const RequestHeader& header1, const RequestHeader& header2)
    {
      //TODO  ASSERT_EQ(deserialized.Header.Additional.Encoding, opcua.Header.Additional.Encoding);
      ASSERT_EQ(header1.Additional.TypeID, header2.Additional.TypeID);
      ASSERT_EQ(header1.SessionAuthenticationToken, header2.SessionAuthenticationToken);
      ASSERT_EQ(header1.AuditEntryID, header2.AuditEntryID);
      ASSERT_EQ(header1.RequestHandle, header2.RequestHandle);
      ASSERT_EQ(header1.ReturnDiagnostics, header2.ReturnDiagnostics);
      ASSERT_EQ(header1.Timeout, header2.Timeout);
      ASSERT_EQ(header1.UtcTime, header2.UtcTime);
    }

    inline bool IsEqualNodeID(ns3__NodeId* id, const char* data)
    {
      return id != nullptr && id->Identifier != nullptr && *id->Identifier == data;
    }

  }

}
