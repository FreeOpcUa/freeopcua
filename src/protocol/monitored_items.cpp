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

  CreateMonitoredItemsResult::CreateMonitoredItemsResult()
    : Status(StatusCode::BadNotImplemented)
    , MonitoredItemID(0)
    , RevisedSamplingInterval(0)
    , RevizedQueueSize(0)
  {
  }

  DeleteMonitoredItemsRequest::DeleteMonitoredItemsRequest()
    : TypeID(MessageID::DELETE_MONITORED_ITEMS_REQUEST)
  {
  }

  DeleteMonitoredItemsResponse::DeleteMonitoredItemsResponse()
    : TypeID(MessageID::DELETE_MONITORED_ITEMS_RESPONSE)
  {
  }

  MonitoredItemsData::MonitoredItemsData()
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

    template <>
    std::size_t RawSize<DeleteMonitoredItemsParameters>(const DeleteMonitoredItemsParameters& data)
    {
      return RawSize(data.SubscriptionId) + RawSizeContainer(data.MonitoredItemsIds);
    }

    template<>
    void DataSerializer::Serialize<DeleteMonitoredItemsParameters>(const DeleteMonitoredItemsParameters& data)
    {
      *this << data.SubscriptionId;
      *this << data.MonitoredItemsIds;
    }

    template<>
    void DataDeserializer::Deserialize<DeleteMonitoredItemsParameters>(DeleteMonitoredItemsParameters& data)
    {
      *this >> data.SubscriptionId;
      *this >> data.MonitoredItemsIds;
    }

    template <>
    std::size_t RawSize<DeleteMonitoredItemsRequest>(const DeleteMonitoredItemsRequest& data)
    {
      return RawSize(data.TypeID) + RawSize(data.Header) + RawSize(data.Parameters);
    }

    template<>
    void DataSerializer::Serialize<DeleteMonitoredItemsRequest>(const DeleteMonitoredItemsRequest& data)
    {
      *this << data.TypeID;
      *this << data.Header;
      *this << data.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<DeleteMonitoredItemsRequest>(DeleteMonitoredItemsRequest& data)
    {
      *this >> data.TypeID;
      *this >> data.Header;
      *this >> data.Parameters;
    }


    template <>
    std::size_t RawSize<DeleteMonitoredItemsResponse>(const DeleteMonitoredItemsResponse& data)
    {
      return RawSize(data.TypeID) + RawSize(data.Header) + RawSizeContainer(data.Results);
    }

    template<>
    void DataSerializer::Serialize<DeleteMonitoredItemsResponse>(const DeleteMonitoredItemsResponse& data)
    {
      *this << data.TypeID;
      *this << data.Header;
      *this << data.Results;
    }

    template<>
    void DataDeserializer::Deserialize<DeleteMonitoredItemsResponse>(DeleteMonitoredItemsResponse& data)
    {
      *this >> data.TypeID;
      *this >> data.Header;
      *this >> data.Results;
    }

  }
}
