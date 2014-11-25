/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>

#include "opc/ua/protocol/attribute_ids.h"
#include "opc/ua/protocol/types.h"
#include "opc/ua/protocol/variant.h"
#include "opc/ua/protocol/view.h"

#include "py_opcua_enums.h"

using namespace boost::python;
using namespace OpcUa;

extern void py_opcua_enums_ObjectID();
extern void py_opcua_enums_StatusCode();

void py_opcua_enums()
{

  // codegen
  py_opcua_enums_ObjectID();
  py_opcua_enums_StatusCode();

  enum_<ApplicationType>("ApplicationType")
#define _value(X) value(#X, ApplicationType:: X)
  ._value(CLIENT)
  ._value(CLIENT_AND_SERVER)
  ._value(DISCOVERY_SERVER)
  ._value(SERVER)
#undef _value
  ;

  enum_<MessageSecurityMode>("MessageSecurityMode")
#define _value(X) value(#X, MessageSecurityMode:: X)
  ._value(MSM_NONE)
  ._value(MSM_SIGN)
  ._value(MSM_SIGN_AND_ENCRYPT)
#undef _value
  ;

  enum_<UserIdentifyTokenType>("UserIdentifyTokenType")
#define _value(X) value(#X, UserIdentifyTokenType:: X)
  ._value(ANONYMOUS)
  ._value(USERNAME)
  ._value(CERTIFICATE)
  ._value(ISSUED_TOKEN)
#undef _value
  ;

  enum_<BrowseDirection>("BrowseDirection")
#define _value(X) value(#X, BrowseDirection:: X)
  ._value(Both)
  ._value(Forward)
  ._value(Inverse)
#undef _value
  ;

  enum_<NodeClass>("NodeClass")
#define _value(X) value(#X, NodeClass:: X)
  ._value(All)
  ._value(Object)
  ._value(Variable)
  ._value(Method)
  ._value(ObjectType)
  ._value(VariableType)
  ._value(ReferenceType)
  ._value(DataType)
  ._value(View)
#undef _value
  ;

  enum_<TimestampsToReturn>("TimestampsToReturn")
#define _value(X) value(#X, TimestampsToReturn:: X)
  ._value(SOURCE)
  ._value(SERVER)
  ._value(BOTH)
  ._value(NEITHER)
#undef _value
  ;

  enum_<AttributeID>("AttributeID")
#define _value(X) value(#X, AttributeID:: X)
  ._value(ACCESS_LEVEL)
  ._value(ARRAY_DIMENSIONS)
  ._value(BROWSE_NAME)
  ._value(CONTAINS_NO_LOOPS)
  ._value(DATA_TYPE)
  ._value(DESCRIPTION)
  ._value(DISPLAY_NAME)
  ._value(EVENT_NOTIFIER)
  ._value(EXECUTABLE)
  ._value(HISTORIZING)
  ._value(INVERSE_NAME)
  ._value(IS_ABSTRACT)
  ._value(MINIMUM_SAMPLING_INTERVAL)
  ._value(NODE_CLASS)
  ._value(NODE_ID)
  ._value(SYMMETRIC)
  ._value(UNKNOWN)
  ._value(USER_ACCESS_LEVEL)
  ._value(USER_EXECUTABLE)
  ._value(VALUE)
  ._value(VALUE_RANK)
  ._value(WRITE_MASK)
#undef _value
  ;

  enum_<VariantType>("VariantType")
#define _value(X) value(#X, VariantType:: X )
  ._value(NUL)
  ._value(BOOLEAN)
  ._value(SBYTE)
  ._value(BYTE)
  ._value(INT16)
  ._value(UINT16)
  ._value(INT32)
  ._value(UINT32)
  ._value(INT64)
  ._value(UINT64)
  ._value(FLOAT)
  ._value(DOUBLE)
  ._value(STRING)
  ._value(DATE_TIME)
  ._value(GUID)
  ._value(BYTE_STRING)
  ._value(XML_ELEMENT)
  ._value(NODE_ID)
  ._value(EXPANDED_NODE_ID)
  ._value(STATUS_CODE)
  ._value(QUALIFIED_NAME)
  ._value(LOCALIZED_TEXT)
  ._value(EXTENSION_OBJECT)
  ._value(DATA_VALUE)
  ._value(VARIANT)
  ._value(DIAGNOSTIC_INFO)
#undef _value
  ;
}

