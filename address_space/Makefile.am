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

AM_CPPFLAGS = -Iinclude -I$(top_srcdir)/services_registry/include $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/tests

lib_LTLIBRARIES = libaddress_space_addon.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/address_space_registry.h

libaddress_space_addon_la_SOURCES = \
   src/address_space_addon.cpp \
   src/address_space_addon.h \
   src/address_space_internal.cpp \
   src/address_space_internal.h \
   src/address_space_module.cpp \
   tests/address_space_registry_test.h

libaddress_space_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS)

#check_PROGRAMS = $(TESTS)

