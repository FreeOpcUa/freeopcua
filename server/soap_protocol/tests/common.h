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

#include "../src/serialization/serialize.h"
#include "../src/serialization/deserialize.h"

#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <gtest/gtest.h>

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
      header.UtcTime = OpcUa::ToDateTime(6);
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


#define ASSERT_NODE_ID_EQ(id, data) \
  ASSERT_NE(id, nullptr); \
  ASSERT_NE(id->Identifier, nullptr); \
  ASSERT_EQ(*id->Identifier, data);

    inline OpcUa::DiagnosticInfo CreateDiagnosticInfo()
    {
      OpcUa::DiagnosticInfo diagnostic;
      unsigned mask =
          OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO |
          OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE |
          OpcUa::DiagnosticInfoMask::DIM_LOCALE |
          OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT |
          OpcUa::DiagnosticInfoMask::DIM_NAMESPACE |
          OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID |
          OpcUa::DiagnosticInfoMask::DIM_INNER_DIAGNOSTIC_INFO;
      diagnostic.EncodingMask = static_cast<OpcUa::DiagnosticInfoMask>(mask);
      diagnostic.AdditionalInfo = "additional";
      diagnostic.InnerStatusCode = OpcUa::StatusCode::BadNotImplemented;
      diagnostic.Locale = 1;
      diagnostic.LocalizedText = 2;
      diagnostic.NamespaceURI = 3;
      diagnostic.SymbolicID = 4;
      diagnostic.InnerDiagnostics.reset(new DiagnosticInfo());
      return diagnostic;
    }

    inline OpcUa::DiagnosticInfoList CreateDiagnosticInfoList()
    {
      OpcUa::DiagnosticInfoList diags;
      diags.push_back(CreateDiagnosticInfo());
      diags.push_back(CreateDiagnosticInfo());
      return diags;
    }

    inline ResponseHeader CreateResponseHeader()
    {
      ResponseHeader header;
      header.Additional.Encoding = 1;
      header.Additional.TypeID = OpcUa::NumericNodeID(2);
      header.InnerDiagnostics = CreateDiagnosticInfo();
      header.RequestHandle = 3;
      header.ServiceResult = OpcUa::StatusCode::BadNotReadable;
      header.StringTable.push_back("table");
      header.Timestamp = OpcUa::ToDateTime(4);

      return header;
    }

    inline void AssertDiagnosticInfoValid(const ns3__DiagnosticInfo* diag)
    {
      ASSERT_NE(diag, nullptr);
      ASSERT_NE(diag->AdditionalInfo, nullptr);
      ASSERT_EQ(*diag->AdditionalInfo, "additional");

      ASSERT_NE(diag->InnerStatusCode, nullptr);
      ASSERT_NE(diag->InnerStatusCode->Code, nullptr);
      ASSERT_FALSE(diag->InnerStatusCode->Code->empty());

      ASSERT_NE(diag->Locale, nullptr);
      ASSERT_EQ(*diag->Locale, 1);

      ASSERT_NE(diag->LocalizedText, nullptr);
      ASSERT_EQ(*diag->LocalizedText, 2);

      ASSERT_NE(diag->NamespaceURI, nullptr);
      ASSERT_EQ(*diag->NamespaceURI, 3);

      ASSERT_NE(diag->SymbolicId, nullptr);
      ASSERT_EQ(*diag->SymbolicId, 4);

      ASSERT_EQ(diag->InnerDiagnosticInfo->AdditionalInfo, nullptr);
      ASSERT_EQ(diag->InnerDiagnosticInfo->InnerStatusCode, nullptr);
      ASSERT_EQ(diag->InnerDiagnosticInfo->Locale, nullptr);
      ASSERT_EQ(diag->InnerDiagnosticInfo->LocalizedText, nullptr);
      ASSERT_EQ(diag->InnerDiagnosticInfo->NamespaceURI, nullptr);
      ASSERT_EQ(diag->InnerDiagnosticInfo->SymbolicId, nullptr);
      ASSERT_EQ(diag->InnerDiagnosticInfo->InnerDiagnosticInfo, nullptr);
    }

    inline void AssertDiagnosticInfoListValid(const ns3__ListOfDiagnosticInfo* diags)
    {
      ASSERT_NE(diags, nullptr);
      ASSERT_EQ(diags->DiagnosticInfo.size(), 2);
      std::for_each(diags->DiagnosticInfo.begin(), diags->DiagnosticInfo.end(), AssertDiagnosticInfoValid);
    }

    inline void AssertResponseHeaderValid(const ns3__ResponseHeader* header)
    {
      ASSERT_NE(header, nullptr);
      ASSERT_NE(header->AdditionalHeader, nullptr);
      ASSERT_NE(header->AdditionalHeader->Body, nullptr);
      ASSERT_NODE_ID_EQ(header->AdditionalHeader->TypeId, "ns=0;i=2;");
      AssertDiagnosticInfoValid(header->ServiceDiagnostics);

      ASSERT_EQ(header->RequestHandle, 3);
      ASSERT_NE(header->ServiceResult, nullptr);
      ASSERT_NE(header->ServiceResult->Code, nullptr);
      ASSERT_FALSE(header->ServiceResult->Code->empty());
      ASSERT_NE(header->StringTable, nullptr);
      ASSERT_EQ(header->StringTable->String.size(), 1);
      ASSERT_EQ(header->StringTable->String[0], "table");
      ASSERT_EQ(header->Timestamp, 4);
    }

    inline void AssertDiagnosticInfoValid(const OpcUa::DiagnosticInfo& diag)
    {
      unsigned mask =
          OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO |
          OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE |
          OpcUa::DiagnosticInfoMask::DIM_LOCALE |
          OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT |
          OpcUa::DiagnosticInfoMask::DIM_NAMESPACE |
          OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID |
          OpcUa::DiagnosticInfoMask::DIM_INNER_DIAGNOSTIC_INFO;

      ASSERT_EQ(diag.AdditionalInfo, "additional");
      ASSERT_EQ(diag.InnerStatusCode, OpcUa::StatusCode::BadNotImplemented);
      ASSERT_EQ(diag.Locale, 1);
      ASSERT_EQ(diag.LocalizedText, 2);
      ASSERT_EQ(diag.NamespaceURI, 3);
      ASSERT_EQ(diag.SymbolicID, 4);
      ASSERT_EQ(diag.EncodingMask, mask);
      ASSERT_TRUE(static_cast<bool>(diag.InnerDiagnostics));
      ASSERT_EQ(diag.InnerDiagnostics->EncodingMask, OpcUa::DiagnosticInfoMask::DIM_NONE);
    }

    inline void AssertDiagnosticInfoListValid(const OpcUa::DiagnosticInfoList& diags)
    {
      ASSERT_EQ(diags.size(), 2);
      AssertDiagnosticInfoValid(diags[0]);
      AssertDiagnosticInfoValid(diags[1]);
    }

