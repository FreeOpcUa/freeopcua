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
 include/opc/ua/channel.h \
 include/opc/ua/binary/protocol/attribute.h \
 include/opc/ua/binary/protocol/common.h \
 include/opc/ua/binary/protocol/endpoints.h \
 include/opc/ua/binary/protocol/secure_channel.h \
 include/opc/ua/binary/protocol/session.h \
 include/opc/ua/binary/protocol/view.h \
 include/opc/ua/binary/remote_connection.h \
 include/opc/ua/binary/server.h \
 include/opc/ua/binary/types.h \
 include/opc/ua/binary/stream.h \
 include/opc/ua/binary/secure_channel.h \
 include/opc/ua/binary/datetime.h \
 include/opc/ua/interface.h \
 include/opc/ua/message_identifiers.h \
 include/opc/ua/session.h

lib_LIBRARIES = libopcuabinary.a


libopcuabinary_a_SOURCES = \
 src/socket_channel.h \
 src/binary_attribute.cpp \
 src/binary_data_value.cpp \
 src/binary_endpoints.cpp \
 src/binary_messages.cpp \
 src/binary_raw_size.cpp \
 src/socket_channel.cpp \
 src/binary_serialization.h \
 src/binary_server.cpp \
 src/binary_session.cpp \
 src/binary_stream.cpp \
 src/binary_variant.cpp \
 src/binary_view.cpp \
 src/thread.h \
 src/binary_secure_channel.cpp \
 src/binary_connection.cpp \
 src/thread.cpp

libopcuabinary_a_CPPFLAGS = -I$(top_srcdir)/include 

TESTS = opc_ua_binary_test

if BUILD_CLIENT
TESTS += opc_ua_client_test
endif

if BUILD_SERVER
TESTS += opc_ua_server_test
endif

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
 tests/common.h \
 tests/thread_test.cpp

opc_ua_binary_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
opc_ua_binary_test_LDADD = libopcuabinary.a
opc_ua_binary_test_LDFLAGS = -lpthread -Wl,-z,defs $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) 

if BUILD_SERVER
opc_ua_server_test_SOURCES =  \
 tests/binary_server_test_main.cpp \
 tests/binary_server_test.cpp

opc_ua_server_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
opc_ua_server_test_LDADD = libopcuabinary.a
opc_ua_server_test_LDFLAGS = -lpthread -Wl,-z,defs $(GTEST_LIB) $(GMOCK_LIBS) 
endif

if BUILD_CLIENT
opc_ua_client_test_SOURCES =  \
 tests/binary_handshake.cpp

opc_ua_client_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
opc_ua_client_test_LDADD = libopcuabinary.a
opc_ua_client_test_LDFLAGS = -lpthread -Wl,-z,defs $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) 
endif

