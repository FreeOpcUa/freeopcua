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
uaserverincludedir = $(opcuaincludedir)/server
addonsincludedir = $(uaserverincludedir)/addons

configdir = $(sysconfdir)/opcua

INCLUDES= \
  -Iinclude \
  -I$(top_srcdir)/common/include \
  -I$(top_srcdir)/server/endpoint_services/include \
  -I$(top_srcdir)/server/services_registry/include \
  -I$(top_srcdir)/server/tcp_server/include \
  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) -Itests

lib_LTLIBRARIES = libopcua_protocol.la
addonsinclude_HEADERS = include/opc/ua/server/addons/opcua_protocol.h
libopcua_protocol_la_SOURCES = \
 src/opcua_protocol_addon.cpp \
 src/opc_tcp_processor.cpp \
 src/opc_tcp_processor.h
libopcua_protocol_la_CPPFLAGS = $(INCLUDES)
libopcua_protocol_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(URI_LIBS) -no-undefined

TESTS = test_opcua_protocol_addon

check_PROGRAMS = $(TESTS)

test_opcua_protocol_addon_SOURCES = \
  src/server/server_options.cpp \
  tests/server/opcua_protocol_addon_test.cpp \
  tests/server/common.cpp
test_opcua_protocol_addon_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_opcua_protocol_addon_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

