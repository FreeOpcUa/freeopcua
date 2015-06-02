/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Secure channel service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_PROTOCOL_SUBSCRIPTIONS_H
#define OPC_UA_PROTOCOL_SUBSCRIPTIONS_H

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/data_value.h>

namespace OpcUa
{
  ////////////////////////////////////////////////////////
  // DeleteSubscriptionRequest
  ////////////////////////////////////////////////////////

  struct DeleteSubscriptionRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    std::vector<uint32_t> SubscriptionsIds;

    DeleteSubscriptionRequest();
  };

  struct DeleteSubscriptionResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    std::vector<StatusCode> Results;
    DiagnosticInfoList Diagnostic;

    DeleteSubscriptionResponse();
  };


  ////////////////////////////////////////////////////////
  // SetPublishingMode
  ////////////////////////////////////////////////////////

  struct PublishingModeParameters
  {
    bool Enabled;
    std::vector<IntegerId> SubscriptionIds;

    PublishingModeParameters();
  };

  struct SetPublishingModeRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    PublishingModeParameters Parameters;

    SetPublishingModeRequest();
  };

  struct PublishingModeResult
  {
    std::vector<StatusCode> Statuses;
    DiagnosticInfoList Diagnostics;
  };

  struct SetPublishingModeResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    PublishingModeResult Result;

    SetPublishingModeResponse();
  };

} // namespace OpcUa

#endif /// OPC_UA_PROTOCOL_SUBSCRIPTIONS_H
