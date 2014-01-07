# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $


COMMON_INCLUDES= $(UAMAPPINGS_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) tests

bin_PROGRAMS = opcuaserver
opcuaserver_SOURCES = \
  src/daemon.cpp \
  src/daemon.h \
  src/server_main.cpp \
  src/server_options.cpp \
  src/server_options.h

opcuaserver_CPPFLAGS = $(COMMON_INCLUDES)
opcuaserver_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) -Wl,--no-undefined,-rpath -Wl,LIBDIR

configdir=$(sysconfdir)/opcua
config_DATA = configs/server.config

#upstartdir=$(sysconfdir)/init
#upstart_DATA=configs/opcuaserver.conf

sysvdir=$(sysconfdir)/init.d
sysv_DATA=configs/opcuaserver

EXTRA_DIST = \
  configs/opcuaserver \
  configs/server.config \
  configs/opcuaserver.conf \
  tests/configs/endpoints.xml \
  tests/configs/test_addon.config \
  tests/configs/test_tcp_server.xml \
  tests/common.cpp \
  tests/common.h \
  tests/test_addon.cpp \
  tests/test_addon.h


#install-data-hook:
#	install -d $(localstatedir)/log/opcua
#	chmod 744 $(sysvdir)/opcuaserver
#	update-rc.d opcuaserver defaults || test true

#uninstall-hook:
#	update-rc.d opcuaserver remove || test true


TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(abs_top_builddir)/exe
TESTS=test_opcuaserver

check_PROGRAMS=$(TESTS)

test_opcuaserver_SOURCES = \
  src/server_options.cpp \
  tests/test_server_options.cpp

test_opcuaserver_CPPFLAGS = $(COMMON_INCLUDES) $(TESTINCLUDES)
test_opcuaserver_LDADD = test_addon.so
test_opcuaserver_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl  $(UAMAPPINGS_LIBS) $(OPC_CORE_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,--no-undefined

check : test_addon.so

test_addon.so: tests/test_addon.h tests/test_addon.cpp
	gcc -o0 -g -Wall -std=c++0x -fPIC -c $(COMMON_INCLUDES) tests/test_addon.cpp -o test_addon.o 
	gcc -o0 -g -Wall -std=c++0x -shared test_addon.o -o test_addon.so $(OPC_CORE_LIBS)

CLEANFILES = test_addon.o test_addon.so

