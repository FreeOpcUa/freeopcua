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

INCLUDES= \
  -Iinclude \
  -I$(top_srcdir)/common/include \
  -I$(top_srcdir)/client/include \
  -I$(top_srcdir)/server/server/include \
  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)

#config_DATA = $(top_srcdir)/configs/server.config

lib_LTLIBRARIES = libopcua_tcp_server.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/tcp_server_addon.h \
  include/opc/ua/server/tcp_server.h

libopcua_tcp_server_la_SOURCES = \
  src/tcp_server_addon_impl.cpp \
  src/tcp_server_addon_impl.h \
  src/tcp_server_factory.h \
  src/tcp_server_module.cpp \
  src/tcp_server.cpp

libopcua_tcp_server_la_CPPFLAGS = $(INCLUDES)
libopcua_tcp_server_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined
TESTS = test_tcp_server test_tcp_server_addon

TESTS_ENVIRONMENT = 

check_PROGRAMS = $(TESTS)

test_tcp_server_SOURCES = \
  src/tcp_server.cpp \
  src/tcp_server_addon_impl.cpp \
  src/tcp_server_addon_impl.h \
  tests/tcp_server_test.cpp

test_tcp_server_LDADD = $(top_builddir)/client/libopcua_client.a
test_tcp_server_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_tcp_server_LDFLAGS = -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

test_tcp_server_addon_SOURCES = \
  src/tcp_server.cpp \
  src/tcp_server_addon_impl.cpp \
  src/tcp_server_addon_impl.h \
  tests/tcp_server_addon_test.cpp

test_tcp_server_addon_LDADD = $(top_builddir)/client/libopcua_client.a
test_tcp_server_addon_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_tcp_server_addon_LDFLAGS = -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

