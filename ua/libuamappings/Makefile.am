# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $



opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua

opcuainclude_HEADERS = \
  include/opc/ua/protocol/attribute.h \
  include/opc/ua/protocol/endpoints.h \
  include/opc/ua/protocol/types.h \
  include/opc/ua/protocol/binary/stream.h \
  include/opc/ua/protocol/binary/common.h \
  include/opc/ua/protocol/binary/secure_connection.h \
  include/opc/ua/protocol/variant.h \
  include/opc/ua/protocol/secure_channel.h \
  include/opc/ua/protocol/datetime.h \
  include/opc/ua/protocol/view.h \
  include/opc/ua/protocol/session.h \
  include/opc/ua/protocol/data_value.h \
  include/opc/ua/extension_identifiers.h \
  include/opc/ua/channel.h \
  include/opc/ua/object_ids.h \
  include/opc/ua/interface.h \
  include/opc/ua/attribute_ids.h \
  include/opc/ua/message_identifiers.h

lib_LIBRARIES = libopcuabinary.a


libopcuabinary_a_SOURCES = \
  src/binary_variant.cpp \
  src/binary_data_value.cpp \
  src/binary_endpoints.cpp \
  src/binary_messages.cpp \
  src/binary_serialization.h \
  src/binary_raw_size.cpp \
  src/binary_stream.cpp \
  src/binary_attribute.cpp \
  src/binary_view.cpp \
  src/binary_session.cpp \
  src/binary_secure_channel.cpp

libopcuabinary_a_CPPFLAGS = -I$(top_srcdir)/include 

TESTS = opc_ua_binary_test

check_PROGRAMS = $(TESTS)

opc_ua_binary_test_SOURCES =  \
 tests/binary_deserialize.cpp \
 tests/binary_serialize_attribute.cpp \
 tests/binary_serialize_data_value.cpp \
 tests/binary_serialize_endpoints.cpp \
 tests/binary_serialize_session.cpp \
 tests/binary_serialize_variant.cpp \
 tests/binary_serialize_view.cpp \
 tests/binary_serialize.cpp \
 tests/common.h

opc_ua_binary_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
opc_ua_binary_test_LDADD = libopcuabinary.a
opc_ua_binary_test_LDFLAGS = -lpthread -Wl,-z,defs $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) 

