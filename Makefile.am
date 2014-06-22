# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $


#######################################################
# list of binaries.
#######################################################


bin_PROGRAMS = opcuaserver opcua
lib_LTLIBRARIES = libopcuaprotocol.la libopcuacore.la libopcuaserver.la libopcuaclient.la

TESTS = test_opcuaserver test_opcuaprotocol common_gtest common_test  
TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(abs_top_builddir)

check_PROGRAMS = $(TESTS)

#######################################################
# Installation directories.
#######################################################

opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua
clientincludedir = $(opcuaincludedir)/client
serverincludedir = $(opcuaincludedir)/server
addonsincludedir = $(uaserverincludedir)/addons
protocolincludedir = $(opcuaincludedir)/protocol
binaryincludedir = $(protocolincludedir)/binary

configdir = ${sysconfdir}/opcua
serverconfigdir=${configdir}/server
serverconfig_DATA = src/server/configs/server.conf

clientconfigdir=${configdir}/client
clientconfig_DATA = src/client/configs/opc_tcp.conf

sysvdir=$(sysconfdir)/init.d
sysv_DATA=src/server/configs/opcuaserver

#upstartdir=$(sysconfdir)/init
#upstart_DATA=configs/opcuaserver.conf

pkgsharedir = $(libdir)/pkgconfig
pkgshare_DATA = libopcuaprotocol.pc libopcuacore.pc libopcuaserver.pc libopcuaclient.pc


pkgconfigdir = $(datarootdir)/pkgconfig
pkgconfig_DATA = libopcuaprotocol.pc libopcuacore.pc libopcuaserver.pc libopcuaclient.pc


#custom shared libraries required for testing
all-local : test_addon.so libtest_dynamic_addon.so tests/gtest/lib/.libs/libgtest.a tests/gtest/lib/.libs/libgmock.a

####################################################
# opcua server library
####################################################

serverinclude_HEADERS = \
	include/opc/ua/server/standard_namespace.h \
	include/opc/ua/server/opcua_protocol.h \
	include/opc/ua/server/address_space.h \
	include/opc/ua/server/opcuaserver.h \
	include/opc/ua/server/endpoints_services.h \
	include/opc/ua/server/tcp_server.h \
	include/opc/ua/server/services_registry.h \
	include/opc/ua/server/builtin_server.h \
	include/opc/ua/server/addons/standard_namespace.h \
	include/opc/ua/server/addons/opcua_protocol.h \
	include/opc/ua/server/addons/address_space.h \
	include/opc/ua/server/addons/xml_ns.h \
	include/opc/ua/server/addons/endpoints_services.h \
	include/opc/ua/server/addons/tcp_server.h \
	include/opc/ua/server/addons/services_registry.h \
	include/opc/ua/server/addons/builtin_server.h

libopcuaserver_la_SOURCES = \
	src/server/xml_address_space_loader.cpp \
	src/server/opcua_protocol_addon.cpp \
	src/server/services_registry_impl.cpp \
	src/server/endpoints_parameters.cpp \
	src/server/endpoints_services_addon.cpp \
	src/server/address_space_addon.h \
	src/server/tcp_server.cpp \
	src/server/xml_address_space_loader.h \
	src/server/standard_namespace_addon.cpp \
	src/server/tcp_server_factory.cpp \
	src/server/standard_namespace.cpp \
	src/server/endpoints_parameters.h \
	src/server/xml_address_space_addon.cpp \
	src/server/endpoints_registry.cpp \
	src/server/address_space_internal.cpp \
	src/server/server_options.cpp \
	src/server/server_options.h \
	src/server/services_registry_factory.cpp \
	src/server/opc_tcp_processor.cpp \
	src/server/address_space_addon.cpp \
	src/server/xml_processor.h \
	src/server/opcuaserver.cpp

libopcuaserver_la_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuaserver_la_LIBADD = libopcuaprotocol.la libopcuacore.la
libopcuaserver_la_LDFLAGS = -lpthread -ldl -lboost_thread -lboost_program_options -lboost_filesystem $(GCOV_LIBS) -lxml2 -Wl,--no-undefined


####################################################
# OPCUA server executable
####################################################

opcuaserver_SOURCES = \
	src/server/daemon.cpp \
	src/server/daemon.h \
	src/server/server_main.cpp

opcuaserver_LDADD = libopcuaserver.la libopcuaprotocol.la libopcuacore.la
opcuaserver_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2
opcuaserver_LDFLAGS = -ldl -lpthread -lboost_filesystem -lboost_system -Wl,--no-undefined 


