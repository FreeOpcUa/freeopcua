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
 include/opc/ua/client/computer.h

lib_LIBRARIES = libuaclient.a


libuaclient_a_SOURCES = \
  src/binary_connection.cpp \
  src/computer.cpp \
  src/socket_channel.cpp

libuaclient_a_CPPFLAGS = -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(URI_INCLUDES)

if BUILD_UA_CMD

bin_PROGRAMS = uacmd

uacmd_SOURCES = src/main.cpp
uacmd_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES) $(UA_MAPPING_INCLUDES)
uacmd_LDADD = libuaclient.a
uacmd_LDFLAGS = $(UA_MAPPING_LIBS) $(URI_LIBS) -lpthread -Wl,-z,defs 

endif

TESTS_ENVIRONMENT = TEST_SERVER_URI=$(TEST_SERVER_URI)
TESTS = libuaclient_test

check_PROGRAMS = $(TESTS)

# tests/binary_handshake.cpp
# tests/thread_test.cpp
libuaclient_test_SOURCES =  \
 tests/computer_connect.cpp

libuaclient_test_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(URI_INCLUDES)
libuaclient_test_LDADD = libuaclient.a
libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(URI_LIBS)

