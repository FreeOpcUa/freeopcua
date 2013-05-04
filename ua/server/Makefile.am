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
addonsincludedir = $(uaserverincludedir)/addons

configdir = $(sysconfdir)/opcua

opcuainclude_HEADERS = \
  include/opc/ua/attributes.h \
  include/opc/ua/computer.h \
  include/opc/ua/endpoints.h \
  include/opc/ua/socket_channel.h \
  include/opc/ua/view.h

uaclientinclude_HEADERS = \
  include/opc/ua/client/binary_computer.h \
  include/opc/ua/client/remote_computer.h \
  include/opc/ua/client/remote_connection.h

EXTRA_DIST = \
  src/internal/uri_facade.h \
  src/internal/thread.h \
  src/endpoints.h \
  src/stream_computer.h \
  src/stream_attribute.h \
  tests/configs/endpoints.xml \
  tests/configs/test.xml \
  tests/configs/test_tcp_server.xml


TESTS = libuaclient_test

bin_PROGRAMS =
lib_LTLIBRARIES =

if BUILD_UA_CMD

bin_PROGRAMS += opcua

opcua_SOURCES = \
  src/binary_computer.cpp \
  src/binary_connection.cpp \
  src/computer.cpp \
  src/endpoints.h \
  src/opcua_main.cpp \
  src/opcua_options.cpp \
  src/opcua_options.h \
  src/socket_channel.cpp \
  src/stream_attribute.h \
  src/stream_computer.h \
  src/stream_view.h

#src/thread.cpp


INCLUDES= -I$(top_srcdir)/src -I$(top_srcdir)/include  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/tests

opcua_CPPFLAGS = $(INCLUDES)
opcua_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) -lboost_program_options -lpthread -Wl,-z,defs -no-undefined

endif

if BUILD_UA_SERVER
uaserverinclude_HEADERS = \
  include/opc/ua/server/server.h \
  include/opc/ua/server/tcp_server.h

bin_PROGRAMS += opcuaserver
opcuaserver_SOURCES = \
  src/server/server_main.cpp \
  src/server/server_options.cpp \
  src/server/server_options.h \
  src/socket_channel.cpp \
  src/thread.cpp

opcuaserver_CPPFLAGS = $(INCLUDES)
opcuaserver_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) -no-undefined
TESTS += test_opcuaserver

config_DATA = $(top_srcdir)/configs/server.config

lib_LTLIBRARIES += libopcua_tcp_server.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/tcp_server_addon.h
libopcua_tcp_server_la_SOURCES = \
  src/server/tcp_server.cpp \
  src/server/tcp_server_addon.cpp \
  src/socket_channel.cpp \
  src/thread.cpp
libopcua_tcp_server_la_CPPFLAGS = $(INCLUDES)
libopcua_tcp_server_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) -no-undefined
TESTS += test_tcp_server
TESTS += test_tcp_server_addon
TESTS += server_unittests


lib_LTLIBRARIES += libopcua_protocol.la
addonsinclude_HEADERS += include/opc/ua/server/addons/opcua_protocol.h
libopcua_protocol_la_SOURCES = \
 src/server/opcua_protocol_addon.cpp \
 src/server/opc_tcp_processor.cpp \
 src/server/opc_tcp_processor.h
libopcua_protocol_la_CPPFLAGS = $(INCLUDES)
libopcua_protocol_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(URI_LIBS) -no-undefined
TESTS += test_opcua_protocol_addon

lib_LTLIBRARIES += libbuiltin_computer_addon.la
addonsinclude_HEADERS += \
  include/opc/ua/server/addons/builtin_computer.h
libbuiltin_computer_addon_la_SOURCES = \
  src/binary_computer.cpp \
  src/server/builtin_computer_addon.cpp \
  src/thread.cpp
libibultin_computer_addon_la_CPPFLAGS = $(INCLUDES)
libbuiltin_computer_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined

lib_LTLIBRARIES += libinternal_computer_addon.la
addonsinclude_HEADERS += \
  include/opc/ua/server/addons/internal_computer.h
libinternal_computer_addon_la_SOURCES = \
  src/server/internal_computer.cpp
libinternal_computer_addon_la_CPPFLAGS = $(INCLUDES)
libinternal_computer_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) -no-undefined

lib_LTLIBRARIES += libendpoints_services_addon.la
addonsinclude_HEADERS += \
  include/opc/ua/server/addons/endpoints_services.h
libendpoints_services_addon_la_SOURCES = \
  src/server/endpoints_services_addon.cpp
libendpoints_services_addon_la_CPPFLAGS = $(INCLUDES)
libendpoints_services_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) -no-undefined

lib_LTLIBRARIES += libview_services_addon.la
addonsinclude_HEADERS += \
  include/opc/ua/server/addons/view_services.h
libview_services_addon_la_SOURCES = \
  src/server/standard_namespace.cpp \
  src/server/view_services_addon.cpp
libview_services_addon_la_CPPFLAGS = $(INCLUDES)
libview_services_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined


endif


##############################################################################
# Tests

TESTS_ENVIRONMENT = \
  TEST_SERVER_URI="$(TEST_SERVER_URI)" \
  BUILTIN_COMPUTER_ADDON_PATH="${top_builddir}/.libs/libbuiltin_computer_addon.so" \
  TCP_ADDON_PATH="${top_builddir}/.libs/libopcua_tcp_server.so" \
  OPCUA_PROTOCOL_ADDON_PATH=$(top_builddir)/.libs/libopcua_protocol.so \
  ENDPOINTS_CONFIG_PATH=$(top_builddir)/tests/configs/endpoints.xml

check_PROGRAMS = $(TESTS)

# tests/binary_handshake.cpp
libuaclient_test_SOURCES =  \
  src/socket_channel.cpp \
  src/computer.cpp \
  src/binary_connection.cpp \
  tests/common.h \
  tests/computer_attribute.cpp \
  tests/computer_connect.cpp \
  tests/computer_endpoints.cpp \
  tests/computer_session.cpp \
  tests/computer_view.cpp

libuaclient_test_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(URI_LIBS) -no-undefined

if BUILD_UA_SERVER

test_opcuaserver_SOURCES = \
  src/server/server_options.cpp \
  src/thread.cpp \
  tests/server/common.cpp \
  tests/server/common.h \
  tests/server/test_server_options.cpp \
  tests/thread_test.cpp

test_opcuaserver_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_opcuaserver_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

server_unittests_SOURCES = \
  src/server/standard_namespace.cpp \
  tests/server/standard_namespace_ut.cpp

server_unittests_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
server_unittests_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(URI_LIBS) $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

test_tcp_server_SOURCES = \
  src/socket_channel.cpp \
  src/computer.cpp \
  src/binary_connection.cpp \
  src/server/tcp_server.cpp \
  src/thread.cpp \
  tests/server/tcp_server_test.cpp

test_tcp_server_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_tcp_server_LDFLAGS = -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

test_tcp_server_addon_SOURCES = \
  src/binary_connection.cpp \
  src/socket_channel.cpp \
  tests/server/tcp_server_addon_test.cpp
test_tcp_server_addon_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_tcp_server_addon_LDFLAGS = -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

test_opcua_protocol_addon_SOURCES = \
  src/server/server_options.cpp \
  tests/server/opcua_protocol_addon_test.cpp \
  tests/server/common.cpp
test_opcua_protocol_addon_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_opcua_protocol_addon_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

endif

