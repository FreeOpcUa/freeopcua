# Author: Alexander Rykovanov 2012
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

opccoredir = $(includedir)/opccore

opcclientdir = $(opccoredir)/opc_client
opcclient_HEADERS = \
                  include/opccore/opc_client/errors.h \
                  include/opccore/opc_client/group.h \
                  include/opccore/opc_client/items_manager.h \
                  include/opccore/opc_client/server_browser.h \
                  include/opccore/opc_client/server.h \
                  include/opccore/opc_client/sync_io.h

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
                  include/opccore/common/addons_core/addon_ids.h \
                  include/opccore/common/addons_core/addon_manager.h \
                  include/opccore/common/addons_core/errors.h

managersdir = $(opccoredir)/managers

device_managerdir = $(managersdir)/device_manager
device_manager_HEADERS = \
                  include/opccore/managers/device_manager/errors.h \
                  include/opccore/managers/device_manager/manager.h

error_managerdir = $(managersdir)/error_manager
error_manager_HEADERS = \
                  include/opccore/managers/error_manager/manager.h

io_managerdir = $(managersdir)/io_manager
io_manager_HEADERS = \
                  include/opccore/managers/io_manager/errors.h \
                  include/opccore/managers/io_manager/manager.h



property_tree_managerdir = $(managersdir)/property_tree
property_tree_manager_HEADERS = \
                  include/opccore/managers/property_tree/manager.h \
                  include/opccore/managers/property_tree/errors.h \
                  include/opccore/managers/property_tree/group.h \
                  include/opccore/managers/property_tree/item.h \
                  include/opccore/managers/property_tree/named_object.h \
                  include/opccore/managers/property_tree/property_tree.h \
                  include/opccore/managers/property_tree/tree_object.h


lib_LIBRARIES = libopccore.a
libopccore_a_SOURCES = \
                  include/opccore/common/addons_core/addon.h \
                  include/opccore/common/addons_core/errors.h \
                  include/opccore/common/addons_core/addon_manager.h \
                  include/opccore/common/addons_core/addon_ids.h \
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
                  include/opccore/managers/io_manager/manager.h \
                  include/opccore/managers/property_tree/errors.h \
                  include/opccore/managers/property_tree/item.h \
                  include/opccore/managers/property_tree/group.h \
                  include/opccore/managers/property_tree/manager.h \
                  include/opccore/managers/property_tree/named_object.h \
                  include/opccore/managers/property_tree/property_tree.h \
                  include/opccore/managers/property_tree/tree_object.h \
                  include/opccore/opc_client/errors.h \
                  include/opccore/opc_client/group.h \
                  include/opccore/opc_client/items_manager.h \
                  include/opccore/opc_client/server.h \
                  include/opccore/opc_client/server_browser.h \
                  include/opccore/opc_client/sync_io.h \
                  src/common/object_id.cpp \
                  src/common/addons_core/errors_addon_manager.cpp \
                  src/common/addons_core/addon_manager.cpp \
                  src/common/value.cpp \
                  src/common/exception.cpp \
                  src/common/common_errors.cpp \
                  src/opc_client/sync_io.cpp \
                  src/opc_client/items_manager.cpp \
                  src/opc_client/server.cpp \
                  src/opc_client/server_browser.cpp \
                  src/opc_client/oc_errors.cpp \
                  src/opc_client/oc_group.cpp \
                  src/managers/device_manager/dm_errors.cpp \
                  src/managers/property_tree/pt_errors.cpp \
                  src/managers/property_tree/tree_object.cpp \
                  src/managers/property_tree/group.cpp \
                  src/managers/property_tree/property_tree.cpp \
                  src/managers/property_tree/manager/register.h \
                  src/managers/property_tree/manager/property_tree_manager.cpp \
                  src/managers/property_tree/item.cpp \
                  src/managers/io_manager/manager_impl.h \
                  src/managers/io_manager/register.h \
                  src/managers/io_manager/register.cpp \
                  src/managers/io_manager/manager_impl.cpp \
                  src/managers/io_manager/io_errors.cpp \
                  src/managers/common_managers/common_managers.cpp \
                  src/managers/common_managers/common_managers.h \
                  src/managers/error_manager/register.h \
                  src/managers/error_manager/error_manager.cpp

libopccore_a_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include

TESTS = common_test error_manager_test property_tree_manager_test managers_test opc_client_test

check_PROGRAMS = $(TESTS)

common_test_SOURCES = \
                  src/common/tests/addons_manager_test.cpp \
                  src/common/tests/error_test.cpp \
                  src/common/tests/main.cpp \
                  src/common/tests/value_test.cpp

common_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
common_test_LDADD = libopccore.a
common_test_LDFLAGS = -lcppunit


error_manager_test_SOURCES = \
                  src/managers/error_manager/tests/error_manager_test.cpp \
                  src/managers/error_manager/tests/main.cpp
error_manager_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
error_manager_test_LDADD = libopccore.a
error_manager_test_LDFLAGS = -lcppunit


property_tree_manager_test_SOURCES = \
                  src/managers/property_tree/tests/group_test.cpp \
                  src/managers/property_tree/tests/group_test.h \
                  src/managers/property_tree/tests/item_test.cpp \
                  src/managers/property_tree/tests/item_test.h \
                  src/managers/property_tree/tests/main.cpp \
                  src/managers/property_tree/tests/named_object_test.cpp \
                  src/managers/property_tree/tests/named_object_test.h \
                  src/managers/property_tree/tests/property_tree_manager.cpp \
                  src/managers/property_tree/tests/tree_object_test.cpp \
                  src/managers/property_tree/tests/tree_object_test.h

property_tree_manager_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
property_tree_manager_test_LDADD = libopccore.a
property_tree_manager_test_LDFLAGS = -lcppunit


managers_test_SOURCES = \
                  src/managers/tests/environment/device_manager.cpp \
                  src/managers/tests/environment/device_manager.h \
                  src/managers/tests/environment.cpp \
                  src/managers/tests/environment.h \
                  src/managers/tests/io_manager_tests.cpp \
                  src/managers/tests/main.cpp

managers_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
managers_test_LDADD = libopccore.a
managers_test_LDFLAGS = -lcppunit

opc_client_test_SOURCES = \
                  src/opc_client/tests/items_manager_test.cpp \
                  src/opc_client/tests/main.cpp \
                  src/opc_client/tests/opc_browse_test.cpp \
                  src/opc_client/tests/property_tree_builder.cpp \
                  src/opc_client/tests/property_tree_builder.h \
                  src/opc_client/tests/server_test.cpp \
                  src/opc_client/tests/sync_io_test.cpp

opc_client_test_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include
opc_client_test_LDADD = libopccore.a
opc_client_test_LDFLAGS = -lcppunit

