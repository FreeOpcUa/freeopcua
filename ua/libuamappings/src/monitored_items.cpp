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

  namespace Binary
  {
    ////////////////////////////////////////////////////////////////

    template <>
    std::size_t RawSize<MonitoredItemsData>(const MonitoredItemsData& data)
    {
      return RawSizeContainer(data.Results) + RawSizeContainer(data.Infos);
    }

    template<>
    void OStream::Serialize<MonitoredItemsData>(const MonitoredItemsData& data)
    {
      SerializeContainer(*this, data.Results);
      SerializeContainer(*this, data.Infos);
    }

    ////////////////////////////////////////////////////////////////

    template <>
    std::size_t RawSize<CreateMonitoredItemsResponse>(const CreateMonitoredItemsResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) + RawSize(response.Data);
    }

    template<>
    void OStream::Serialize<CreateMonitoredItemsResponse>(const CreateMonitoredItemsResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Data;
    }

  }
}
