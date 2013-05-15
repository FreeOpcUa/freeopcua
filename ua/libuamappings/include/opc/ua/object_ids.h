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
    Null = 0,
    FolderType = 61,
    RootFolder = 84,
    ObjectsFolder = 85,
    TypesFolder = 86,
    ViewsFolder = 87,
    ReferenceTypes = 91,

    References = 31,
    NonHierarchicalReferences = 32,
    HierarchicalReferences = 33,
    HasChild = 34,
    Organizes = 35,
    HasEventSource = 36,
    HasModellingRule = 37,
    HasEncoding = 38,
    HasDescription = 39,
    HasTypeDefinition = 40,
    GeneratesEvent = 41,
    Aggregates = 44,
    HasSubtype = 45,
    HasProperty = 46,
    HasComponent = 47,
    HasNotifier = 48,
    HasOrderedComponent = 49,
    HasModelParent = 50,
    FromState = 51,
    ToState = 52,
    HasCause = 53,
    HasEffect = 54,
    HasHistoricalConfiguration = 56,
    HasHistoricalEventConfiguration = 57,
    HasSubStateMachine = 117,
    HasEventHistory = 118,
    AlwaysGeneratesEvent = 3065,
    HasTrueSubState = 9004,
    HasFalseSubState = 9005,
    HasCondition = 9006,
  };
}

#endif // __OPC_UA_BINARY_IDENTIFIERS