test_addon.so: tests/server/test_addon.h tests/server/test_addon.cpp libopcuacore.la
	gcc -o0 -g -Wall -std=c++0x -fPIC -c tests/server/test_addon.cpp -I$(top_srcdir)/include -o test_addon.o
	gcc -o0 -g -Wall -std=c++0x -shared test_addon.o -o test_addon.so $(top_builddir)/.libs/libopcuacore.so

test_opcuaserver_SOURCES = \
	tests/server/address_space_registry_test.h \
	tests/server/builtin_server_factory.cpp \
	tests/server/builtin_server_impl.cpp \
	tests/server/builtin_server_impl.h \
	tests/server/builtin_server_test.h \
	tests/server/common.h \
	tests/server/endpoints_services_test.cpp \
	tests/server/endpoints_services_test.h \
	tests/server/opcua_protocol_addon_test.cpp \
	tests/server/opcua_protocol_addon_test.h \
	tests/server/services_registry_test.h \
	tests/server/standard_namespace_test.h \
	tests/server/standard_namespace_ut.cpp \
	tests/server/test_server_options.cpp \
	tests/server/xml_addressspace_ut.cpp \
	tests/server/xml_address_space_addon_ut.cpp \
	tests/server/tcp_server_addon_test.cpp \
	tests/server/tcp_server_addon_test.h \
	tests/server/tcp_server_test.cpp

test_opcuaserver_LDADD = libopcuaserver.la libopcuaprotocol.la libopcuacore.la libopcuaclient.la
test_opcuaserver_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 -I$(top_srcdir)/src/server $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
test_opcuaserver_LDFLAGS =  -ldl -lpthread $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(GCOV_LIBS)

####################################################
# Protocol library
####################################################

binaryinclude_HEADERS = \
  include/opc/ua/protocol/binary/stream.h \
  include/opc/ua/protocol/binary/common.h \
  include/opc/ua/protocol/binary/secure_connection.h

protocolinclude_HEADERS = \
  include/opc/ua/protocol/attribute.h \
  include/opc/ua/protocol/node_management.h \
  include/opc/ua/protocol/data_value.h \
  include/opc/ua/protocol/datetime.h \
  include/opc/ua/protocol/endpoints.h \
  include/opc/ua/protocol/guid.h \
  include/opc/ua/protocol/monitored_items.h \
  include/opc/ua/protocol/nodeid.h \
  include/opc/ua/protocol/secure_channel.h \
  include/opc/ua/protocol/session.h \
  include/opc/ua/protocol/subscriptions.h \
  include/opc/ua/protocol/types.h \
  include/opc/ua/protocol/variant.h \
  include/opc/ua/protocol/view.h \
  include/opc/ua/protocol/describe.h \
  include/opc/ua/protocol/channel.h \
  include/opc/ua/protocol/input_from_buffer.h \
  include/opc/ua/protocol/extension_identifiers.h \
  include/opc/ua/protocol/attribute_ids.h \
  include/opc/ua/protocol/message_identifiers.h \
  include/opc/ua/protocol/node_classes.h \
  include/opc/ua/protocol/object_ids.h \
  include/opc/ua/protocol/expanded_object_ids.h \
  include/opc/ua/protocol/reference_ids.h \
  include/opc/ua/protocol/strings.h \
  include/opc/ua/protocol/string_utils.h \
  include/opc/ua/protocol/status_codes.h \
  include/opc/ua/protocol/variable_access_level.h

libopcuaprotocol_la_SOURCES = \
  src/protocol/binary_variant.cpp \
  src/protocol/binary_stream.cpp \
  src/protocol/binary_serialization.h \
  src/protocol/binary_attribute.cpp \
  src/protocol/binary_node_management.cpp \
  src/protocol/binary_data_value.cpp \
  src/protocol/binary_endpoints.cpp \
  src/protocol/binary_messages.cpp \
  src/protocol/binary_raw_size.cpp \
  src/protocol/binary_session.cpp \
  src/protocol/binary_view.cpp \
  src/protocol/binary_secure_channel.cpp \
  src/protocol/input_from_buffer.cpp \
  src/protocol/monitored_items.cpp \
  src/protocol/nodeid.cpp \
  src/protocol/subscriptions.cpp \
  src/protocol/string_utils.cpp \
  src/protocol/types.cpp


