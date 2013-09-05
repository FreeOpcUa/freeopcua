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

uaclientinclude_HEADERS = \
  include/opc/ua/client/binary_computer.h \
  include/opc/ua/client/remote_computer.h \
  include/opc/ua/client/remote_connection.h

lib_LIBRARIES = libopcua_client.a
libopcua_client_a_SOURCES = \
  src/binary_computer.cpp \
  src/binary_connection.cpp \
  src/computer.cpp \
  src/endpoints.h \
  src/stream_attribute.h \
  src/stream_computer.h \
  src/stream_subscription.h \
  src/stream_view.h

bin_PROGRAMS = opcua

opcua_SOURCES = \
  src/opcua_main.cpp \
  src/opcua_options.cpp \
  src/opcua_options.h

INCLUDES= -I$(top_srcdir) -Iinclude -I$(top_srcdir)/common/include $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/client/tests

opcua_LDADD = libopcua_client.a
opcua_CPPFLAGS = $(INCLUDES)
opcua_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) -lboost_program_options -lpthread -Wl,-z,defs -no-undefined


lib_LTLIBRARIES = libpyopcua.la
libpyopcua_la_SOURCES = src/py_module.cpp
libpyopcua_la_CPPFLAGS = $(INCLUDES) -I/usr/include/python2.7
libpyopcua_la_LDFLAGS = libopcua_client.a -lboost_python


#TESTS = libuaclient_test

#check_PROGRAMS = $(TESTS)

#libuaclient_test_SOURCES =  \
  src/computer.cpp \
  src/binary_connection.cpp \
  tests/common.h \
  tests/computer_attribute.cpp \
  tests/computer_connect.cpp \
  tests/computer_endpoints.cpp \
  tests/computer_session.cpp \
  tests/computer_view.cpp

#libuaclient_test_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
#libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(URI_LIBS) -no-undefined

