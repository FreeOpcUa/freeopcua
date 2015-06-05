/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Monitored items services.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_MAPPINGS_MONITORED_ITEMS_H_
#define OPC_UA_MAPPINGS_MONITORED_ITEMS_H_

#include <opc/ua/protocol/protocol.h>

namespace OpcUa
{

  struct DeleteMonitoredItemsParameters
  {
    IntegerId SubscriptionId;
    std::vector<IntegerId> MonitoredItemsIds;
  };

  struct DeleteMonitoredItemsRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    DeleteMonitoredItemsParameters Parameters;

    DeleteMonitoredItemsRequest();
  };

  struct DeleteMonitoredItemsResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    std::vector<StatusCode> Results;
    DiagnosticInfoList Diagnostics;

    DeleteMonitoredItemsResponse();
  };


  ///////////////////////////////////////////////////////////////////////
  struct CreateMonitoredItemsResult
  {
    StatusCode Status;
    IntegerId MonitoredItemId;
    Duration RevisedSamplingInterval;
    uint32_t RevizedQueueSize;
    MonitoringFilter Filter;

    CreateMonitoredItemsResult();
  };

  struct MonitoredItemsData
  {
    std::vector<CreateMonitoredItemsResult> Results;
    DiagnosticInfoList Diagnostics;

    MonitoredItemsData();
  };

  struct CreateMonitoredItemsResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    MonitoredItemsData Data;

    CreateMonitoredItemsResponse();
  };

}

#endif // OPC_UA_MAPPINGS_MONITORED_ITEMS_H_
