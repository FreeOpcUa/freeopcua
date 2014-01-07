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

COMMON_INCLUDES = \
  -Iinclude \
  -I$(top_srcdir)/address_space/include \
  -I$(top_srcdir)/endpoint_services/include \
  -I$(top_srcdir)/services_registry/include \
  -I$(top_srcdir)/tcp_server/include \
  $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES) $(OPC_CLIENT_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) \
  -Itests \
  -I$(top_srcdir)/builtin_server/include

lib_LTLIBRARIES = libopcua_protocol.la
addonsinclude_HEADERS = include/opc/ua/server/addons/opcua_protocol.h
libopcua_protocol_la_SOURCES = \
 ../endpoint_services/src/endpoints_parameters.cpp \
 src/opcua_protocol_addon.cpp \
 src/opcua_protocol_addon.h \
 src/opcua_protocol_factory.h \
 src/opcua_protocol_module.cpp \
 src/opc_tcp_processor.cpp \
 src/opc_tcp_processor.h
libopcua_protocol_la_CPPFLAGS = $(COMMON_INCLUDES)
libopcua_protocol_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS)

TESTS = test_opcua_protocol_addon

check_PROGRAMS = $(TESTS)

test_opcua_protocol_addon_SOURCES = \
  src/opcua_protocol_addon.cpp \
  src/opcua_protocol_addon.h \
  src/opcua_protocol_module.cpp \
  src/opc_tcp_processor.cpp \
  src/opc_tcp_processor.h \
  tests/opcua_protocol_addon_test.cpp \
  tests/opcua_protocol_addon_test.h \
  ../address_space/src/address_space_addon.cpp \
  ../address_space/src/address_space_internal.cpp \
  ../address_space/src/address_space_internal.h \
  ../builtin_server/src/builtin_server_factory.cpp \
  ../builtin_server/src/builtin_server_impl.cpp \
  ../builtin_server/src/builtin_server_impl.h \
  ../endpoint_services/src/endpoints_parameters.cpp \
  ../endpoint_services/src/endpoints_services_addon.cpp \
  ../endpoint_services/src/endpoints_services_addon.h \
  ../endpoint_services/src/endpoints_services_factory.h \
  ../services_registry/src/services_registry_factory.cpp \
  ../services_registry/src/services_registry_factory.h \
  ../services_registry/src/services_registry_impl.cpp \
  ../services_registry/src/services_registry_impl.h \
  ../standard_namespace/src/standard_namespace_addon.cpp \
  ../standard_namespace/src/standard_namespace.cpp

test_opcua_protocol_addon_CPPFLAGS = $(TESTINCLUDES) $(COMMON_INCLUDES)
test_opcua_protocol_addon_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) $(OPC_CLIENT_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS)

