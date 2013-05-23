/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Well known attributes identifiers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_IDENTIFIERS
#define __OPC_UA_BINARY_IDENTIFIERS

namespace OpcUa
{
  enum class ObjectID : uint32_t
  {
    Null           = 0,
    Boolean        = 1,
    SByte          = 2,
    Byte           = 3,
    Int16          = 4,
    UInt16         = 5,
    Int32          = 6,
    UInt32         = 7,
    Int64          = 8,
    UInt64         = 9,
    Float          = 10,
    Double         = 11,
    String         = 12,
    DateTime       = 13,
    Guid           = 14,
    ByteString     = 15,
    XmlElement     = 16,
    NodeID         = 17,
    ExpandedNodeID = 18,
    StatusCode     = 19,
    QualifiedName  = 20,
    LocalizedText  = 21,
    Structure      = 22,
    DataValue      = 23,
    BaseDataType   = 24,
    DiagnosticInfo = 25,
    Number         = 26,
    Integer        = 27,
    UInteger       = 28,
    Enumeration    = 29,
    Image          = 30,
    FolderType     = 61,
    PropertyType   = 68,
    RootFolder     = 84,
    ObjectsFolder  = 85,
    TypesFolder    = 86,
    ViewsFolder    = 87,
    ObjectTypes    = 88,
    VariableTypes  = 89,
    DataTypes      = 90,
    ReferenceTypes = 91,

    References     = 31,
    NonHierarchicalReferences = 32,
    HierarchicalReferences = 33,
    HasChild       = 34,
    Organizes      = 35,
    HasEventSource = 36,
    HasModellingRule = 37,
    HasEncoding    = 38,
    HasDescription = 39,
    HasTypeDefinition = 40,
    GeneratesEvent = 41,
    Aggregates     = 44,
    HasSubtype     = 45,
    HasProperty    = 46,
    HasComponent   = 47,
    HasNotifier    = 48,
    HasOrderedComponent = 49,
    HasModelParent = 50,
    FromState      = 51,
    ToState        = 52,
    HasCause       = 53,
    HasEffect      = 54,
    HasHistoricalConfiguration = 56,
    HasHistoricalEventConfiguration = 57,
    HasSubStateMachine = 117,
    HasEventHistory = 118,
    IdType         = 256,
    NodeClass      = 257,
    Duration       = 290,
    UtcTime        = 294,
    MessageSecurityMode = 302,
    SecurityTokenRequestType = 315,
    RedundancySupport = 851,
    ServerState    = 852,
    ImageBmp       = 2000,
    ImageGif       = 2001,
    ImageJpg       = 2002,
    ImagePng       = 2003,
    EventTypes     = 3048,
    AlwaysGeneratesEvent = 3065,
    IdTypeEnumStrings = 7591,
    MessageSecurityModeEnumStrings = 7595,
    SecurityTokenRequestTypeEnumStrings = 7598,
    RedundancySupportEnumStrings = 7611,
    ServerStateEnumStrings = 7612,
    HasTrueSubState = 9004,
    HasFalseSubState = 9005,
    HasCondition   = 9006,
  };
}

#endif // __OPC_UA_BINARY_IDENTIFIERS
