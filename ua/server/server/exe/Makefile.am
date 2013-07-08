# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $


INCLUDES= . $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) tests

bin_PROGRAMS = opcuaserver
opcuaserver_SOURCES = \
  src/server_main.cpp \
  src/server_options.cpp \
  src/server_options.h

opcuaserver_CPPFLAGS = $(INCLUDES)
opcuaserver_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) -no-undefined

configdir=$(sysconfdir)/opcua

config_DATA = configs/server.config

TESTS = test_opcuaserver

check_PROGRAMS=$(TESTS)

test_opcuaserver_SOURCES = \
  src/server_options.cpp \
  tests/common.cpp \
  tests/common.h \
  tests/test_server_options.cpp

test_opcuaserver_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
test_opcuaserver_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

