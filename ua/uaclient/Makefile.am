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

#opcuainclude_HEADERS = \
# include/opc/ua/channel.h

lib_LIBRARIES = libuaclient.a


libuaclient_a_SOURCES = 

libopcuabinary_a_CPPFLAGS = -I$(top_srcdir)/include 

if BUILD_UA_CMD

bin_PROGRAMS = uacmd

uacmd_SOURCES = src/main.cpp
uacmd_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
uacmd_LDADD = libuaclient.a
uacmd_LDFLAGS = -lpthread -Wl,-z,defs

endif

TESTS = libuaclient_test

check_PROGRAMS = $(TESTS)

libuaclient_test_SOURCES =  \
 tests/main.cpp

libuaclient_test_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
libuaclient_test_LDADD = libuaclient.a
libuaclient_test_LDFLAGS = -lpthread -Wl,-z,defs $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) 

