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
  -I$(top_srcdir)/src/server/services_registry/include \
  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)


lib_LTLIBRARIES = libendpoints_services_addon.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/endpoints_services.h
libendpoints_services_addon_la_SOURCES = \
  src/endpoints_services_addon.cpp
libendpoints_services_addon_la_CPPFLAGS = $(INCLUDES)
libendpoints_services_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) -no-undefined

##############################################################################
# Tests

check_PROGRAMS = $(TESTS)


