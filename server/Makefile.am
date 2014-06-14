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
  -I$(top_srcdir)/binary_protocol/include \
  -I$(top_srcdir)/endpoint_services/include \
  -I$(top_srcdir)/services_registry/include \
  -I$(top_srcdir)/tcp_server/include \
  -I$(top_srcdir)/server/include \
  $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)


opcuainclude_HEADERS = include/opc/ua/opcuaserver.h


lib_LTLIBRARIES = libopcua_server.la
libopcua_server_la_SOURCES = \
   include/opc/ua/server/opcuaserver.h \
   src/opcuaserver.cpp

libopcua_server_la_CPPFLAGS = $(COMMON_INCLUDES)
libopcua_server_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS)  $(UAMAPPINGS_LIBS) $(OPC_CLIENT_LIBS)\
  -L$(top_builddir)/standard_namespace/.libs -lstandard_ns_addon \
  -L$(top_builddir)/address_space/.libs -laddress_space_addon \
  -L$(top_builddir)/binary_protocol/.libs -lopcua_protocol \
  -L$(top_builddir)/endpoint_services/.libs -lendpoints_services_addon \
  -L$(top_builddir)/services_registry/.libs -lservices_registry_addon \
  -L$(top_builddir)/tcp_server/.libs -lopcua_tcp_server \
  -no-undefined
  

