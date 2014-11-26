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
    Header.TypeID = ExpandedObjectID::ObjectAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::EventNotifier);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::EventNotifier] = attr.EventNotifier;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const VariableAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::VariableAttribute;
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

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::Value] = attr.Value;
    Attributes[AttributeID::DataType] = attr.Type;
    Attributes[AttributeID::ValueRank] = attr.Rank;
    Attributes[AttributeID::ArrayDimensions] = attr.Dimensions;
    Attributes[AttributeID::AccessLevel] = static_cast<uint8_t>(attr.AccessLevel);
    Attributes[AttributeID::UserAccessLevel] = static_cast<uint8_t>(attr.UserAccessLevel);
    Attributes[AttributeID::MinimumSamplingInterval] = attr.MinimumSamplingInterval;
    Attributes[AttributeID::Historizing] = attr.Historizing;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const MethodAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::MethodAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::Executable);
    specattr.set(SpecifiedAttributesEncoding::UserExecutable);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::Executable] = attr.Executable;
    Attributes[AttributeID::UserExecutable] = attr.UserExecutable;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }
  NodeAttributes::NodeAttributes(const ObjectTypeAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::ObjectTypeAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::IsAbstract);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const VariableTypeAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::VariableTypeAttribute;
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

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::Value] = attr.Value;
    Attributes[AttributeID::DataType] = attr.Type;
    Attributes[AttributeID::ValueRank] = attr.Rank;
    Attributes[AttributeID::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeID::ArrayDimensions] = attr.Dimensions;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const ReferenceTypeAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::ReferenceTypeAttribute;
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

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeID::Symmetric] = attr.Symmetric;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
    if (!attr.IsAbstract)
      Attributes[AttributeID::InverseName] = attr.InverseName;
  }

  NodeAttributes::NodeAttributes(const DataTypeAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::DataTypeAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::IsAbstract);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::IsAbstract] = attr.IsAbstract;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }

  NodeAttributes::NodeAttributes(const ViewAttributes& attr)
  {
    Header.TypeID = ExpandedObjectID::ViewAttribute;
    Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);

    std::bitset<32> specattr;
    specattr.set(SpecifiedAttributesEncoding::DisplayName);
    specattr.set(SpecifiedAttributesEncoding::Description);
    specattr.set(SpecifiedAttributesEncoding::ContainsNoLoops);
    specattr.set(SpecifiedAttributesEncoding::WriteMask);
    specattr.set(SpecifiedAttributesEncoding::UserWriteMask);
    SpecifiedAttributes = (uint32_t) specattr.to_ulong();

    Attributes[AttributeID::DisplayName] = attr.DisplayName;
    Attributes[AttributeID::Description] = attr.Description;
    Attributes[AttributeID::ContainsNoLoops] = attr.ContainsNoLoops;
    Attributes[AttributeID::WriteMask] = attr.WriteMask;
    Attributes[AttributeID::UserWriteMask] = attr.UserWriteMask;
  }



  AddNodesRequest::AddNodesRequest()
    : TypeID(ADD_NODES_REQUEST)
  {
  }

  AddNodesResponse::AddNodesResponse()
    : TypeID(ADD_NODES_RESPONSE)
  {
  }

 AddReferencesRequest::AddReferencesRequest()
    : TypeID(ADD_REFERENCES_REQUEST)
  {
  }

  AddReferencesResponse::AddReferencesResponse()
    : TypeID(ADD_REFERENCES_RESPONSE)
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
        total += RawSize(val.Attributes.find(AttributeID::DisplayName)->second.As<LocalizedText>()); //We assume the attr exists!!!
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Description))
      {
        total += RawSize(val.Attributes.find(AttributeID::Description)->second.As<LocalizedText>());
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::EventNotifier))
      {
        total += 1;//RawSize(val.Attributes.find(AttributeID::EventNotifier)->second.Value.U);
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Value))
      {
        total += RawSize(val.Attributes.find(AttributeID::Value)->second);
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DataType))
      {
        total += RawSize(val.Attributes.find(AttributeID::DataType)->second.As<NodeID>());
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ValueRank))
      {
        total += 4;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ArrayDimensions))
      {
        total += RawSizeContainer(val.Attributes.find(AttributeID::ArrayDimensions)->second.As<std::vector<uint32_t>>());
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
        *this << val.Attributes.find(AttributeID::DisplayName)->second.As<LocalizedText>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Description))
      {
        *this << val.Attributes.find(AttributeID::Description)->second.As<LocalizedText>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::WriteMask))
      {
        *this << val.Attributes.find(AttributeID::WriteMask)->second.As<uint32_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserWriteMask))
      {
        *this << val.Attributes.find(AttributeID::UserWriteMask)->second.As<uint32_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::EventNotifier))
      {
        *this << (uint8_t) val.Attributes.find(AttributeID::EventNotifier)->second.As<uint8_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Value))
      {
        *this << val.Attributes.find(AttributeID::Value)->second;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DataType))
      {
        *this << val.Attributes.find(AttributeID::DataType)->second.As<NodeID>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ValueRank))
      {
        *this << val.Attributes.find(AttributeID::ValueRank)->second.As<int32_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ArrayDimensions))
      {
        *this << val.Attributes.find(AttributeID::ArrayDimensions)->second.As<std::vector<uint32_t>>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::AccessLevel))
      {
        *this << val.Attributes.find(AttributeID::AccessLevel)->second.As<uint8_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserAccessLevel))
      {
        *this << val.Attributes.find(AttributeID::UserAccessLevel)->second.As<uint8_t>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::MinimumSamplingInterval))
      {
        *this << val.Attributes.find(AttributeID::MinimumSamplingInterval)->second.As<double>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Historizing))
      {
        *this << val.Attributes.find(AttributeID::Historizing)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Executable))
      {
        *this << val.Attributes.find(AttributeID::Executable)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserExecutable))
      {
        *this << val.Attributes.find(AttributeID::UserExecutable)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::IsAbstract))
      {
        *this << val.Attributes.find(AttributeID::IsAbstract)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Symmetric))
      {
        *this << val.Attributes.find(AttributeID::Symmetric)->second.As<bool>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::InverseName))
      {
        *this << val.Attributes.find(AttributeID::InverseName)->second.As<LocalizedText>();
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ContainsNoLoops))
      {
        *this << val.Attributes.find(AttributeID::ContainsNoLoops)->second.As<bool>();
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
        val.Attributes[AttributeID::DisplayName] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Description))
      {
        LocalizedText tmp;
        *this >> tmp;
        val.Attributes[AttributeID::Description] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::WriteMask))
      {
        uint32_t tmp;
        *this >> tmp;
        val.Attributes[AttributeID::WriteMask] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserWriteMask))
      {
        uint32_t tmp;
        *this >> tmp;
        val.Attributes[AttributeID::UserWriteMask] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::EventNotifier))
      {
        uint8_t tmp;
        *this >> tmp;
        val.Attributes[AttributeID::EventNotifier] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Value))
      {
        Variant tmp;
        *this >> tmp;
        val.Attributes[AttributeID::Value] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::DataType))
      {
        NodeID tmp;
        *this >> tmp;
        val.Attributes[AttributeID::DataType] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ValueRank))
      {
        int32_t tmp;
        *this >> tmp;
        val.Attributes[AttributeID::ValueRank] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::ArrayDimensions))
      {
        std::vector<uint32_t> tmp;
        *this >> tmp;
        val.Attributes[AttributeID::ArrayDimensions] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::AccessLevel))
      {
        uint8_t tmp;
        *this >> tmp;
        val.Attributes[AttributeID::AccessLevel] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::UserAccessLevel))
      {
        uint8_t tmp;
        *this >> tmp;
        val.Attributes[AttributeID::UserAccessLevel] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::MinimumSamplingInterval))
      {
        Duration tmp;
        *this >> tmp;
        val.Attributes[AttributeID::MinimumSamplingInterval] = tmp;
      }
      if (std::bitset<32>(val.SpecifiedAttributes).test(SpecifiedAttributesEncoding::Historizing))
      {
        bool tmp;
        *this >> tmp;
        val.Attributes[AttributeID::Historizing] = tmp;
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
        RawSize(resp.RequestedNewNodeID) +
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
      *this << resp.RequestedNewNodeID;
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
      *this >> resp.RequestedNewNodeID;
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
      return RawSize(resp.TypeID) + RawSize(resp.Header) + RawSize(resp.Parameters);
    }

    template<>
    void DataSerializer::Serialize<AddNodesRequest>(const AddNodesRequest& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;
      *this << resp.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesRequest>(AddNodesRequest& resp)
    {
      *this >> resp.TypeID;
      *this >> resp.Header;
      *this >> resp.Parameters;
    }

    template<>
    std::size_t RawSize<AddNodesResult>(const AddNodesResult& resp)
    {
      return RawSize(resp.Status) + RawSize(resp.AddedNodeID);
    }

    template<>
    void DataSerializer::Serialize<AddNodesResult>(const AddNodesResult& resp)
    {
      *this << resp.Status;
      *this << resp.AddedNodeID;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesResult>(AddNodesResult& resp)
    {
      *this >> resp.Status;
      *this >> resp.AddedNodeID;
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
      return RawSize(resp.TypeID) + RawSize(resp.Header) + RawSizeContainer(resp.results) + RawSize(resp.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<AddNodesResponse>(const AddNodesResponse& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;
      *this << resp.results;
      *this << resp.Diagnostics;
    }

    template<>
    void DataDeserializer::Deserialize<AddNodesResponse>(AddNodesResponse& resp)
    {
      *this >> resp.TypeID;
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
      return RawSize(resp.TypeID) + RawSize(resp.Header) + RawSizeContainer(resp.Results) + RawSize(resp.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<AddReferencesResponse>(const AddReferencesResponse& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;
      *this << resp.Results;
      *this << resp.Diagnostics;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesResponse>(AddReferencesResponse& resp)
    {
      *this >> resp.TypeID;
      *this >> resp.Header;
      *this >> resp.Results;
      *this >> resp.Diagnostics;
    }

     template<>
    std::size_t RawSize<AddReferencesItem>(const AddReferencesItem& val)
    {
      return RawSize(val.SourceNodeID) + 
        RawSize(val.ReferenceTypeId) + 
        RawSize(val.IsForward) + 
        RawSize(val.TargetServerUri) + 
        RawSize(val.TargetNodeID) + 
        RawSize(val.TargetNodeClass)
        ;
    }

    template<>
    void DataSerializer::Serialize<AddReferencesItem>(const AddReferencesItem& val)
    {
      *this << val.SourceNodeID;
      *this << val.ReferenceTypeId;
      *this << val.IsForward;
      *this << val.TargetServerUri;
      *this << val.TargetNodeID;
      *this << val.TargetNodeClass;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesItem>(AddReferencesItem& val)
    {
      *this >> val.SourceNodeID;
      *this >> val.ReferenceTypeId;
      *this >> val.IsForward;
      *this >> val.TargetServerUri;
      *this >> val.TargetNodeID;
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
      return RawSize(val.TypeID) +  RawSize(val.Header) + 
        RawSize(val.Parameters)
        ;
    }

    template<>
    void DataSerializer::Serialize<AddReferencesRequest>(const AddReferencesRequest& val)
    {
      *this << val.TypeID;
      *this << val.Header;
      *this << val.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<AddReferencesRequest>(AddReferencesRequest& val)
    {
      *this >> val.TypeID;
      *this >> val.Header;
      *this >> val.Parameters;
    }




  } // namespace Binary

  const char* GetObjectIdName(const NodeID& id)
  {
    if ( id == ObjectID::FolderType )
    {
      return Names::FolderType;
    }
    else if ( id == ObjectID::BaseVariableType )
    {
      return Names::BaseVariableType;
    }
    else if ( id == ObjectID::BaseDataVariableType )
    {
      return Names::BaseDataVariableType;
    }
    else if ( id == ObjectID::PropertyType )
    {
      return Names::PropertyType;
    }
    else if ( id == ObjectID::BaseObjectType )
    {
      return Names::BaseObjectType;
    }
    else if (id == ObjectID::ModellingRule_Mandatory)
    {
      return Names::ModellingRule_Mandatory;
    }
    else if ( id == ObjectID::ServerType )
    {
      return Names::ServerType;
    }
    return "NoNameFound";
  }

}