libopcuaprotocol_la_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuaprotocol_la_LDFLAGS = $(GCOV_LIBS)

test_opcuaprotocol_SOURCES =  \
 tests/protocol/binary_deserialize.cpp \
 tests/protocol/binary_serialize_variant.cpp \
 tests/protocol/binary_serialize.cpp \
 tests/protocol/binary_serialize_attribute.cpp \
 tests/protocol/binary_serialize_data_value.cpp \
 tests/protocol/binary_serialize_endpoints.cpp \
 tests/protocol/binary_serialize_monitored_items.cpp \
 tests/protocol/binary_serialize_session.cpp \
 tests/protocol/binary_serialize_view.cpp \
 tests/protocol/binary_subscriptions.cpp \
 tests/protocol/binary_node_management.cpp \
 tests/protocol/common.h \
 tests/protocol/message_id.cpp \
 tests/protocol/node_id.cpp \
 tests/protocol/reference_id.cpp \
 tests/protocol/test_input_from_buffer.cpp \
 tests/protocol/utils.cpp

test_opcuaprotocol_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(GCOV_FLAGS)
test_opcuaprotocol_LDADD = libopcuaprotocol.la
test_opcuaprotocol_LDFLAGS = -ldl -lpthread -Wl,-z,defs $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GCOV_LIBS) -lpthread 


###########################################################
# OPCUA Common library
###########################################################

opcuainclude_HEADERS = \
  include/opc/ua/attributes.h \
  include/opc/ua/connection_listener.h \
  include/opc/ua/endpoints.h \
  include/opc/ua/errors.h \
  include/opc/ua/node.h \
  include/opc/ua/node_management.h \
  include/opc/ua/server.h \
  include/opc/ua/socket_channel.h \
  include/opc/ua/subscriptions.h \
  include/opc/ua/view.h

commondir = $(opcincludedir)/common
common_HEADERS = \
                  include/opc/common/application.h \
                  include/opc/common/class_pointers.h \
                  include/opc/common/errors.h \
                  include/opc/common/exception.h \
                  include/opc/common/interface.h \
                  include/opc/common/modules.h \
                  include/opc/common/object_id.h \
                  include/opc/common/thread.h \
                  include/opc/common/uri_facade.h \
                  include/opc/common/value.h

addonscoredir = $(commondir)/addons_core
addonscore_HEADERS = \
                  include/opc/common/addons_core/addon.h \
                  include/opc/common/addons_core/addon_manager.h \
                  include/opc/common/addons_core/addon_parameters.h \
                  include/opc/common/addons_core/config_file.h \
                  include/opc/common/addons_core/dynamic_addon.h \
                  include/opc/common/addons_core/dynamic_addon_factory.h \
                  include/opc/common/addons_core/errors.h

libopcuacore_la_SOURCES = \
                  src/core/common/application.cpp \
                  src/core/common/object_id.cpp \
                  src/core/common/thread.cpp \
                  src/core/common/addons_core/addon_manager.cpp \
                  src/core/common/addons_core/config_file.cpp \
                  src/core/common/addons_core/errors_addon_manager.cpp \
                  src/core/common/addons_core/dynamic_addon_factory.cpp \
                  src/core/common/addons_core/dynamic_library.cpp \
                  src/core/common/addons_core/dynamic_library.h \
                  src/core/common/value.cpp \
                  src/core/common/exception.cpp \
                  src/core/common/common_errors.cpp \
                  src/core/node.cpp \
                  src/core/opcua_errors.cpp \
                  src/core/socket_channel.cpp

libopcuacore_la_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuacore_la_LIBADD = libopcuaprotocol.la
libopcuacore_la_LDFLAGS = -lpthread -ldl -lboost_thread -lboost_filesystem -lboost_system $(GCOV_LIBS) -Wl,--no-undefined

test_dynamic_addon.o: tests/core/test_dynamic_addon.cpp
	$(CC) -std=c++0x -I$(top_srcdir)/include -fPIC -g -c -Wall tests/core/test_dynamic_addon.cpp 


libtest_dynamic_addon.so: test_dynamic_addon.o
	$(CC) -std=c++0x -g -Wall -ldl -shared -o libtest_dynamic_addon.so test_dynamic_addon.o


common_test_SOURCES = \
                  tests/core/common/error_test.cpp \
                  tests/core/common/main.cpp \
                  tests/core/common/value_test.cpp


