# Author: Alexander Rykovanov 2012
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#


# TESTS += opc_client_test
#opcclientdir = $(opccoredir)/opc_client
#opcclient_HEADERS = 
#                  include/opccore/opc_client/errors.h \
#                  include/opccore/opc_client/group.h \
#                  include/opccore/opc_client/items_manager.h \
#                  include/opccore/opc_client/server_browser.h \
#                  include/opccore/opc_client/server.h \
#                  include/opccore/opc_client/sync_io.h
#                  include/opccore/opc_client/errors.h 
#                  include/opccore/opc_client/group.h 
#                  include/opccore/opc_client/items_manager.h 
#                  include/opccore/opc_client/server.h 
#                  include/opccore/opc_client/server_browser.h 
#                  include/opccore/opc_client/sync_io.h 
#                  src/opc_client/sync_io.cpp 
#                  src/opc_client/items_manager.cpp 
#                  src/opc_client/server.cpp 
#                  src/opc_client/server_browser.cpp 
#                  src/opc_client/oc_errors.cpp 
#                  src/opc_client/oc_group.cpp 
#opc_client_test_SOURCES = \
#                  src/opc_client/tests/items_manager_test.cpp \
#                  src/opc_client/tests/main.cpp \
#                  src/opc_client/tests/opc_browse_test.cpp \
#                  src/opc_client/tests/property_tree_builder.cpp \
#                  src/opc_client/tests/property_tree_builder.h \
#                  src/opc_client/tests/server_test.cpp \
#                  src/opc_client/tests/sync_io_test.cpp
#
#opc_client_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
#opc_client_test_LDADD = libopccore.a
#opc_client_test_LDFLAGS = -lcppunit

opccoredir = $(includedir)/opccore

commondir = $(opccoredir)/common
common_HEADERS = \
                  include/opccore/common/class_pointers.h \
                  include/opccore/common/errors.h \
                  include/opccore/common/exception.h \
                  include/opccore/common/modules.h \
                  include/opccore/common/noncopyable.h \
                  include/opccore/common/object_id.h \
                  include/opccore/common/value.h


addonsdir = $(commondir)/addons_core
addons_HEADERS = \
                  include/opccore/common/addons_core/addon.h \
                  include/opccore/common/addons_core/addon_manager.h \
                  include/opccore/common/addons_core/addon_parameters.h \
                  include/opccore/common/addons_core/dynamic_addon.h \
                  include/opccore/common/addons_core/dynamic_addon_factory.h \
                  include/opccore/common/addons_core/errors.h

managersdir = $(opccoredir)/managers

device_managerdir = $(managersdir)/device_manager
device_manager_HEADERS = \
                  include/opccore/managers/device_manager/errors.h \
                  include/opccore/managers/device_manager/id.h \
                  include/opccore/managers/device_manager/manager.h

error_managerdir = $(managersdir)/error_manager
error_manager_HEADERS = \
                  include/opccore/managers/error_manager/factory.h \
                  include/opccore/managers/error_manager/id.h \
                  include/opccore/managers/error_manager/manager.h

io_managerdir = $(managersdir)/io_manager
io_manager_HEADERS = \
                  include/opccore/managers/io_manager/errors.h \
                  include/opccore/managers/io_manager/factory.h \
                  include/opccore/managers/io_manager/id.h \
                  include/opccore/managers/io_manager/manager.h



property_tree_managerdir = $(managersdir)/property_tree
property_tree_manager_HEADERS = \
                  include/opccore/managers/property_tree/errors.h \
                  include/opccore/managers/property_tree/factory.h \
                  include/opccore/managers/property_tree/group.h \
                  include/opccore/managers/property_tree/id.h \
                  include/opccore/managers/property_tree/item.h \
                  include/opccore/managers/property_tree/manager.h \
                  include/opccore/managers/property_tree/named_object.h \
                  include/opccore/managers/property_tree/property_tree.h \
                  include/opccore/managers/property_tree/tree_object.h

