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
serverconfigdir=$(configdir)/server

COMMON_INCLUDES = \
  -I$(top_srcdir)/standard_namespace/include \
  -I$(top_srcdir)/address_space/include \
  -I$(top_srcdir)/services_registry/include \
  $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)


serverconfig_DATA = config/standard_address_space.xml

lib_LTLIBRARIES = libstandard_ns_addon.la
libstandard_ns_addon_la_SOURCES = \
   src/standard_namespace.cpp \
   src/standard_namespace.h \
   src/standard_namespace_addon.cpp \
   src/standard_namespace_module.cpp

EXTRA_DIST = config/standard_address_space.xml
 
libstandard_ns_addon_la_CPPFLAGS = $(COMMON_INCLUDES)
libstandard_ns_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS) -no-undefined

TESTS = test_standard_address_space
check_PROGRAMS = $(TESTS)

test_standard_address_space_CPPFLAGS = $(TESTINCLUDES)  $(COMMON_INCLUDES)
test_standard_address_space_LDFLAGS = -lboost_thread -lpthread -ldl -Wl,-z,defs $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined
test_standard_address_space_SOURCES=\
  ../address_space/src/address_space_addon.cpp \
  ../address_space/src/address_space_internal.cpp \
  src/standard_namespace.cpp \
  src/standard_namespace_addon.cpp \
  tests/standard_namespace_ut.cpp \
  tests/standard_namespace_test.h