common_test_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_test_LDADD = libopcuacore.la 
common_test_LDFLAGS = -ldl -lpthread -lcppunit

common_gtest_SOURCES = \
  tests/core/test_addon_manager.cpp \
  tests/core/test_config_file.cpp \
  tests/core/test_dynamic_addon.cpp \
  tests/core/test_dynamic_addon_factory.cpp \
  tests/core/test_dynamic_addon.h \
  tests/core/test_dynamic_addon_id.h \
  tests/core/test_uri.cpp \
  tests/core/common/thread_test.cpp

common_gtest_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_gtest_LDADD = libopcuacore.la
common_gtest_LDFLAGS = -ldl -lpthread -lxml2 $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIB) -no-undefined


#########################################################
# OPCUA client library
#########################################################

clientinclude_HEADERS = \
  include/opc/ua/client/addon.h \
  include/opc/ua/client/binary_server.h \
  include/opc/ua/client/client.h \
  include/opc/ua/client/remote_server.h \
  include/opc/ua/client/remote_connection.h

libopcuaclient_la_SOURCES = \
  src/client/computer.cpp \
  src/client/client.cpp \
  src/client/binary_computer_addon.cpp \
  src/client/binary_computer.cpp \
  src/client/binary_connection.cpp \
  src/client/endpoints.h \
  src/client/stream_attribute.h \
  src/client/stream_node_management.h \
  src/client/stream_server.h \
  src/client/stream_subscription.h \
  src/client/stream_view.h

libopcuaclient_la_CPPFLAGS =  -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuaclient_la_LIBADD = libopcuaprotocol.la libopcuacore.la
libopcuaclient_la_LDFLAGS = -ldl -lpthread -lxml2

#########################################################
# OPCUA commandline client
#########################################################

opcua_SOURCES = \
  src/client/opcua_main.cpp \
  src/client/opcua_options.cpp \
  src/client/opcua_options.h

opcua_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2
opcua_LDADD = libopcuaclient.la libopcuaprotocol.la libopcuacore.la
opcua_LDFLAGS =  -ldl -lboost_program_options -lpthread -lxml2

noinst_PROGRAMS = example_client
example_client_SOURCES = \
  src/client/client.cpp \
  src/client/example_main.cpp

example_client_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2
example_client_LDADD = libopcuaclient.la libopcuaprotocol.la libopcuacore.la
example_client_LDFLAGS = -ldl -lpthread -lxml2

#############################################################
# Extra configs and sources have to be in the distribution.
#############################################################

EXTRA_DIST = \
  src/server/configs/opcuaserver \
  src/server/configs/main.conf \
  src/server/configs/opcuaserver.conf \
  tests/client/configs \
  tests/core/configs \
  tests/gtest \
  tests/gmock \
  tests/server/common.cpp \
  tests/server/common.h \
  tests/server/test_addon.cpp \
  tests/server/test_addon.h \
  tests/server/base_node.xml \
  tests/server/configs/test_addon.conf \
  tests/server/empty.xml \
  tests/server/invalid_root.xml \
  tests/server/invalid_version.xml \
  tests/server/no_version.xml \
  tests/server/predefined_references.xml \
  debian make_deb.sh


#############################################################
# Debian packaging
#############################################################

dist-deb: dist
	VERSION=$(VERSION) $(top_srcdir)/make_deb.sh

#############################################################
# Clean additionaly compiled staff
#############################################################

CLEANFILES = \
  test_addon.o test_addon.so \
  libtest_dynamic_addon.so test_dynamic_addon.o test_config.xml \
  tests/configs/test.xml \
  test_config.config

clean-local:
	rm -rvf opcuaserver*$(VERSION)*
	rm -rvf opcuamappings*$(VERSION)* *.gcov *.gcno *.gcda
	rm -rvf libopcuacore*$(VERSION)*
	rm -rvf opcuaclient*$(VERSION)*
	$(MAKE) -C $(top_srcdir)/tests/gmock clean
	$(MAKE) -C $(top_srcdir)/tests/gtest clean


tests/gtest/lib/.libs/libgmock.a:
	$(MAKE) -C $(top_srcdir)/tests/gmock

tests/gtest/lib/.libs/libgtest.a:
	$(MAKE) -C $(top_srcdir)/tests/gtest
 
distclean-local:
	$(MAKE) -C $(top_srcdir)/tests/gmock distclean
	$(MAKE) -C $(top_srcdir)/tests/gtest distclean
 
