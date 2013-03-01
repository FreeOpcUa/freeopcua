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
 include/opc/ua/client/socket_channel.h

lib_LIBRARIES = libuaclient.a


libuaclient_a_SOURCES = \
  src/socket_channel.cpp \
  src/binary_connection.cpp

libuaclient_a_CPPFLAGS = -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES)

if BUILD_UA_CMD

bin_PROGRAMS = uacmd

uacmd_SOURCES = src/main.cpp
uacmd_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(UA_MAPPING_INCLUDES)
uacmd_LDADD = libuaclient.a
uacmd_LDFLAGS = $(UA_MAPPING_LIBS) -lpthread -Wl,-z,defs 

endif

TESTS = libuaclient_test

check_PROGRAMS = $(TESTS)

libuaclient_test_SOURCES =  \
 tests/binary_handshake.cpp \
 tests/thread_test.cpp

libuaclient_test_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(UA_MAPPING_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
libuaclient_test_LDADD = libuaclient.a
libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(UA_MAPPING_LIBS)  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) 

