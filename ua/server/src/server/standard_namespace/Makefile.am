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
  -I$(top_srcdir)/src/common/include \
  -I$(top_srcdir)/src/server/address_space/include \
  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)


config_DATA = config/standard_address_space.xml

lib_LTLIBRARIES = libstandard_ns_addon.la
libstandard_ns_addon_la_SOURCES = \
   src/standard_namespace_addon.cpp \
   src/standard_namespace.cpp \
   src/standard_namespace.h
libstandard_ns_addon_la_CPPFLAGS = $(INCLUDES)
libstandard_ns_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined

TESTS = test_standard_address_space
check_PROGRAMS = $(TESTS)

test_standard_address_space_SOURCES=\
  tests/standard_namespace_ut.cpp

