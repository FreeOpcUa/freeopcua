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

#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/protocol.h>

namespace OpcUa
{

  struct DataChangeFilter
  {
    DataChangeTrigger Trigger;
    DeadbandType Deadband;
    double DeadbandValue;
  };

  struct SimpleAttributeOperand
  {
    NodeId TypeId;
    std::vector<QualifiedName> BrowsePath;
    AttributeId Attribute;
    std::vector<std::string> IndexRange; 
  };

  struct ElementOperand
  {
    uint32_t Index;
  };

  struct LiteralOperand
  {
    // BaseDataType Value; // TODO
    Variant Value;
  };

  struct AttributeOperand
  {
    NodeId Node;
    std::string Alias;
    RelativePath Path;
    IntegerId AttributeId;
    std::vector<std::string> IndexRange;
  };

  struct FilterOperand
  {
    ExtensionObjectHeader Header;
    ElementOperand Element;
    LiteralOperand Literal;
    AttributeOperand Attribute;
    SimpleAttributeOperand SimpleAttribute;
  };

  struct ContentFilterElement
  {
    FilterOperator Operator;
    std::vector<FilterOperand> FilterOperands;
  };

  struct EventFilter
  {
    std::vector<SimpleAttributeOperand> SelectClauses;
    std::vector<ContentFilterElement> WhereClause;
  };

  struct AggregateFilter
  {
    DateTime StartTime;
    NodeId AggregateType;
    Duration ProcessingInterval;
    //AggregateConfiguration Configuration; //aggregate conf is in fact the following parameters
    bool UseServerCapabilitiesDefaults;
    bool TreatUncertainAsBad;
    uint8_t PercentDataBad;
    uint8_t PercentDataGood;
    bool SteppedSlopedExtrapolation;
  };

  struct MonitoringFilter
  {
    ExtensionObjectHeader Header;
    DataChangeFilter DataChange;
    EventFilter Event;
    AggregateFilter Aggregate;
    MonitoringFilter() {}
    MonitoringFilter(DataChangeFilter filter);
    MonitoringFilter(EventFilter filter);
    MonitoringFilter(AggregateFilter filter);
  };

  struct MonitoringParameters
  {
    IntegerId ClientHandle;
    Duration SamplingInterval;
    MonitoringFilter Filter;
    uint32_t QueueSize;
    bool DiscardOldest;
  };

  struct MonitoredItemRequest
  {
    ReadValueId ItemToMonitor;
    MonitoringMode Mode;
    MonitoringParameters Parameters;
  };

  struct MonitoredItemsParameters
  {
    IntegerId SubscriptionId;
    TimestampsToReturn Timestamps;
    std::vector<MonitoredItemRequest> ItemsToCreate;

    MonitoredItemsParameters();
  };

  struct CreateMonitoredItemsRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    MonitoredItemsParameters Parameters;

    CreateMonitoredItemsRequest();
  };


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