#define ASSERT_RESPONSE_HEADER_EQ(h1, h2) \
    ASSERT_EQ(h1.Additional.TypeID, h2.Additional.TypeID); \
    ASSERT_EQ(h1.RequestHandle,     h2.RequestHandle); \
    ASSERT_EQ(h1.ServiceResult,     h2.ServiceResult); \
    ASSERT_EQ(h1.StringTable,       h2.StringTable); \
    ASSERT_EQ(h1.Timestamp,         h2.Timestamp); \
    ASSERT_EQ(h1.InnerDiagnostics.AdditionalInfo,  h2.InnerDiagnostics.AdditionalInfo); \
    ASSERT_EQ(h1.InnerDiagnostics.InnerStatusCode, h2.InnerDiagnostics.InnerStatusCode); \
    ASSERT_EQ(h1.InnerDiagnostics.Locale,          h2.InnerDiagnostics.Locale); \
    ASSERT_EQ(h1.InnerDiagnostics.LocalizedText,   h2.InnerDiagnostics.LocalizedText); \
    ASSERT_EQ(h1.InnerDiagnostics.NamespaceURI,    h2.InnerDiagnostics.NamespaceURI); \
    ASSERT_EQ(h1.InnerDiagnostics.SymbolicID,      h2.InnerDiagnostics.SymbolicID); \
    ASSERT_EQ(h1.InnerDiagnostics.EncodingMask,    h2.InnerDiagnostics.EncodingMask);
  }
}
