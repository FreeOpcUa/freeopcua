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

INCLUDES = -Iinclude -I$(top_srcdir)/src/common/include -I$(top_srcdir)/src/server/services_registry/include $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/tests

lib_LTLIBRARIES = libaddress_space_addon.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/address_space_registry.h

libaddress_space_addon_la_SOURCES = \
   src/address_space_addon.cpp \
   src/address_space_internal.cpp \
   src/address_space_internal.h \
   src/address_space_module.cpp

libaddress_space_addon_la_CPPFLAGS = $(INCLUDES)
libaddress_space_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined

#check_PROGRAMS = $(TESTS)

