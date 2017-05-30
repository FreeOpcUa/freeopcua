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
#include <opc/ua/protocol/protocol.h>
#include <opc/ua/protocol/binary/stream.h>

#include <iostream> //for debug
#include <opc/ua/protocol/string_utils.h> //for debug

namespace OpcUa
{

MonitoringFilter::MonitoringFilter(DataChangeFilter filter) : DataChange(filter)
{
  Header.TypeId  = ExpandedObjectId::DataChangeFilter;
  Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
}

MonitoringFilter::MonitoringFilter(EventFilter filter) : Event(filter)
{
  Header.TypeId  = ExpandedObjectId::EventFilter;
  Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
}

MonitoringFilter:: MonitoringFilter(AggregateFilter filter) : Aggregate(filter)
{
  Header.TypeId  = ExpandedObjectId::DataChangeFilter;
  Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
}

namespace Binary
{

////////////////////////////////////////////////////////
// AttributeOperand
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<AttributeOperand>(const OpcUa::AttributeOperand & params)
{
  return RawSize(params.Node) +
         RawSize(params.Alias) +
         RawSize(params.Path) +
         RawSize(params.AttributeId) +
         RawSizeContainer(params.IndexRange);
}

template<>
void DataDeserializer::Deserialize<AttributeOperand>(AttributeOperand & params)
{
  *this >> params.Node;
  *this >> params.Alias;
  *this >> params.Path;
  *this >> params.AttributeId;
  *this >> params.IndexRange;
}

template<>
void DataSerializer::Serialize<AttributeOperand>(const AttributeOperand & params)
{
  *this << params.Node;
  *this << params.Alias;
  *this << params.Path;
  *this << params.AttributeId;
  *this << params.IndexRange;
}

////////////////////////////////////////////////////////
// SimpleAttributeOperand
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<SimpleAttributeOperand>(const OpcUa::SimpleAttributeOperand & params)
{
  return RawSize(params.TypeId) +
         RawSizeContainer(params.BrowsePath) +
         RawSize(params.Attribute) +
         RawSizeContainer(params.IndexRange);
}

template<>
void DataDeserializer::Deserialize<SimpleAttributeOperand>(SimpleAttributeOperand & params)
{
  *this >> params.TypeId;
  *this >> params.BrowsePath;
  *this >> params.Attribute;
  *this >> params.IndexRange;
}

template<>
void DataSerializer::Serialize<SimpleAttributeOperand>(const SimpleAttributeOperand & params)
{
  *this << params.TypeId;
  *this << params.BrowsePath;
  *this << params.Attribute;
  *this << params.IndexRange;
}

template<>
void DataSerializer::Serialize<std::vector<SimpleAttributeOperand>>(const std::vector<SimpleAttributeOperand> & targets)
{
  SerializeContainer(*this, targets);
}

template<>
void DataDeserializer::Deserialize<std::vector<SimpleAttributeOperand>>(std::vector<SimpleAttributeOperand> & targets)
{
  DeserializeContainer(*this, targets);
}


////////////////////////////////////////////////////////
// LiteralOperand
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<LiteralOperand>(const OpcUa::LiteralOperand & params)
{
  return RawSize(params.Value);
}

template<>
void DataDeserializer::Deserialize<LiteralOperand>(LiteralOperand & params)
{
  *this >> params.Value;
}

template<>
void DataSerializer::Serialize<LiteralOperand>(const LiteralOperand & params)
{
  *this << params.Value;
}



////////////////////////////////////////////////////////
// ElementOperand
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<ElementOperand>(const OpcUa::ElementOperand & params)
{
  return RawSize(params.Index);
}

template<>
void DataDeserializer::Deserialize<ElementOperand>(ElementOperand & params)
{
  *this >> params.Index;
}

template<>
void DataSerializer::Serialize<ElementOperand>(const ElementOperand & params)
{
  *this << params.Index;
}


////////////////////////////////////////////////////////
// FilterOperand
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<FilterOperand>(const OpcUa::FilterOperand & params)
{
  size_t total = RawSize(params.Header);

  if (params.Header.TypeId == ExpandedObjectId::ElementOperand)
    {
      total += 4;
      total += RawSize(params.Element);
    }

  else if (params.Header.TypeId == ExpandedObjectId::LiteralOperand)
    {
      total += 4;
      total += RawSize(params.Literal);
    }

  else if (params.Header.TypeId == ExpandedObjectId::AttributeOperand)
    {
      total += 4;
      total += RawSize(params.Attribute);
    }

  else if (params.Header.TypeId == ExpandedObjectId::SimpleAttributeOperand)
    {
      total += 4;
      total += RawSize(params.SimpleAttribute);
    }

  return total;
}

template<>
void DataDeserializer::Deserialize<FilterOperand>(FilterOperand & params)
{
  *this >> params.Header;
  uint32_t size;
  *this >> size;

  if (params.Header.TypeId == ExpandedObjectId::ElementOperand)
    {
      *this >> params.Element;
    }

  else if (params.Header.TypeId == ExpandedObjectId::LiteralOperand)
    {
      *this >> params.Literal;
    }

  else if (params.Header.TypeId == ExpandedObjectId::AttributeOperand)
    {
      *this >> params.Attribute;
    }

  else if (params.Header.TypeId == ExpandedObjectId::SimpleAttributeOperand)
    {
      *this >> params.SimpleAttribute;
    }
}

template<>
void DataSerializer::Serialize<FilterOperand>(const FilterOperand & params)
{
  *this << params.Header;

  if (params.Header.TypeId == ExpandedObjectId::ElementOperand)
    {
      uint32_t size = RawSize(params.Element);
      *this << size;
      *this << params.Element;
    }

  else if (params.Header.TypeId == ExpandedObjectId::LiteralOperand)
    {
      uint32_t size = RawSize(params.Literal);
      *this << size;
      *this << params.Literal;
    }

  else if (params.Header.TypeId == ExpandedObjectId::AttributeOperand)
    {
      uint32_t size = RawSize(params.Attribute);
      *this << size;
      *this << params.Attribute;
    }

  else if (params.Header.TypeId == ExpandedObjectId::SimpleAttributeOperand)
    {
      uint32_t size = RawSize(params.SimpleAttribute);
      *this << size;
      *this << params.SimpleAttribute;
    }
}

template<>
void DataSerializer::Serialize<std::vector<FilterOperand>>(const std::vector<FilterOperand> & targets)
{
  SerializeContainer(*this, targets);
}

template<>
void DataDeserializer::Deserialize<std::vector<FilterOperand>>(std::vector<FilterOperand> & targets)
{
  DeserializeContainer(*this, targets);
}



////////////////////////////////////////////////////////
// ContentFilterElement
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<ContentFilterElement>(const OpcUa::ContentFilterElement & params)
{
  return RawSize(params.Operator) +
         RawSizeContainer(params.FilterOperands);
}

template<>
void DataDeserializer::Deserialize<ContentFilterElement>(ContentFilterElement & params)
{
  *this >> params.Operator;
  //*this >> params.FilterOperands;
  DeserializeContainer(*this, params.FilterOperands);
}

template<>
void DataSerializer::Serialize<ContentFilterElement>(const ContentFilterElement & params)
{
  *this << params.Operator;
  //*this << params.FilterOperands;
  SerializeContainer(*this, params.FilterOperands);
}




////////////////////////////////////////////////////////
// AggregateFilter
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<AggregateFilter>(const OpcUa::AggregateFilter & params)
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
void DataDeserializer::Deserialize<AggregateFilter>(AggregateFilter & params)
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
void DataSerializer::Serialize<AggregateFilter>(const AggregateFilter & params)
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
std::size_t RawSize<EventFilter>(const OpcUa::EventFilter & params)
{
  return RawSizeContainer(params.SelectClauses) +
         RawSizeContainer(params.WhereClause);
}

template<>
void DataDeserializer::Deserialize<EventFilter>(EventFilter & params)
{
  *this >> params.SelectClauses;
  DeserializeContainer(*this, params.WhereClause);
}

template<>
void DataSerializer::Serialize<EventFilter>(const EventFilter & params)
{
  *this << params.SelectClauses;
  SerializeContainer(*this, params.WhereClause);
}


////////////////////////////////////////////////////////
// DataChangeFilter
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<DataChangeFilter>(const OpcUa::DataChangeFilter & params)
{
  return RawSize(params.Trigger) +
         RawSize(params.Deadband) +
         RawSize(params.DeadbandValue);
}

template<>
void DataDeserializer::Deserialize<DataChangeFilter>(DataChangeFilter & params)
{
  *this >> params.Trigger;
  *this >> params.Deadband;
  *this >> params.DeadbandValue;
}

template<>
void DataSerializer::Serialize<DataChangeFilter>(const DataChangeFilter & params)
{
  *this << params.Trigger;
  *this << params.Deadband;
  *this << params.DeadbandValue;
}


////////////////////////////////////////////////////////
// MonitoringFilter
////////////////////////////////////////////////////////

template<>
std::size_t RawSize<MonitoringFilter>(const MonitoringFilter & data)
{
  size_t total = 0;
  total += RawSize(data.Header);

  if (data.Header.TypeId == ExpandedObjectId::DataChangeFilter)
    {
      total += 4;
      total += RawSize(data.DataChange);
    }

  else if (data.Header.TypeId == ExpandedObjectId::EventFilter)
    {
      total += 4;
      total += RawSize(data.Event);
    }

  else if (data.Header.TypeId == ExpandedObjectId::AggregateFilter)
    {
      total += 4;
      total += RawSize(data.Aggregate);
    }

  else if (data.Header.TypeId == NodeId(0, 0))
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
void DataDeserializer::Deserialize<MonitoringFilter>(MonitoringFilter & data)
{
  *this >> data.Header;
  int32_t size;

  if (data.Header.TypeId == ExpandedObjectId::DataChangeFilter)
    {
      *this >> size; //not used yet
      *this >> data.DataChange;
    }

  else if (data.Header.TypeId == ExpandedObjectId::EventFilter)
    {
      *this >> size; //not used yet
      *this >> data.Event;
    }

  else if (data.Header.TypeId == ExpandedObjectId::AggregateFilter)
    {
      *this >> size; //not used yet
      *this >> data.Aggregate;
    }

  else if (data.Header.TypeId == NodeId(0, 0))
    {
      //No filter is used
    }
  else
    {
      throw std::runtime_error("Filter data type not supported in deserialization");
    }
}

template<>
void DataSerializer::Serialize<MonitoringFilter>(const MonitoringFilter & data)
{
  *this << data.Header;

  if (data.Header.TypeId == ExpandedObjectId::DataChangeFilter)
    {
      *this << (uint32_t) RawSize(data.DataChange);
      *this << data.DataChange;
    }

  else if (data.Header.TypeId == ExpandedObjectId::EventFilter)
    {
      *this << (uint32_t) RawSize(data.Event);
      *this << data.Event;
    }

  else if (data.Header.TypeId == ExpandedObjectId::AggregateFilter)
    {
      *this << (uint32_t) RawSize(data.Aggregate);
      *this << data.Aggregate;
    }

  else if (data.Header.TypeId == NodeId(0, 0))
    {
      //No filter is used
    }
  else
    {
      throw std::runtime_error("Filter data type not supported in serialization");
    }
}

////////////////////////////////////////////////////////
// MonitoredItemCreateRequest
////////////////////////////////////////////////////////

template<>
void DataSerializer::Serialize<std::vector<MonitoredItemCreateRequest>>(const std::vector<MonitoredItemCreateRequest> & targets)
{
  SerializeContainer(*this, targets);
}

template<>
void DataDeserializer::Deserialize<std::vector<MonitoredItemCreateRequest>>(std::vector<MonitoredItemCreateRequest> & targets)
{
  DeserializeContainer(*this, targets);
}

////////////////////////////////////////////////////////
// CreateMonitoredItemsRequest
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

template<>
void DataSerializer::Serialize<std::vector<MonitoredItemCreateResult>>(const std::vector<MonitoredItemCreateResult> & targets)
{
  SerializeContainer(*this, targets);
}

template<>
void DataDeserializer::Deserialize<std::vector<MonitoredItemCreateResult>>(std::vector<MonitoredItemCreateResult> & targets)
{
  DeserializeContainer(*this, targets);
}





}
}
