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

COMMON_INCLUDES = \
  -Iinclude \
  $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES) $(OPC_CLIENT_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)

lib_LTLIBRARIES = libopcua_tcp_server.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/tcp_server.h \
  include/opc/ua/server/tcp_server.h

libopcua_tcp_server_la_SOURCES = \
  src/tcp_server_factory.cpp \
  src/tcp_server_module.cpp \
  src/tcp_server.cpp

libopcua_tcp_server_la_CPPFLAGS = $(COMMON_INCLUDES)
libopcua_tcp_server_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS)

TESTS = test_tcp_server test_tcp_server_addon

TESTS_ENVIRONMENT = 

check_PROGRAMS = $(TESTS)

test_tcp_server_SOURCES = \
  src/tcp_server_factory.cpp \
  src/tcp_server_module.cpp \
  src/tcp_server.cpp \
  tests/tcp_server_test.cpp

test_tcp_server_CPPFLAGS = $(TESTINCLUDES) $(COMMON_INCLUDES)
test_tcp_server_LDFLAGS = -lboost_thread -lpthread -ldl $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) $(OPC_CLIENT_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,--no-undefined 

test_tcp_server_addon_SOURCES = \
  src/tcp_server_factory.cpp \
  src/tcp_server_module.cpp \
  src/tcp_server.cpp \
  tests/tcp_server_addon_test.cpp

test_tcp_server_addon_CPPFLAGS = $(TESTINCLUDES) $(COMMON_INCLUDES)
test_tcp_server_addon_LDFLAGS = -lboost_thread -lpthread -ldl $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) $(OPC_CLIENT_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,--no-undefined

