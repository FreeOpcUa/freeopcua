/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <boost/python.hpp>

#include "opc/ua/protocol/attribute_ids.h"
#include "opc/ua/protocol/types.h"
#include "opc/ua/protocol/variant.h"
#include "opc/ua/protocol/view.h"

#include "py_opcua_enums.h"

using namespace boost::python;
using namespace OpcUa;

extern void py_opcua_enums_ObjectId();
extern void py_opcua_enums_StatusCode();
extern void py_opcua_enums_AttributeId();

void py_opcua_enums()
{

  // codegen
  py_opcua_enums_ObjectId();
  py_opcua_enums_StatusCode();
  py_opcua_enums_AttributeId();

  enum_<ApplicationType>("ApplicationType")
#define _value(X) value(#X, ApplicationType:: X)
  ._value(Client)
  ._value(ClientAndServer)
  ._value(DiscoveryServer)
  ._value(Server)
#undef _value
  ;

  enum_<MessageSecurityMode>("MessageSecurityMode")
#define _value(X) value(#X, MessageSecurityMode:: X)
  ._value(Invalid)
  ._value(None)
  ._value(Sign)
  ._value(SignAndEncrypt)
#undef _value
  ;

  enum_<UserTokenType>("UserTokenType")
#define _value(X) value(#X, UserTokenType:: X)
  ._value(Anonymous)
  ._value(UserName)
  ._value(Certificate)
  ._value(IssuedToken)
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
  ._value(Unspecified)
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
  ._value(Source)
  ._value(Server)
  ._value(Both)
  ._value(Neither)
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
  ._value(GUId)
  ._value(BYTE_STRING)
  ._value(XML_ELEMENT)
  ._value(NODE_Id)
  ._value(EXPANDED_NODE_Id)
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

