/// @author Olivier Roulet-Dubonnet 2014 
/// @email olivier.roulet@gmail.com
/// @brief Opc Ua Binary. Attributes service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/expanded_object_ids.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/node_management.h>
#include <opc/ua/protocol/string_utils.h>

#include <algorithm>
#include <memory>
#include <string>
#include <iostream>

namespace OpcUa
{


  NodeAttributes::NodeAttributes(const ObjectAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::ObjectAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::EventNotifier);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::EventNotifier] = attr.EventNotifier;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const VariableAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::VariableAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::Value);
    specattr.set(SpecifiedAttributesEncoding::DataType);
    specattr.set(SpecifiedAttributesEncoding::ValueRank);
    specattr.set(SpecifiedAttributesEncoding::ArrayDimensions);
    specattr.set(SpecifiedAttributesEncoding::AccessLevel);
    specattr.set(SpecifiedAttributesEncoding::UserAccessLevel);
    specattr.set(SpecifiedAttributesEncoding::MinimumSamplingInterval);
    specattr.set(SpecifiedAttributesEncoding::Historizing);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::Value] = attr.Value;
    Attributes[AttributeId::DataType] = attr.Type;
    Attributes[AttributeId::ValueRank] = attr.Rank;
    Attributes[AttributeId::ArrayDimensions] = attr.Dimensions;
    Attributes[AttributeId::AccessLevel] = static_cast<uint8_t>(attr.AccessLevel);
    Attributes[AttributeId::UserAccessLevel] = static_cast<uint8_t>(attr.UserAccessLevel);
    Attributes[AttributeId::MinimumSamplingInterval] = attr.MinimumSamplingInterval;
    Attributes[AttributeId::Historizing] = attr.Historizing;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const MethodAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::MethodAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::Executable);
    specattr.set(SpecifiedAttributesEncoding::UserExecutable);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::Executable] = attr.Executable;
    Attributes[AttributeId::UserExecutable] = attr.UserExecutable;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }
  NodeAttributes::NodeAttributes(const ObjectTypeAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::ObjectTypeAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::IsAbstract);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const VariableTypeAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::VariableTypeAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::Value);
    specattr.set(SpecifiedAttributesEncoding::DataType);
    specattr.set(SpecifiedAttributesEncoding::ValueRank);
    specattr.set(SpecifiedAttributesEncoding::ArrayDimensions);
    specattr.set(SpecifiedAttributesEncoding::IsAbstract);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::Value] = attr.Value;
    Attributes[AttributeId::DataType] = attr.Type;
    Attributes[AttributeId::ValueRank] = attr.Rank;
    Attributes[AttributeId::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeId::ArrayDimensions] = attr.Dimensions;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const ReferenceTypeAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::ReferenceTypeAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::IsAbstract);
    specattr.set(SpecifiedAttributesEncoding::Symmetric);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    if (!attr.IsAbstract)
      specattr.set(SpecifiedAttributesEncoding::InverseName);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeId::Symmetric] = attr.Symmetric;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
    if (!attr.IsAbstract)
      Attributes[AttributeId::InverseName] = attr.InverseName;
  }

  NodeAttributes::NodeAttributes(const DataTypeAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::DataTypeAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::IsAbstract);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const ViewAttributes& attr)
  {
    Header.TypeId = ExpandedObjectId::ViewAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::ContainsNoLoops);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeId::DisplayName] = attr.DisplayName;
    Attributes[AttributeId::Description] = attr.Description;
    Attributes[AttributeId::ContainsNoLoops] = attr.ContainsNoLoops;
    Attributes[AttributeId::WriteMask] = attr.WriteMask;
    Attributes[AttributeId::UserWriteMask] = attr.UserWriteMask;
  }



  AddNodesRequest::AddNodesRequest()
    : TypeId(ADD_NODES_REQUEST)
  {
  }

  AddNodesResponse::AddNodesResponse()
    : TypeId(ADD_NODES_RESPONSE)
  {
  }

 AddReferencesRequest::AddReferencesRequest()
    : TypeId(ADD_REFERENCES_REQUEST)
  {
  }

  AddReferencesResponse::AddReferencesResponse()
    : TypeId(ADD_REFERENCES_RESPONSE)
  {
  }

  namespace Binary
  {

    //
    //AddNodeRequest
    //
/*
    template<>
    std::size_t RawSize<ObjectAttributes>(const ObjectAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.EventNotifier) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<ObjectAttributes>(const ObjectAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.EventNotifier;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<ObjectAttributes>(ObjectAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.EventNotifier;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }

    template<>
    std::size_t RawSize<VariableAttributes>(const VariableAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.Value) + 
        RawSize(val.Type) + 
        RawSize(val.Rank) + 
        RawSizeContainer(val.Dimensions) + 
        RawSize((uint8_t)val.AccessLevel) +
        RawSize((uint8_t)val.UserAccessLevel) +
        RawSize(val.MinimumSamplingInterval) + 
        RawSize(val.Historizing) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<VariableAttributes>(const VariableAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.Value;
      *this << val.Type;
      *this << val.Rank;
      *this << val.Dimensions;
      *this << (uint8_t)val.AccessLevel; // TODO
      *this << (uint8_t)val.UserAccessLevel; //TODO
      *this << val.MinimumSamplingInterval;
      *this << val.Historizing;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<VariableAttributes>(VariableAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.Value;
      *this >> val.Type;
      *this >> val.Rank;
      *this >> val.Dimensions;
      uint8_t tmp = 0;
      *this >> tmp; val.AccessLevel = static_cast<VariableAccessLevel>(tmp);
      *this >> tmp; val.UserAccessLevel = static_cast<VariableAccessLevel>(tmp);
      *this >> val.MinimumSamplingInterval;
      *this >> val.Historizing;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }


    template<>
    std::size_t RawSize<MethodAttributes>(const MethodAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.Executable) + 
        RawSize(val.UserExecutable) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<MethodAttributes>(const MethodAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.Executable;
      *this << val.UserExecutable;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<MethodAttributes>(MethodAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.Executable;
      *this >> val.UserExecutable;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }

    template<>
    std::size_t RawSize<ObjectTypeAttributes>(const ObjectTypeAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.IsAbstract) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<ObjectTypeAttributes>(const ObjectTypeAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.IsAbstract;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<ObjectTypeAttributes>(ObjectTypeAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.IsAbstract;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }


    template<>
    std::size_t RawSize<VariableTypeAttributes>(const VariableTypeAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.Value) + 
        RawSize(val.Type) + 
        RawSize(val.Rank) + 
        RawSizeContainer(val.Dimensions) + 
        RawSize(val.IsAbstract) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<VariableTypeAttributes>(const VariableTypeAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.Value;
      *this << val.Type;
      *this << val.Rank;
      *this << val.Dimensions;
      *this << val.IsAbstract;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<VariableTypeAttributes>(VariableTypeAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.Value;
      *this >> val.Type;
      *this >> val.Rank;
      *this >> val.Dimensions;
      *this >> val.IsAbstract;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }


    template<>
    std::size_t RawSize<ReferenceTypeAttributes>(const ReferenceTypeAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.IsAbstract) + 
        RawSize(val.Symmetric) + 
        RawSize(val.InverseName) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<ReferenceTypeAttributes>(const ReferenceTypeAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.IsAbstract;
      *this << val.Symmetric;
      *this << val.InverseName;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<ReferenceTypeAttributes>(ReferenceTypeAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.IsAbstract;
      *this >> val.Symmetric;
      *this >> val.InverseName;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }


    template<>
    std::size_t RawSize<DataTypeAttributes>(const DataTypeAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.IsAbstract) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<DataTypeAttributes>(const DataTypeAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.IsAbstract;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    template<>
    void DataDeserializer::Deserialize<DataTypeAttributes>(DataTypeAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.IsAbstract;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }

    template<>
    std::size_t RawSize<ViewAttributes>(const ViewAttributes& val)
    {
      return RawSize(val.SpecifiedAttributes) + 
        RawSize(val.DisplayName) + 
        RawSize(val.Description) + 
        RawSize(val.ContainsNoLoops) + 
        RawSize(val.WriteMask) + 
        RawSize(val.UserWriteMask)
        ;
    }

    template<>
    void DataSerializer::Serialize<ViewAttributes>(const ViewAttributes& val)
    {
      *this << val.SpecifiedAttributes;
      *this << val.DisplayName;
      *this << val.Description;
      *this << val.ContainsNoLoops;
      *this << val.WriteMask;
      *this << val.UserWriteMask;
    }

    // TODO Seems doesn't required because there is serialization of NodeAttributes
    // TODO Exact attributes have to be initialized from node attributes which will serialize.
    template<>
    void DataDeserializer::Deserialize<ViewAttributes>(ViewAttributes& val)
    {
      *this >> val.SpecifiedAttributes;
      *this >> val.DisplayName;
      *this >> val.Description;
      *this >> val.ContainsNoLoops;
      *this >> val.WriteMask;
      *this >> val.UserWriteMask;
    }
*/


    std::size_t RawSizeBodyNodeAttributes(const NodeAttributes& val)
    {
      size_t total = 0;
      total += RawSize(val.SpecifiedAttributes);
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DisplayName))
      {
        total += RawSize(val.Attributes.find(AttributeId::DisplayName)->second.As<LocalizedText>()); //We assume the attr exists!!!
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Description))
      {
        total += RawSize(val.Attributes.find(AttributeId::Description)->second.As<LocalizedText>());
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::EventNotifier))
      {
        total += 1;//RawSize(val.Attributes.find(AttributeId::EventNotifier)->second.Value.U);
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Value))
      {
        total += RawSize(val.Attributes.find(AttributeId::Value)->second);
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DataType))
      {
        total += RawSize(val.Attributes.find(AttributeId::DataType)->second.As<NodeId>());
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ValueRank))
      {
        total += 4;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ArrayDimensions))
      {
        total += RawSizeContainer(val.Attributes.find(AttributeId::ArrayDimensions)->second.As<std::vector<uint32_t>>());
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::AccessLevel))
      {
        total += 1;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserAccessLevel))
      {
        total += 1;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::MinimumSamplingInterval))
      {
        total += sizeof(double);
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Historizing))
      {
        total += sizeof(bool);
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Executable))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserExecutable))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::IsAbstract))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Symmetric))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::InverseName))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ContainsNoLoops))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::WriteMask))
      {
        total += 4;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserWriteMask))
      {
        total += 4;
      }

      return total;

    }


    template<>
    std::size_t RawSize<NodeAttributes>(const NodeAttributes& val)
    {
      size_t total = 0;
      total += RawSize(val.Header);
      total += 4 ; //uint32_t size body
      total += RawSizeBodyNodeAttributes(val) ;
      return total;
    }



    template<>
    void DataSerializer::Serialize<NodeAttributes>(const NodeAttributes& val)
    {
      *this << val.Header;
      *this << (int32_t) RawSizeBodyNodeAttributes(val); //debug
      *this << val.SpecifiedAttributes;
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DisplayName))
      {
        *this << val.Attributes.find(AttributeId::DisplayName)->second.As<LocalizedText>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Description))
      {
        *this << val.Attributes.find(AttributeId::Description)->second.As<LocalizedText>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::WriteMask))
      {
        *this << val.Attributes.find(AttributeId::WriteMask)->second.As<uint32_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserWriteMask))
      {
        *this << val.Attributes.find(AttributeId::UserWriteMask)->second.As<uint32_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::EventNotifier))
      {
        *this << (uint8_t) val.Attributes.find(AttributeId::EventNotifier)->second.As<uint8_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Value))
      {
        *this << val.Attributes.find(AttributeId::Value)->second;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DataType))
      {
        *this << val.Attributes.find(AttributeId::DataType)->second.As<NodeId>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ValueRank))
      {
        *this << val.Attributes.find(AttributeId::ValueRank)->second.As<int32_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ArrayDimensions))
      {
        *this << val.Attributes.find(AttributeId::ArrayDimensions)->second.As<std::vector<uint32_t>>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::AccessLevel))
      {
        *this << val.Attributes.find(AttributeId::AccessLevel)->second.As<uint8_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserAccessLevel))
      {
        *this << val.Attributes.find(AttributeId::UserAccessLevel)->second.As<uint8_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::MinimumSamplingInterval))
      {
        *this << val.Attributes.find(AttributeId::MinimumSamplingInterval)->second.As<double>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Historizing))
      {
        *this << val.Attributes.find(AttributeId::Historizing)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Executable))
      {
        *this << val.Attributes.find(AttributeId::Executable)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserExecutable))
      {
        *this << val.Attributes.find(AttributeId::UserExecutable)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::IsAbstract))
      {
        *this << val.Attributes.find(AttributeId::IsAbstract)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Symmetric))
      {
        *this << val.Attributes.find(AttributeId::Symmetric)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::InverseName))
      {
        *this << val.Attributes.find(AttributeId::InverseName)->second.As<LocalizedText>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ContainsNoLoops))
      {
        *this << val.Attributes.find(AttributeId::ContainsNoLoops)->second.As<bool>();
      }

    }

    template<>
    void DataDeserializer::Deserialize<NodeAttributes>(NodeAttributes& val)
    {
      *this >> val.Header;
      int32_t size;
      *this >> size ; // not used yet
      *this >> val.SpecifiedAttributes;

      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DisplayName))
      {
        LocalizedText tmp;
        *this >> tmp;
        val.Attributes[AttributeId::DisplayName] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Description))
      {
        LocalizedText tmp;
        *this >> tmp;
        val.Attributes[AttributeId::Description] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::WriteMask))
      {
        uint32_t tmp;
        *this >> tmp;
        val.Attributes[AttributeId::WriteMask] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserWriteMask))
      {
        uint32_t tmp;
        *this >> tmp;
        val.Attributes[AttributeId::UserWriteMask] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::EventNotifier))
      {
        uint8_t tmp;
        *this >> tmp;
        val.Attributes[AttributeId::EventNotifier] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Value))
      {
        Variant tmp;
        *this >> tmp;
        val.Attributes[AttributeId::Value] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DataType))
      {
        NodeId tmp;
        *this >> tmp;
        val.Attributes[AttributeId::DataType] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ValueRank))
      {
        int32_t tmp;
        *this >> tmp;
        val.Attributes[AttributeId::ValueRank] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ArrayDimensions))
      {
        std::vector<uint32_t> tmp;
        *this >> tmp;
        val.Attributes[AttributeId::ArrayDimensions] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::AccessLevel))
      {
        uint8_t tmp;
        *this >> tmp;
        val.Attributes[AttributeId::AccessLevel] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserAccessLevel))
      {
        uint8_t tmp;
        *this >> tmp;
        val.Attributes[AttributeId::UserAccessLevel] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::MinimumSamplingInterval))
      {
        Duration tmp;
        *this >> tmp;
        val.Attributes[AttributeId::MinimumSamplingInterval] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Historizing))
      {
        bool tmp;
        *this >> tmp;
        val.Attributes[AttributeId::Historizing] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Executable))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserExecutable))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::IsAbstract))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Symmetric))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::InverseName))
      {
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ContainsNoLoops))
      {
      }

    }

    template<>
    std::size_t RawSize<AddNodesItem>(const AddNodesItem& resp)
    {
      return RawSize(resp.ParentNodeId) + 
        RawSize(resp.ReferenceTypeId) +
        RawSize(resp.RequestedNewNodeId) +
        RawSize(resp.BrowseName) +
        RawSize(resp.Class) +
        RawSize(resp.Attributes) +
        RawSize(resp.TypeDefinition);
    }

    template<>
    void DataSerializer::Serialize<AddNodesItem>(const AddNodesItem& resp)
    {
      *this << resp.ParentNodeId;
      *this << resp.ReferenceTypeId;
      *this << resp.RequestedNewNodeId;
      *this << resp.BrowseName;
      *this << resp.Class;
      *this << resp.Attributes;
      *this << resp.TypeDefinition;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesItem>(AddNodesItem& resp)
    {
      *this >> resp.ParentNodeId;
      *this >> resp.ReferenceTypeId;
      *this >> resp.RequestedNewNodeId;
      *this >> resp.BrowseName;
      *this >> resp.Class;
      *this >> resp.Attributes;
      *this >> resp.TypeDefinition;
    }

    template<>
    std::size_t RawSize(const std::vector<AddNodesItem>& ack)
    {
      return RawSizeContainer(ack);
    }


    template<>
    void DataDeserializer::Deserialize<std::vector<AddNodesItem>>(std::vector<AddNodesItem>& ack)
    {
      DeserializeContainer(*this, ack);
    }

    template<>
    void DataSerializer::Serialize<std::vector<AddNodesItem>>(const std::vector<AddNodesItem>& ack)
    {
      SerializeContainer(*this, ack);
    }

    template<>
    std::size_t RawSize<AddNodesParameters>(const AddNodesParameters& val)
    {
      return RawSize(val.NodesToAdd);
    }

    template<>
    void DataSerializer::Serialize<AddNodesParameters>(const AddNodesParameters& val)
    {
      *this << val.NodesToAdd;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesParameters>(AddNodesParameters& resp)
    {
      *this >> resp.NodesToAdd;
    }



    template<>
    std::size_t RawSize<AddNodesRequest>(const AddNodesRequest& resp)
    {
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Parameters);
    }

    template<>
    void DataSerializer::Serialize<AddNodesRequest>(const AddNodesRequest& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesRequest>(AddNodesRequest& resp)
    {
      *this >> resp.TypeId;
      *this >> resp.Header;
      *this >> resp.Parameters;
    }

    template<>
    std::size_t RawSize<AddNodesResult>(const AddNodesResult& resp)
    {
      return RawSize(resp.Status) + RawSize(resp.AddedNodeId);
    }

    template<>
    void DataSerializer::Serialize<AddNodesResult>(const AddNodesResult& resp)
    {
      *this << resp.Status;
      *this << resp.AddedNodeId;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesResult>(AddNodesResult& resp)
    {
      *this >> resp.Status;
      *this >> resp.AddedNodeId;
    }


    template<>
    void DataDeserializer::Deserialize<std::vector<AddNodesResult>>(std::vector<AddNodesResult>& ack)
    {
      DeserializeContainer(*this, ack);
    }

    template<>
    void DataSerializer::Serialize<std::vector<AddNodesResult>>(const std::vector<AddNodesResult>& ack)
    {
      SerializeContainer(*this, ack);
    }






    template<>
    std::size_t RawSize<AddNodesResponse>(const AddNodesResponse& resp)
    {
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSizeContainer(resp.results) + RawSize(resp.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<AddNodesResponse>(const AddNodesResponse& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.results;
      *this << resp.Diagnostics;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesResponse>(AddNodesResponse& resp)
    {
      *this >> resp.TypeId;
      *this >> resp.Header;
      *this >> resp.results;
      *this >> resp.Diagnostics;
    }

  
    //
    // AddReferences
    //




    template<>
    std::size_t RawSize<AddReferencesResponse>(const AddReferencesResponse& resp)
    {
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSizeContainer(resp.Results) + RawSize(resp.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<AddReferencesResponse>(const AddReferencesResponse& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.Results;
      *this << resp.Diagnostics;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesResponse>(AddReferencesResponse& resp)
    {
      *this >> resp.TypeId;
      *this >> resp.Header;
      *this >> resp.Results;
      *this >> resp.Diagnostics;
    }

     template<>
    std::size_t RawSize<AddReferencesItem>(const AddReferencesItem& val)
    {
      return RawSize(val.SourceNodeId) + 
        RawSize(val.ReferenceTypeId) + 
        RawSize(val.IsForward) + 
        RawSize(val.TargetServerUri) + 
        RawSize(val.TargetNodeId) + 
        RawSize(val.TargetNodeClass)
        ;
    }

    template<>
    void DataSerializer::Serialize<AddReferencesItem>(const AddReferencesItem& val)
    {
      *this << val.SourceNodeId;
      *this << val.ReferenceTypeId;
      *this << val.IsForward;
      *this << val.TargetServerUri;
      *this << val.TargetNodeId;
      *this << val.TargetNodeClass;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesItem>(AddReferencesItem& val)
    {
      *this >> val.SourceNodeId;
      *this >> val.ReferenceTypeId;
      *this >> val.IsForward;
      *this >> val.TargetServerUri;
      *this >> val.TargetNodeId;
      *this >> val.TargetNodeClass;
    }

    template<>
    std::size_t RawSize(const std::vector<AddReferencesItem>& ack)
    {
      return RawSizeContainer(ack);
    }



    template<>
    void DataDeserializer::Deserialize<std::vector<AddReferencesItem>>(std::vector<AddReferencesItem>& ack)
    {
      DeserializeContainer(*this, ack);
    }

    template<>
    void DataSerializer::Serialize<std::vector<AddReferencesItem>>(const std::vector<AddReferencesItem>& ack)
    {
      SerializeContainer(*this, ack);
    }


     template<>
    std::size_t RawSize<AddReferencesParameters>(const AddReferencesParameters& val)
    {
      return RawSizeContainer(val.ReferencesToAdd);
    }

    template<>
    void DataSerializer::Serialize<AddReferencesParameters>(const AddReferencesParameters& val)
    {
      *this << val.ReferencesToAdd;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesParameters>(AddReferencesParameters& val)
    {
      *this >> val.ReferencesToAdd;
    }




     template<>
    std::size_t RawSize<AddReferencesRequest>(const AddReferencesRequest& val)
    {
      return RawSize(val.TypeId) +  RawSize(val.Header) + 
        RawSize(val.Parameters)
        ;
    }

    template<>
    void DataSerializer::Serialize<AddReferencesRequest>(const AddReferencesRequest& val)
    {
      *this << val.TypeId;
      *this << val.Header;
      *this << val.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesRequest>(AddReferencesRequest& val)
    {
      *this >> val.TypeId;
      *this >> val.Header;
      *this >> val.Parameters;
    }




  } // namespace Binary

  const char* GetObjectIdName(const NodeId& id)
  {
    if ( id == ObjectId::FolderType )
    {
      return Names::FolderType;
    }
    else if ( id == ObjectId::BaseVariableType )
    {
      return Names::BaseVariableType;
    }
    else if ( id == ObjectId::BaseDataVariableType )
    {
      return Names::BaseDataVariableType;
    }
    else if ( id == ObjectId::PropertyType )
    {
      return Names::PropertyType;
    }
    else if ( id == ObjectId::BaseObjectType )
    {
      return Names::BaseObjectType;
    }
    else if (id == ObjectId::ModellingRule_Mandatory)
    {
      return Names::ModellingRule_Mandatory;
    }
    else if ( id == ObjectId::ServerType )
    {
      return Names::ServerType;
    }
    return "NoNameFound";
  }

}
