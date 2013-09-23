# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $


INCLUDES= -I$(top_srcdir) -Iinclude -I$(top_srcdir)/common/include $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/client/tests

configdir=$(sysconfdir)/opcua
config_DATA = configs/client.modules

opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua
uaclientincludedir = $(opcuaincludedir)/client

uaclientinclude_HEADERS = \
  include/opc/ua/client/binary_computer.h \
  include/opc/ua/client/remote_computer.h \
  include/opc/ua/client/remote_connection.h \
  include/opc/ua/protocol/binary/secure_connection.h

lib_LTLIBRARIES = libopcua_client.la
libopcua_client_la_SOURCES = \
  src/computer.cpp \
  src/binary_computer_addon.cpp \
  src/binary_computer.cpp \
  src/binary_connection.cpp \
  src/binary_secure_channel.cpp \
  src/endpoints.h \
  src/stream_attribute.h \
  src/stream_computer.h \
  src/stream_subscription.h \
  src/stream_view.h

libopcua_client_la_LDFLAGS =  $(URI_LIBS) $(UA_MAPPING_LIBS) $(OPC_CORE_LIBS) -ldl -lpthread

bin_PROGRAMS = opcua

opcua_SOURCES = \
  src/opcua_main.cpp \
  src/opcua_options.cpp \
  src/opcua_options.h

opcua_CPPFLAGS = $(INCLUDES) $(CLIENT_CONFIG_PATH)
opcua_LDFLAGS =  $(URI_LIBS) $(UA_MAPPING_LIBS) $(OPC_CORE_LIBS) -ldl -lboost_program_options -lpthread

#TESTS = libuaclient_test

#check_PROGRAMS = $(TESTS)

#libuaclient_test_SOURCES =  \
#  src/computer.cpp \
#  src/binary_connection.cpp \
#  tests/common.h \
#  tests/computer_attribute.cpp \
#  tests/computer_connect.cpp \
#  tests/computer_endpoints.cpp \
#  tests/computer_session.cpp \
#  tests/computer_view.cpp

#libuaclient_test_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
#libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(URI_LIBS) -no-undefined

