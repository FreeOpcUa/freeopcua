/// @author Olivier Roulet-Dubonnet 2014 
/// @email olivier.roulet@gmail.com
/// @brief Opc Ua Binary. Attributes service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_NODEMANAGEMENT_H_
#define __OPC_UA_BINARY_NODEMANAGEMENT_H_

#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/node_classes.h>
#include <map>
#include <bitset>

namespace OpcUa
{

  /*
   * AaddNode Message definition
  */
  
  enum SpecifiedAttributesEncoding : uint32_t
  {
    AccessLevel = 0,
    ArrayDimensions = 1,
    ContainsNoLoops = 3,
    DataType = 4,
    Description = 5,
    DisplayName = 6,
    EventNotifier = 7,
    Executable = 8,
    Historizing = 9,
    InverseName = 10,
    IsAbstract = 11,
    MinimumSamplingInterval = 12,
    Symmetric = 15,
    UserAccessLevel = 16,
    UserExecutable = 17,
    UserWriteMask = 18,
    ValueRank = 19,
    WriteMask = 20,
    Value = 21,
  };

  typedef std::bitset<sizeof(uint32_t)> IntBits;

  struct ObjectAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    uint8_t EventNotifier;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct VariableAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    Variant Value;
    NodeID Type;
    int32_t Rank;
    std::vector<uint32_t> Dimensions;
    uint8_t AccessLevel;
    uint8_t UserAccessLevel;
    Duration MinimumSamplingInterval;
    bool Historizing;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct MethodAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    bool Executable;
    bool UserExecutable;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct ObjectTypeAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    bool IsAbstract;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct VariableTypeAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    Variant Value;
    NodeID Type;
    int32_t Rank;
    std::vector<uint32_t> Dimensions;
    bool IsAbstract;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct ReferenceTypeAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    bool IsAbstract;
    bool Symmetric;
    LocalizedText InverseName;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct DataTypeAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    bool IsAbstract;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct ViewAttributes
  {
    uint32_t SpecifiedAttributes;
    LocalizedText DisplayName;
    LocalizedText Description;
    bool ContainsNoLoops;
    uint32_t WriteMask;
    uint32_t UserWriteMask;

  };

  struct NodeAttributes
  {
    ExtensionObjectHeader Header;
    uint32_t SpecifiedAttributes;
    std::map<AttributeID, Variant> Attributes;

    NodeAttributes(){}
    NodeAttributes(const ObjectAttributes&);
    NodeAttributes(const VariableAttributes&);
    NodeAttributes(const MethodAttributes&);
    NodeAttributes(const ObjectTypeAttributes&);
    NodeAttributes(const VariableTypeAttributes&);
    NodeAttributes(const ReferenceTypeAttributes&);
    NodeAttributes(const DataTypeAttributes&);
    NodeAttributes(const ViewAttributes&);

  };

  struct AddNodesItem
  {
    NodeID ParentNodeId;
    NodeID ReferenceTypeId;
    NodeID RequestedNewNodeID;
    QualifiedName BrowseName;
    NodeClass Class;
    NodeAttributes Attributes;
    NodeID TypeDefinition;

  };

  struct AddNodesParameters
  {
    std::vector<AddNodesItem> NodesToAdd;
  };

  struct AddNodesRequest
  {
    NodeID TypeID;
    RequestHeader Header;
    AddNodesParameters Parameters;
    AddNodesRequest();
  };

  struct AddNodesResult
  {
    StatusCode Status;
    NodeID AddedNodeID;
  };
  
  struct AddNodesResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    std::vector<AddNodesResult> results;
    DiagnosticInfoList Diagnostics;
    AddNodesResponse();
  };


  //
  // AddReference
  //

  struct AddReferencesItem
  {
    NodeID SourceNodeID;
    NodeID ReferenceTypeId;
    bool IsForward;
    std::string TargetServerUri;
    NodeID TargetNodeID;
    NodeClass TargetNodeClass;
  };

  struct AddReferencesParameters
  {
    std::vector<AddReferencesItem> ReferencesToAdd;
  };

  struct AddReferencesRequest
  {
    NodeID TypeID;
    RequestHeader Header;
    AddReferencesParameters Parameters;

    AddReferencesRequest();
  };

  struct AddReferencesResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    std::vector<StatusCode> Results;
    DiagnosticInfoList Diagnostics;

    AddReferencesResponse();
  };

  //FIXME: move somewhere else!!
  const char* GetObjectIdName(const NodeID& id) ;


}

#endif 
