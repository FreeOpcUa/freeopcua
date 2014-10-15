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

#include <iostream> //for debug
#include <opc/ua/protocol/string_utils.h> //for debug

namespace OpcUa
{

  MonitoredItemsParameters::MonitoredItemsParameters()
    : Timestamps(TimestampsToReturn::NEITHER)
  {
  }

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
    , MonitoredItemID(1) // 0 is not allowed for integerID
    , RevisedSamplingInterval(0)
    , RevizedQueueSize(0)
    , Filter()
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

  MonitoringFilter::MonitoringFilter(DataChangeFilter filter) : DataChange(filter)
  {
    Header.TypeID  = ExpandedObjectID::DataChangeFilter;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
  }

  MonitoringFilter::MonitoringFilter(EventFilter filter) : Event(filter)
  {
    Header.TypeID  = ExpandedObjectID::EventFilter;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
  }

  MonitoringFilter:: MonitoringFilter(AggregateFilter filter) : Aggregate(filter)
  {
    Header.TypeID  = ExpandedObjectID::DataChangeFilter;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
  }

  namespace Binary
  {

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
      return RawSize(data.TypeID) + RawSize(data.Header) + RawSizeContainer(data.Results) + RawSize(data.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<DeleteMonitoredItemsResponse>(const DeleteMonitoredItemsResponse& data)
    {
      *this << data.TypeID;
      *this << data.Header;
      *this << data.Results;
      *this << data.Diagnostics;
    }



    template<>
    void DataDeserializer::Deserialize<DeleteMonitoredItemsResponse>(DeleteMonitoredItemsResponse& data)
    {
      *this >> data.TypeID;
      *this >> data.Header;
      *this >> data.Results;
      *this >> data.Diagnostics;
    }

    ////////////////////////////////////////////////////////////////
    // FilterOperator
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<FilterOperator>(const FilterOperator&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<FilterOperator>(const FilterOperator& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<FilterOperator>(FilterOperator& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<FilterOperator>(tmp);
    }

    ////////////////////////////////////////////////////////////////
    // DeadbandType
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<DeadbandType>(const DeadbandType&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<DeadbandType>(const DeadbandType& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<DeadbandType>(DeadbandType& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<DeadbandType>(tmp);
    }


    ////////////////////////////////////////////////////////////////
    // DataChangeTrigger
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<DataChangeTrigger>(const DataChangeTrigger&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<DataChangeTrigger>(const DataChangeTrigger& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<DataChangeTrigger>(DataChangeTrigger& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<DataChangeTrigger>(tmp);
    }

    ////////////////////////////////////////////////////////////////
    // MonitoringMode
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoringMode>(const MonitoringMode&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<MonitoringMode>(const MonitoringMode& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<MonitoringMode>(MonitoringMode& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<MonitoringMode>(tmp);
    }
    ////////////////////////////////////////////////////////
    // AttributeOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<AttributeOperand>(const OpcUa::AttributeOperand& params)
    {
      return RawSize(params.Node) +
          RawSize(params.Alias) +
          RawSize(params.Path) +
          RawSize(params.AttributeID) +
          RawSizeContainer(params.IndexRange); 
    }

    template<>
    void DataDeserializer::Deserialize<AttributeOperand>(AttributeOperand& params)
    {
      *this >> params.Node;
      *this >> params.Alias;
      *this >> params.Path;
      *this >> params.AttributeID;
      *this >> params.IndexRange;
    }

    template<>
    void DataSerializer::Serialize<AttributeOperand>(const AttributeOperand& params)
    {
      *this << params.Node;
      *this << params.Alias;
      *this << params.Path;
      *this << params.AttributeID;
      *this << params.IndexRange;
    }

    ////////////////////////////////////////////////////////
    // SimpleAttributeOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<SimpleAttributeOperand>(const OpcUa::SimpleAttributeOperand& params)
    {
      return RawSize(params.TypeID) +
          RawSizeContainer(params.BrowsePath) +
          RawSize(params.Attribute) +
          RawSizeContainer(params.IndexRange); 
    }

    template<>
    void DataDeserializer::Deserialize<SimpleAttributeOperand>(SimpleAttributeOperand& params)
    {
      *this >> params.TypeID;
      *this >> params.BrowsePath;
      *this >> params.Attribute;
      *this >> params.IndexRange;
    }

    template<>
    void DataSerializer::Serialize<SimpleAttributeOperand>(const SimpleAttributeOperand& params)
    {
      *this << params.TypeID;
      *this << params.BrowsePath;
      *this << params.Attribute;
      *this << params.IndexRange;
    }

    template<>
    void DataSerializer::Serialize<std::vector<SimpleAttributeOperand>>(const std::vector<SimpleAttributeOperand>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<SimpleAttributeOperand>>(std::vector<SimpleAttributeOperand>& targets)
    {
      DeserializeContainer(*this, targets);
    }


    ////////////////////////////////////////////////////////
    // LiteralOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<LiteralOperand>(const OpcUa::LiteralOperand& params)
    {
      return RawSize(params.Value);
    }

    template<>
    void DataDeserializer::Deserialize<LiteralOperand>(LiteralOperand& params)
    {
      *this >> params.Value;
    }

    template<>
    void DataSerializer::Serialize<LiteralOperand>(const LiteralOperand& params)
    {
      *this << params.Value;
    }



    ////////////////////////////////////////////////////////
    // ElementOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<ElementOperand>(const OpcUa::ElementOperand& params)
    {
      return RawSize(params.Index);
    }

    template<>
    void DataDeserializer::Deserialize<ElementOperand>(ElementOperand& params)
    {
      *this >> params.Index;
    }

    template<>
    void DataSerializer::Serialize<ElementOperand>(const ElementOperand& params)
    {
      *this << params.Index;
    }


    ////////////////////////////////////////////////////////
    // FilterOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<FilterOperand>(const OpcUa::FilterOperand& params)
    {
      size_t total = RawSize(params.Header);
      if (params.Header.TypeID == ExpandedObjectID::ElementOperand )
      {
        total += 4;
        total += RawSize(params.Element);
      }
      else if (params.Header.TypeID == ExpandedObjectID::LiteralOperand )
      {
        total += 4;
        total += RawSize(params.Literal);
      }
      else if (params.Header.TypeID == ExpandedObjectID::AttributeOperand )
      {
        total += 4;
        total += RawSize(params.Attribute);
      }
      else if (params.Header.TypeID == ExpandedObjectID::SimpleAttributeOperand )
      {
        total += 4;
        total += RawSize(params.SimpleAttribute);
      }

      return total;
    }

    template<>
    void DataDeserializer::Deserialize<FilterOperand>(FilterOperand& params)
    {
      *this >> params.Header;
      uint32_t size;
      *this >> size;
      if ( params.Header.TypeID == ExpandedObjectID::ElementOperand )
      {
        *this >> params.Element;
      }
      else if ( params.Header.TypeID == ExpandedObjectID::LiteralOperand )
      {
        *this >> params.Literal;
      }
      else if ( params.Header.TypeID == ExpandedObjectID::AttributeOperand )
      {
        *this >> params.Attribute;
      }
      else if ( params.Header.TypeID == ExpandedObjectID::SimpleAttributeOperand )
      {
        *this >> params.SimpleAttribute;
      }
    }

    template<>
    void DataSerializer::Serialize<FilterOperand>(const FilterOperand& params)
    {
      *this << params.Header;
      if ( params.Header.TypeID == ExpandedObjectID::ElementOperand )
      {
        uint32_t size = RawSize(params.Element);
        *this << size;
        *this << params.Element;
      }
      else if ( params.Header.TypeID == ExpandedObjectID::LiteralOperand )
      {
        uint32_t size = RawSize(params.Literal);
        *this << size;
        *this << params.Literal;
      }
      else if ( params.Header.TypeID == ExpandedObjectID::AttributeOperand )
      {
        uint32_t size = RawSize(params.Attribute);
        *this << size;
        *this << params.Attribute;
      }
      else if ( params.Header.TypeID == ExpandedObjectID::SimpleAttributeOperand )
      {
        uint32_t size = RawSize(params.SimpleAttribute);
        *this << size;
        *this << params.SimpleAttribute;
      }
    }
    
    template<>
    void DataSerializer::Serialize<std::vector<FilterOperand>>(const std::vector<FilterOperand>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<FilterOperand>>(std::vector<FilterOperand>& targets)
    {
      DeserializeContainer(*this, targets);
    }



    ////////////////////////////////////////////////////////
    // ContentFilterElement
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<ContentFilterElement>(const OpcUa::ContentFilterElement& params)
    {
      return RawSize(params.Operator) +
          RawSizeContainer(params.FilterOperands); 
    }

    template<>
    void DataDeserializer::Deserialize<ContentFilterElement>(ContentFilterElement& params)
    {
      *this >> params.Operator;
      //*this >> params.FilterOperands;
      DeserializeContainer(*this, params.FilterOperands);
    }

    template<>
    void DataSerializer::Serialize<ContentFilterElement>(const ContentFilterElement& params)
    {
      *this << params.Operator;
      //*this << params.FilterOperands;
      SerializeContainer(*this, params.FilterOperands);
    }




    ////////////////////////////////////////////////////////
    // AggregateFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<AggregateFilter>(const OpcUa::AggregateFilter& params)
    {
      return RawSize(params.StartTime) +
          RawSize(params.AggregateType) +
          RawSize(params.ProcessingInterval) +
          RawSize(params.UseServerCapabilitiesDefaults) +
          RawSize(params.TreatUncertainAsBad) +
          RawSize(params.PercentDataBad) +
          RawSize(params.PercentDataGood) +
          RawSize(params.SteppedSlopedExtrapolation);
    }

    template<>
    void DataDeserializer::Deserialize<AggregateFilter>(AggregateFilter& params)
    {
      *this >> params.StartTime;
      *this >> params.AggregateType;
      *this >> params.ProcessingInterval;
      *this >> params.UseServerCapabilitiesDefaults;
      *this >> params.TreatUncertainAsBad;
      *this >> params.PercentDataBad;
      *this >> params.PercentDataGood;
      *this >> params.SteppedSlopedExtrapolation;
    }

    template<>
    void DataSerializer::Serialize<AggregateFilter>(const AggregateFilter& params)
    {
      *this << params.StartTime;
      *this << params.AggregateType;
      *this << params.ProcessingInterval;
      *this << params.UseServerCapabilitiesDefaults;
      *this << params.TreatUncertainAsBad;
      *this << params.PercentDataBad;
      *this << params.PercentDataGood;
      *this << params.SteppedSlopedExtrapolation;
    }



    ////////////////////////////////////////////////////////
    // EventFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<EventFilter>(const OpcUa::EventFilter& params)
    {
      return RawSizeContainer(params.SelectClauses) +
          RawSizeContainer(params.WhereClause);
    }

    template<>
    void DataDeserializer::Deserialize<EventFilter>(EventFilter& params)
    {
      *this >> params.SelectClauses;
      DeserializeContainer(*this, params.WhereClause);
    }

    template<>
    void DataSerializer::Serialize<EventFilter>(const EventFilter& params)
    {
      *this << params.SelectClauses;
      SerializeContainer(*this, params.WhereClause);
    }


    ////////////////////////////////////////////////////////
    // DataChangeFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<DataChangeFilter>(const OpcUa::DataChangeFilter& params)
    {
      return RawSize(params.Trigger) +
          RawSize(params.Deadband) +
          RawSize(params.DeadbandValue);
    }

    template<>
    void DataDeserializer::Deserialize<DataChangeFilter>(DataChangeFilter& params)
    {
      *this >> params.Trigger;
      *this >> params.Deadband;
      *this >> params.DeadbandValue;
    }

    template<>
    void DataSerializer::Serialize<DataChangeFilter>(const DataChangeFilter& params)
    {
      *this << params.Trigger;
      *this << params.Deadband;
      *this << params.DeadbandValue;
    }


    ////////////////////////////////////////////////////////
    // MonitoringFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoringFilter>(const MonitoringFilter& data)
    {
      size_t total = 0;
      total += RawSize(data.Header);
      if ( data.Header.TypeID == ExpandedObjectID::DataChangeFilter) 
      {
        total += 4;
        total += RawSize(data.DataChange);
      }
      else if ( data.Header.TypeID == ExpandedObjectID::EventFilter) 
      {
        total += 4;
        total += RawSize(data.Event);
      }
      else if ( data.Header.TypeID == ExpandedObjectID::AggregateFilter) 
      {
        total += 4;
        total += RawSize(data.Aggregate);
      }
      else if ( data.Header.TypeID == NodeID(0, 0) ) 
      {
        //No filter is used
      }
      else
      {
        throw std::runtime_error("MonitoringFilter type not implemented");
      }

      return total; 
    }
    

    template<>
    void DataDeserializer::Deserialize<MonitoringFilter>(MonitoringFilter& data)
    {
      *this >> data.Header;
      int32_t size;
      if ( data.Header.TypeID == ExpandedObjectID::DataChangeFilter ) 
      {
        *this >> size; //not used yet
        *this >> data.DataChange;
      }
      else if ( data.Header.TypeID == ExpandedObjectID::EventFilter ) 
      {
        *this >> size; //not used yet
        *this >> data.Event;
      }
      else if ( data.Header.TypeID == ExpandedObjectID::AggregateFilter ) 
      {
        *this >> size; //not used yet
        *this >> data.Aggregate;
      }
      else if ( data.Header.TypeID == NodeID(0, 0) ) 
      {
        //No filter is used
      }
      else
      {
        throw std::runtime_error("Filter data type not supported in deserialization");
      }
    }

    template<>
    void DataSerializer::Serialize<MonitoringFilter>(const MonitoringFilter& data)
    {
      *this << data.Header;
      if ( data.Header.TypeID == ExpandedObjectID::DataChangeFilter ) 
      {
        *this << (uint32_t) RawSize(data.DataChange);
        *this << data.DataChange;
      }
      else if ( data.Header.TypeID == ExpandedObjectID::EventFilter ) 
      {
        *this << (uint32_t) RawSize(data.Event);
        *this << data.Event;
      }
      else if ( data.Header.TypeID == ExpandedObjectID::AggregateFilter ) 
      {
        *this << (uint32_t) RawSize(data.Aggregate);
        *this << data.Aggregate;
      }
      else if ( data.Header.TypeID == NodeID(0, 0) ) 
      {
        //No filter is used
      }
      else
      {
        throw std::runtime_error("Filter data type not supported in serialization");
      }
    }

    ////////////////////////////////////////////////////////
    // MonitoringParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoringParameters>(const OpcUa::MonitoringParameters& params)
    {
      return RawSize(params.ClientHandle) +
          RawSize(params.SamplingInterval) +
          RawSize(params.Filter) +
          RawSize(params.QueueSize) +
          RawSize(params.DiscardOldest); 
    }

    template<>
    void DataDeserializer::Deserialize<MonitoringParameters>(MonitoringParameters& params)
    {
      *this >> params.ClientHandle;
      *this >> params.SamplingInterval;
      *this >> params.Filter;
      *this >> params.QueueSize;
      *this >> params.DiscardOldest;
    }

    template<>
    void DataSerializer::Serialize<MonitoringParameters>(const MonitoringParameters& params)
    {
      *this << params.ClientHandle;
      *this << params.SamplingInterval;
      *this << params.Filter;
      *this << params.QueueSize;
      *this << params.DiscardOldest;
    }

    ////////////////////////////////////////////////////////
    // MonitoredItemRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoredItemRequest>(const OpcUa::MonitoredItemRequest& params)
    {
      return RawSize(params.ItemToMonitor) +
          RawSize(params.Mode) +
          RawSize(params.Parameters); 
    }

    template<>
    void DataDeserializer::Deserialize<MonitoredItemRequest>(MonitoredItemRequest& params)
    {
      *this >> params.ItemToMonitor;
      *this >> params.Mode;
      *this >> params.Parameters;
    }

    template<>
    void DataSerializer::Serialize<MonitoredItemRequest>(const MonitoredItemRequest& params)
    {
      *this << params.ItemToMonitor;
      *this << params.Mode;
      *this << params.Parameters;
    }

    template<>
    void DataSerializer::Serialize<std::vector<MonitoredItemRequest>>(const std::vector<MonitoredItemRequest>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<MonitoredItemRequest>>(std::vector<MonitoredItemRequest>& targets)
    {
      DeserializeContainer(*this, targets);
    }


    ////////////////////////////////////////////////////////
    // MonitoredItemsParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoredItemsParameters>(const OpcUa::MonitoredItemsParameters& params)
    {
      return RawSize(params.SubscriptionID) +
          RawSize(params.Timestamps) +
          RawSizeContainer(params.ItemsToCreate); 
    }

    template<>
    void DataDeserializer::Deserialize<MonitoredItemsParameters>(MonitoredItemsParameters& params)
    {
      *this >> params.SubscriptionID;
      *this >> params.Timestamps;
      *this >> params.ItemsToCreate;
    }

    template<>
    void DataSerializer::Serialize<MonitoredItemsParameters>(const MonitoredItemsParameters& params)
    {
      *this << params.SubscriptionID;
      *this << params.Timestamps;
      *this << params.ItemsToCreate;
    }

    ////////////////////////////////////////////////////////
    // CreateMonitoredItemsRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<CreateMonitoredItemsRequest>(const OpcUa::CreateMonitoredItemsRequest& params)
    {
      return RawSize(params.TypeID) +
          RawSize(params.Header) +
          RawSize(params.Parameters); 
    }

    template<>
    void DataDeserializer::Deserialize<CreateMonitoredItemsRequest>(CreateMonitoredItemsRequest& params)
    {
      *this >> params.TypeID;
      *this >> params.Header;
      *this >> params.Parameters;
    }

    template<>
    void DataSerializer::Serialize<CreateMonitoredItemsRequest>(const CreateMonitoredItemsRequest& params)
    {
      *this << params.TypeID;
      *this << params.Header;
      *this << params.Parameters;
    }

    ////////////////////////////////////////////////////////////////

    template <>
    std::size_t RawSize<CreateMonitoredItemsResult>(const CreateMonitoredItemsResult& result)
    {
      return RawSize(result.Status) +
        RawSize(result.MonitoredItemID) +
        RawSize(result.RevisedSamplingInterval) +
        RawSize(result.RevizedQueueSize) +
        RawSize(result.Filter);
    }

    template<>
    void DataSerializer::Serialize<CreateMonitoredItemsResult>(const CreateMonitoredItemsResult& result)
    {
      *this << result.Status;
      *this << result.MonitoredItemID;
      *this << result.RevisedSamplingInterval;
      *this << result.RevizedQueueSize;
      *this << result.Filter;
    }

   template<>
    void DataDeserializer::Deserialize<CreateMonitoredItemsResult>(CreateMonitoredItemsResult& params)
    {
      *this >> params.Status;
      *this >> params.MonitoredItemID;
      *this >> params.RevisedSamplingInterval;
      *this >> params.RevizedQueueSize;
      *this >> params.Filter;
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





  }
}
