/// @author Denis Štogl 2015
/// @email denis.stogl@mailbox.org
/// @brief Opc Ua binary method services.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/method.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/extension_identifiers.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{

  CallMethodRequest::CallMethodRequest()
  {
  }

  CallRequest::CallRequest()
    : TypeId(CALL_REQUEST)
  {
  }

  CallMethodResult::CallMethodResult()
    : Status(StatusCode::Good)
  {
  }

  CallResponse::CallResponse()
    : TypeId(CALL_RESPONSE)
  {
  }


  namespace Binary
  {

    ////////////////////////////////////////////////////////////////////////////
    // CallMethodRequest
    ////////////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<CallMethodRequest>(const CallMethodRequest& desc)
    {
      return RawSize(desc.ObjectId) + RawSize(desc.MethodId) + RawSizeContainer(desc.InputArguments);
    }

    template<>
    void DataSerializer::Serialize<CallMethodRequest>(const CallMethodRequest& desc)
    {
      *this << desc.ObjectId;
      *this << desc.MethodId;
      SerializeContainer(*this, desc.InputArguments);
    }

    template<>
    void DataDeserializer::Deserialize<CallMethodRequest>(CallMethodRequest& desc)
    {
      *this >> desc.ObjectId;
      *this >> desc.MethodId;
      DeserializeContainer(*this, desc.InputArguments);
    }

    //---------------------------------------------------
    // CallRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CallRequest>(const CallRequest& request)
    {
      return RawSize(request.TypeId) + RawSize(request.Header) + RawSizeContainer(request.MethodsToCall);
    }

    template<>
    void DataSerializer::Serialize<CallRequest>(const CallRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
      SerializeContainer(*this, request.MethodsToCall);
    }

    template<>
    void DataDeserializer::Deserialize<CallRequest>(CallRequest& request)
    {
      *this >> request.TypeId;
      *this >> request.Header;
      DeserializeContainer(*this, request.MethodsToCall);
    }

    //---------------------------------------------------
    // CallMethodResult
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CallMethodResult>(const CallMethodResult& result)
    {
      return RawSize(result.Status) + RawSizeContainer(result.InputArgumentResults) +
          RawSizeContainer(result.InputArgumentDiagnosticInfos) +
          RawSizeContainer(result.OutputArguments);
    }

    template<>
    void DataSerializer::Serialize<CallMethodResult>(const CallMethodResult& result)
    {
      *this << result.Status;
      SerializeContainer(*this, result.InputArgumentResults);
      SerializeContainer(*this, result.InputArgumentDiagnosticInfos);
      SerializeContainer(*this, result.OutputArguments);
    }

    template<>
    void DataDeserializer::Deserialize<CallMethodResult>(CallMethodResult& result)
    {
      *this >> result.Status;
      DeserializeContainer(*this, result.InputArgumentResults);
      DeserializeContainer(*this, result.InputArgumentDiagnosticInfos);
      DeserializeContainer(*this, result.OutputArguments);
    }

    //---------------------------------------------------
    // CallResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CallResponse>(const CallResponse& response)
    {
      return RawSize(response.TypeId) + RawSize(response.Header) +
          RawSizeContainer(response.Results) + RawSizeContainer(response.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<CallResponse>(const CallResponse& response)
    {
       *this << response.TypeId;
       *this << response.Header;
       SerializeContainer(*this, response.Results, 0);
       SerializeContainer(*this, response.Diagnostics, 0);
    }

    template<>
    void DataDeserializer::Deserialize<CallResponse>(CallResponse& response)
    {
      *this >> response.TypeId;
      *this >> response.Header;
      DeserializeContainer(*this, response.Results);
      DeserializeContainer(*this, response.Diagnostics);
    }

  } // namespace Binary
} // namespace OpcUa
