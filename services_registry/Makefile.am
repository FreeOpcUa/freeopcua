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
  -I$(top_srcdir)/common/include \
  $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)

lib_LTLIBRARIES = libservices_registry_addon.la

addonsinclude_HEADERS = \
  include/opc/ua/server/addons/services_registry.h

libservices_registry_addon_la_SOURCES = \
  src/services_registry_factory.cpp \
  src/services_registry_factory.h \
  src/services_registry_impl.cpp \
  src/services_registry_impl.h \
  src/services_registry_module.cpp \
  tests/services_registry_test.h

libservices_registry_addon_la_CPPFLAGS = $(COMMON_INCLUDES)
libservices_registry_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined

#TESTS = test_services_registry

#check_PROGRAMS = $(TESTS)

#test_services_registry_SOURCES =
#test_services_registry_CPPFLAGS = $(TESTINCLUDES) $(COMMON_INCLUDES)
#test_services_registry_LDFLAGS = -lboost_thread -lpthread -ldl -Wl,-z,defs $(UA_MAPPING_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

