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


lib_LTLIBRARIES = libopcua_server.la
libopcua_server_la_SOURCES = \
   src/opcuaserver.cpp

libopcua_server_la_CPPFLAGS = $(COMMON_INCLUDES)
libopcua_server_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS) -no-undefined

