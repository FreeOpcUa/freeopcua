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

#include <opc/ua/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>

namespace OpcUa
{

  CreateMonitoredItemsResponse::CreateMonitoredItemsResponse()
    : TypeID(MessageID::CREATE_MONITORED_ITEMS_RESPONSE)
  {
  }

  MonitoredItemsData::MonitoredItemsData()
    : Results(1)
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

  }
}
