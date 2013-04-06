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
uaserverincludedir = $(opcuaincludedir)/server


opcuainclude_HEADERS = \
  include/opc/ua/attributes.h \
  include/opc/ua/computer.h \
  include/opc/ua/endpoints.h \
  include/opc/ua/socket_channel.h \
  include/opc/ua/view.h

uaclientinclude_HEADERS = \
  include/opc/ua/client/remote_computer.h \
  include/opc/ua/client/remote_connection.h


lib_LIBRARIES = libuaclient.a
libuaclient_a_SOURCES = \
  src/internal/uri_facade.h \
  src/internal/thread.h \
  src/endpoints.h \
  src/socket_channel.cpp \
  src/computer.cpp \
  src/binary_connection.cpp \
  src/stream_computer.h \
  src/stream_attribute.h

libuaclient_a_CPPFLAGS = -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(URI_INCLUDES)

TESTS = libuaclient_test


bin_PROGRAMS =

if BUILD_UA_CMD

bin_PROGRAMS += opcua

opcua_SOURCES = \
  src/opcua_main.cpp \
  src/opcua_options.cpp

opcua_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES) $(UA_MAPPING_INCLUDES)
opcua_LDADD = libuaclient.a
opcua_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) -lboost_program_options -lpthread -Wl,-z,defs 

endif

if BUILD_UA_SERVER
uaserverinclude_HEADERS = \
  include/opc/ua/server/server.h \
  include/opc/ua/server/tcp_server.h

lib_LIBRARIES += libopcuaserver.a

libopcuaserver_a_SOURCES = \
  src/server/server_options.h \
  src/server/server_options.cpp \
  src/server/tcp_server.cpp \
  src/socket_channel.cpp \
  src/thread.cpp


libopcuaserver_a_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(URI_INCLUDES)

bin_PROGRAMS += opcuaserver
opcuaserver_SOURCES = \
  src/server/server_main.cpp \
  src/server/server_options.cpp

opcuaserver_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
opcuaserver_LDADD = libopcuaserver.a
opcuaserver_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) -lboost_program_options -lboost_thread -lpthread -Wl,-z,defs 

TESTS += test_opcuaserver

endif


##############################################################################
# Tests

TESTS_ENVIRONMENT = TEST_SERVER_URI=$(TEST_SERVER_URI)
check_PROGRAMS = $(TESTS)

# tests/binary_handshake.cpp
libuaclient_test_SOURCES =  \
 tests/computer_attribute.cpp \
 tests/computer_connect.cpp \
 tests/computer_endpoints.cpp \
 tests/computer_session.cpp \
 tests/computer_view.cpp

libuaclient_test_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES) $(OPC_CORE_INCLUDES)
libuaclient_test_LDADD = libuaclient.a
libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS)

if BUILD_UA_SERVER


test_opcuaserver_SOURCES = \
 src/server/server_options.cpp \
 tests/server/binary_server_test.cpp \
 tests/server/test_server_options.cpp \
 tests/thread_test.cpp

test_opcuaserver_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES) $(UA_MAPPING_INCLUDES)
test_opcuaserver_LDADD = libopcuaserver.a libuaclient.a
test_opcuaserver_LDFLAGS = $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(UA_MAPPING_LIBS) $(URI_LIBS) -lboost_program_options -lboost_thread -lpthread -Wl,-z,defs 

#bin_PROGRAMS += libopcua_server_test


#libopcua_server_test_SOURCES =  \
#libopcua_server_test_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES)
#libopcua_server_test_LDADD = libopcuaserver.a
#libopcua_server_test_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) -lboost_program_options -lboost_thread -lpthread  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,-z,defs


endif
