/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Well known referencies identifiers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_referencie_ids_h
#define opc_ua_referencie_ids_h

#include <stdint.h>

namespace OpcUa
{

  enum class ReferenceId : uint32_t
  {
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

    Unknown = ~uint32_t()
  };

} // namespace OpcUa
#endif // opc_ua_referencie_ids_h

