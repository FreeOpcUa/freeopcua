# Author: Alexander Rykovanov 2012
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua
clientincludedir = $(opcuaincludedir)/client

clientinclude_HEADERS = \
  include/opc/ua/subscriptions.h \
  include/opc/ua/view.h \
  include/opc/ua/connection_listener.h \
  include/opc/ua/computer.h \
  include/opc/ua/channel.h \
  include/opc/ua/attributes.h \
  include/opc/ua/endpoints.h

commondir = $(opcincludedir)/common
common_HEADERS = \
                  include/opc/common/class_pointers.h \
                  include/opc/common/errors.h \
                  include/opc/common/exception.h \
                  include/opc/common/interface.h \
                  include/opc/common/modules.h \
                  include/opc/common/object_id.h \
                  include/opc/common/thread.h \
                  include/opc/common/uri_facade.h \
                  include/opc/common/value.h \
                  include/opc/ua/socket_channel.h


addonsdir = $(commondir)/addons_core
addons_HEADERS = \
                  include/opc/common/addons_core/addon.h \
                  include/opc/common/addons_core/addon_manager.h \
                  include/opc/common/addons_core/addon_parameters.h \
                  include/opc/common/addons_core/dynamic_addon.h \
                  include/opc/common/addons_core/dynamic_addon_factory.h \
                  include/opc/common/addons_core/errors.h

lib_LTLIBRARIES = libopccore.la
libopccore_la_SOURCES = \
                  src/common/object_id.cpp \
                  src/common/thread.cpp \
                  src/common/addons_core/addon_manager.cpp \
                  src/common/addons_core/errors_addon_manager.cpp \
                  src/common/addons_core/dynamic_addon_factory.cpp \
                  src/common/addons_core/dynamic_addon_factory.h \
                  src/common/addons_core/dynamic_library.cpp \
                  src/common/addons_core/dynamic_library.h \
                  src/common/value.cpp \
                  src/common/exception.cpp \
                  src/common/common_errors.cpp \
                  src/socket_channel.cpp

libopccore_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I/usr/include/libxml2
libopccore_la_LDFLAGS = -ldl -lpthread -lxml2

test_dynamic_addon.o: tests/test_dynamic_addon.cpp
	$(CC) -std=c++0x -I$(top_srcdir)/src -I$(top_srcdir)/include -fPIC -g -c -Wall tests/test_dynamic_addon.cpp 


libtest_dynamic_addon.so: test_dynamic_addon.o
	$(CC) -std=c++0x -g -Wall -ldl -shared -o libtest_dynamic_addon.so test_dynamic_addon.o

all-local: libtest_dynamic_addon.so

CLEANFILES = libtest_dynamic_addon.so test_dynamic_addon.o

TESTS = common_gtest common_test  

check_PROGRAMS = $(TESTS)

common_test_SOURCES = \
                  tests/common/error_test.cpp \
                  tests/common/main.cpp \
                  tests/common/value_test.cpp


common_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_test_LDADD = libopccore.la
common_test_LDFLAGS = -lcppunit -ldl 

common_gtest_SOURCES = \
  tests/test_addon_manager.cpp \
  tests/test_dynamic_addon.cpp \
  tests/test_dynamic_addon_factory.cpp \
  tests/test_dynamic_addon.h \
  tests/test_dynamic_addon_id.h \
  tests/test_uri.cpp \
  tests/common/thread_test.cpp

common_gtest_CPPFLAGS =  -I/usr/include/libxml2 -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_gtest_LDADD = libopccore.la
common_gtest_LDFLAGS = -lxml2 -lpthread -ldl -L$(top_builddir) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIB) -no-undefined

