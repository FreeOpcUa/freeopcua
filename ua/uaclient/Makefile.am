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
uaclientincludedir = $(opcuaincludedir)/client


uaclientinclude_HEADERS = \
  include/opc/ua/client/computer.h \
  include/opc/ua/client/socket_channel.h \
  include/opc/ua/client/endpoints.h \
  include/opc/ua/client/remote_connection.h

lib_LIBRARIES = libuaclient.a


libuaclient_a_SOURCES = \
  src/endpoints.h \
  src/socket_channel.cpp \
  src/uri_facade.h \
  src/computer.cpp \
  src/binary_connection.cpp \
  src/stream_computer.h \
  src/stream_attribute.h

libuaclient_a_CPPFLAGS = -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(URI_INCLUDES)

if BUILD_UA_CMD

bin_PROGRAMS = opcua

opcua_SOURCES = \
  src/opcua_main.cpp \
  src/opcua_options.cpp

opcua_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES) $(UA_MAPPING_INCLUDES)
opcua_LDADD = libuaclient.a
opcua_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) -lboost_program_options -lpthread -Wl,-z,defs 

endif

TESTS_ENVIRONMENT = TEST_SERVER_URI=$(TEST_SERVER_URI)
TESTS = libuaclient_test

check_PROGRAMS = $(TESTS)

# tests/binary_handshake.cpp
# tests/thread_test.cpp
libuaclient_test_SOURCES =  \
 tests/computer_attribute.cpp \
 tests/computer_connect.cpp \
 tests/computer_endpoints.cpp \
 tests/computer_session.cpp \
 tests/computer_view.cpp

libuaclient_test_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES)
libuaclient_test_LDADD = libuaclient.a
libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(URI_LIBS)