lib_LIBRARIES = libopccore.a
libopccore_a_SOURCES = \
                  include/opccore/common/addons_core/addon.h \
                  include/opccore/common/addons_core/errors.h \
                  include/opccore/common/addons_core/addon_manager.h \
                  include/opccore/common/addons_core/dynamic_addon.h \
                  include/opccore/common/class_pointers.h \
                  include/opccore/common/errors.h \
                  include/opccore/common/exception.h \
                  include/opccore/common/modules.h \
                  include/opccore/common/noncopyable.h \
                  include/opccore/common/object_id.h \
                  include/opccore/common/value.h \
                  include/opccore/managers/device_manager/errors.h \
                  include/opccore/managers/device_manager/manager.h \
                  include/opccore/managers/error_manager/manager.h \
                  include/opccore/managers/io_manager/errors.h \
                  include/opccore/managers/io_manager/factory.h \
                  include/opccore/managers/io_manager/id.h \
                  include/opccore/managers/io_manager/manager.h \
                  include/opccore/managers/property_tree/errors.h \
                  include/opccore/managers/property_tree/item.h \
                  include/opccore/managers/property_tree/group.h \
                  include/opccore/managers/property_tree/factory.h \
                  include/opccore/managers/property_tree/id.h \
                  include/opccore/managers/property_tree/manager.h \
                  include/opccore/managers/property_tree/named_object.h \
                  include/opccore/managers/property_tree/property_tree.h \
                  include/opccore/managers/property_tree/tree_object.h \
                  src/common/object_id.cpp \
                  src/common/addons_core/addon_manager.cpp \
                  src/common/addons_core/errors_addon_manager.cpp \
                  src/common/addons_core/dynamic_addon_factory.cpp \
                  src/common/addons_core/dynamic_addon_factory.h \
                  src/common/addons_core/dynamic_library.cpp \
                  src/common/addons_core/dynamic_library.h \
                  src/common/value.cpp \
                  src/common/exception.cpp \
                  src/common/common_errors.cpp \
                  src/managers/device_manager/dm_errors.cpp \
                  src/managers/property_tree/pt_errors.cpp \
                  src/managers/property_tree/tree_object.cpp \
                  src/managers/property_tree/group.cpp \
                  src/managers/property_tree/property_tree.cpp \
                  src/managers/property_tree/manager/property_tree_manager.cpp \
                  src/managers/property_tree/item.cpp \
                  src/managers/io_manager/factory.cpp \
                  src/managers/io_manager/io_errors.cpp \
                  src/managers/io_manager/manager_impl.h \
                  src/managers/io_manager/manager_impl.cpp \
                  src/managers/error_manager/error_manager.cpp

libopccore_a_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include

test_dynamic_addon.o: tests/test_dynamic_addon.cpp
	$(CC) -std=c++0x -I$(top_srcdir)/src -I$(top_srcdir)/include -fPIC -g -c -Wall tests/test_dynamic_addon.cpp 


libtest_dynamic_addon.so: test_dynamic_addon.o
	$(CC) -std=c++0x -g -Wall -ldl -shared -module -o libtest_dynamic_addon.so test_dynamic_addon.o

all-local: libtest_dynamic_addon.so

CLEANFILES = libtest_dynamic_addon.so test_dynamic_addon.o

TESTS = common_gtest common_test error_manager_test property_tree_manager_test managers_test 

check_PROGRAMS = $(TESTS)

common_test_SOURCES = \
                  tests/common/addons_manager_test.cpp \
                  tests/common/error_test.cpp \
                  tests/common/main.cpp \
                  tests/common/value_test.cpp

common_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_test_LDADD = libopccore.a
common_test_LDFLAGS = -lcppunit -ldl

common_gtest_SOURCES = \
  tests/test_addon_manager.cpp \
  tests/test_dynamic_addon.cpp \
  tests/test_dynamic_addon_factory.cpp \
  tests/test_dynamic_addon.h \
  tests/test_dynamic_addon_id.h

common_gtest_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_gtest_LDFLAGS = -lpthread -ldl -L$(top_builddir) -Wl,--whole-archive,-lopccore,--no-whole-archive  $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined


error_manager_test_SOURCES = \
                  tests/error_manager/error_manager_test.cpp \
                  tests/error_manager/main.cpp

error_manager_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
error_manager_test_LDADD = libopccore.a
error_manager_test_LDFLAGS = -lcppunit -ldl


property_tree_manager_test_SOURCES = \
                  tests/property_tree/group_test.cpp \
                  tests/property_tree/group_test.h \
                  tests/property_tree/item_test.cpp \
                  tests/property_tree/item_test.h \
                  tests/property_tree/main.cpp \
                  tests/property_tree/named_object_test.cpp \
                  tests/property_tree/named_object_test.h \
                  tests/property_tree/property_tree_manager.cpp \
                  tests/property_tree/tree_object_test.cpp \
                  tests/property_tree/tree_object_test.h

property_tree_manager_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
property_tree_manager_test_LDADD = libopccore.a
property_tree_manager_test_LDFLAGS = -lcppunit -ldl


#                  src/managers/tests/environment.cpp
#                  src/managers/tests/environment.h

managers_test_SOURCES = \
                  tests/managers/io_manager_tests.cpp \
                  tests/managers/main.cpp \
                  tests/managers/environment/device_manager.h \
                  tests/managers/environment/device_manager.cpp

managers_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
managers_test_LDADD = libopccore.a
managers_test_LDFLAGS = -lcppunit -ldl


