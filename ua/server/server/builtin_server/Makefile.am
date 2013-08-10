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
  -I$(top_srcdir)/client/include \
  -I$(top_srcdir)/common/include \
  -I$(top_srcdir)/server/builin_server/include \
  -I$(top_srcdir)/server/tcp_server/include \
   $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/tests

lib_LTLIBRARIES = libbuiltin_server_addon.la
addonsinclude_HEADERS = \
  include/opc/ua/server/addons/builtin_server.h
libbuiltin_server_addon_la_SOURCES = \
  src/builtin_server_addon.cpp \
  src/builtin_server_factory.cpp \
  src/builtin_server_impl.cpp \
  src/builtin_server_impl.h

libibultin_server_addon_la_CPPFLAGS = $(INCLUDES)
libbuiltin_server_addon_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined

#TESTS = builtin_server_test

#check_PROGRAMS = $(TESTS)
