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
  -I$(top_srcdir)/services_registry/include \
  $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)


lib_LTLIBRARIES = libendpoints_services_addon.la

addonsinclude_HEADERS = \
  include/opc/ua/server/addons/endpoints_services.h

libendpoints_services_addon_la_SOURCES = \
  src/endpoints_parameters.cpp \
  src/endpoints_parameters.h \
  src/endpoints_services_addon.cpp \
  src/endpoints_services_addon.h \
  src/endpoints_services_factory.h \
  src/endpoints_services_module.cpp
  
libendpoints_services_addon_la_CPPFLAGS = $(COMMON_INCLUDES)
libendpoints_services_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS)

##############################################################################
# Tests

TESTS = test_endpoints_registry
check_PROGRAMS = $(TESTS)

test_endpoints_registry_SOURCES = \
  src/endpoints_parameters.cpp \
  src/endpoints_services_addon.cpp \
  src/endpoints_services_addon.h \
  src/endpoints_services_factory.h \
  tests/endpoints_services_test.cpp \
  tests/endpoints_services_test.h \
  ../services_registry/src/services_registry_factory.cpp \
  ../services_registry/src/services_registry_factory.h \
  ../services_registry/src/services_registry_impl.cpp \
  ../services_registry/src/services_registry_impl.h
  
test_endpoints_registry_CPPFLAGS = $(COMMON_INCLUDES) $(TESTINCLUDES)
test_endpoints_registry_LDFLAGS = -lboost_thread -lpthread -ldl  $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,-z,defs,-no-undefined

