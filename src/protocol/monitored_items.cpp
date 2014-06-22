/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Monitored items services.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>

namespace OpcUa
{

  CreateMonitoredItemsRequest::CreateMonitoredItemsRequest()
    : TypeID(MessageID::CREATE_MONITORED_ITEMS_REQUEST)
  {
  }


  CreateMonitoredItemsResponse::CreateMonitoredItemsResponse()
    : TypeID(MessageID::CREATE_MONITORED_ITEMS_RESPONSE)
  {
  }

  MonitoredItemsData::MonitoredItemsData()
  {
  }


  CreateMonitoredItemsResult::CreateMonitoredItemsResult()
    : Status(StatusCode::BadNotImplemented)
    , MonitoredItemID(0)
    , RevisedSamplingInterval(0)
    , RevizedQueueSize(0)
  {
  }

  namespace Binary
  {
    ////////////////////////////////////////////////////////////////

    template <>
    std::size_t RawSize<CreateMonitoredItemsResult>(const CreateMonitoredItemsResult& result)
    {
      return RawSize(result.Status) +
        RawSize(result.MonitoredItemID) +
        RawSize(result.RevisedSamplingInterval) +
        RawSize(result.RevizedQueueSize) +
        RawSize(result.FilterResult);
    }

    template<>
    void DataSerializer::Serialize<CreateMonitoredItemsResult>(const CreateMonitoredItemsResult& result)
    {
      *this << result.Status;
      *this << result.MonitoredItemID;
      *this << result.RevisedSamplingInterval;
      *this << result.RevizedQueueSize;
      *this << result.FilterResult;
    }

   template<>
    void DataDeserializer::Deserialize<CreateMonitoredItemsResult>(CreateMonitoredItemsResult& params)
    {
      *this >> params.Status;
      *this >> params.MonitoredItemID;
      *this >> params.RevisedSamplingInterval;
      *this >> params.RevizedQueueSize;
      *this >> params.FilterResult;
    }

    template<>
    void DataSerializer::Serialize<std::vector<CreateMonitoredItemsResult>>(const std::vector<CreateMonitoredItemsResult>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<CreateMonitoredItemsResult>>(std::vector<CreateMonitoredItemsResult>& targets)
    {
      DeserializeContainer(*this, targets);
    }



    ////////////////////////////////////////////////////////////////

    template <>
    std::size_t RawSize<MonitoredItemsData>(const MonitoredItemsData& data)
    {
      return RawSizeContainer(data.Results) + RawSizeContainer(data.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<MonitoredItemsData>(const MonitoredItemsData& data)
    {
      SerializeContainer(*this, data.Results, 0);
      SerializeContainer(*this, data.Diagnostics, 0);
    }

    template<>
    void DataDeserializer::Deserialize<MonitoredItemsData>(MonitoredItemsData& params)
    {
      *this >> params.Results;
      *this >> params.Diagnostics;
    }
    
    template<>
    void DataSerializer::Serialize<std::vector<MonitoredItemsData>>(const std::vector<MonitoredItemsData>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<MonitoredItemsData>>(std::vector<MonitoredItemsData>& targets)
    {
      DeserializeContainer(*this, targets);
    }





    ////////////////////////////////////////////////////////////////

    template <>
    std::size_t RawSize<CreateMonitoredItemsResponse>(const CreateMonitoredItemsResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) + RawSize(response.Data);
    }

    template<>
    void DataSerializer::Serialize<CreateMonitoredItemsResponse>(const CreateMonitoredItemsResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Data;
    }

    template<>
    void DataDeserializer::Deserialize<CreateMonitoredItemsResponse>(CreateMonitoredItemsResponse& params)
    {
      *this >> params.TypeID;
      *this >> params.Header;
      *this >> params.Data;
    }



  }
}
