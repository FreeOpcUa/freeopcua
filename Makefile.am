# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

if ENABLE_PYTHON
SUBDIRS = . python
endif

#######################################################
# list of binaries.
#######################################################

AUTOMAKE_OPTIONS = subdir-objects


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
servicesincludedir = $(opcuaincludedir)/services
serverincludedir = $(opcuaincludedir)/server
addonsincludedir = $(serverincludedir)/addons
protocolincludedir = $(opcuaincludedir)/protocol
binaryincludedir = $(protocolincludedir)/binary

configdir = ${sysconfdir}/opcua
serverconfigdir=${configdir}/server
serverconfig_DATA = src/serverapp/configs/server.conf

clientconfigdir=${configdir}/client
clientconfig_DATA = src/clientapp/configs/opc_tcp.conf

sysvdir=$(sysconfdir)/init.d
sysv_DATA=src/serverapp/configs/opcuaserver

#upstartdir=$(sysconfdir)/init
#upstart_DATA=configs/opcuaserver.conf

pkgsharedir = $(libdir)/pkgconfig
pkgshare_DATA = libopcuaprotocol.pc libopcuacore.pc libopcuaserver.pc libopcuaclient.pc


pkgconfigdir = $(datarootdir)/pkgconfig
pkgconfig_DATA = libopcuaprotocol.pc libopcuacore.pc libopcuaserver.pc libopcuaclient.pc


#custom shared libraries required for testing
all-local : libtest_dynamic_addon.so tests/gtest/lib/.libs/libgtest.a tests/gtest/lib/.libs/libgmock.a


DYNAMIC_ADDON_PATH = $(top_builddir)/libtest_dynamic_addon.so
TEST_CORE_CONFIG_PATH = $(top_srcdir)/tests/core/configs/


####################################################
# opcua server library
####################################################


serverinclude_HEADERS = \
	include/opc/ua/server/address_space.h \
	include/opc/ua/server/endpoints_services.h \
	include/opc/ua/server/opc_tcp_async.h \
	include/opc/ua/server/server.h \
	include/opc/ua/server/services_registry.h \
  include/opc/ua/server/standard_address_space.h \
  include/opc/ua/server/subscription_service.h

addonsinclude_HEADERS = \
	include/opc/ua/server/addons/asio_addon.h \
	include/opc/ua/server/addons/address_space.h \
	include/opc/ua/server/addons/endpoints_services.h \
	include/opc/ua/server/addons/opc_tcp_async.h \
	include/opc/ua/server/addons/opcua_protocol.h \
	include/opc/ua/server/addons/services_registry.h \
	include/opc/ua/server/addons/standard_address_space.h \
	include/opc/ua/server/addons/subscription_service.h \
	include/opc/ua/server/addons/common_addons.h \
	include/opc/ua/server/addons/xml_ns.h

libopcuaserver_la_SOURCES = \
	src/server/asio_addon.cpp \
	src/server/address_space_addon.cpp \
	src/server/address_space_addon.h \
	src/server/address_space_internal.cpp \
	src/server/address_space_internal.h \
	src/server/common_addons.cpp \
	src/server/endpoints_parameters.cpp \
	src/server/endpoints_parameters.h \
	src/server/endpoints_services_addon.cpp \
	src/server/endpoints_registry.cpp \
	src/server/internal_subscription.h \
	src/server/internal_subscription.cpp \
	src/server/opc_tcp_async_addon.cpp \
	src/server/opc_tcp_async.cpp \
	src/server/opc_tcp_async_parameters.cpp \
	src/server/opc_tcp_async_parameters.h \
	src/server/opc_tcp_processor.cpp \
	src/server/opc_tcp_processor.h \
	src/server/opcua_protocol.h \
	src/server/opcua_protocol_addon.cpp \
	src/server/server.cpp \
	src/server/server_object.cpp \
	src/server/server_object.h \
	src/server/server_object_addon.cpp \
	src/server/server_object_addon.h \
	src/server/services_registry_impl.cpp \
	src/server/services_registry_factory.cpp \
	src/server/subscription_service_addon.cpp \
	src/server/subscription_service_internal.h \
	src/server/subscription_service_internal.cpp \
	src/server/standard_address_space_addon.cpp \
	src/server/standard_address_space.cpp \
	src/server/standard_address_space_parts.h \
	src/server/standard_address_space_part3.cpp \
	src/server/standard_address_space_part4.cpp \
	src/server/standard_address_space_part5.cpp \
	src/server/standard_address_space_part8.cpp \
	src/server/standard_address_space_part9.cpp \
	src/server/standard_address_space_part10.cpp \
	src/server/standard_address_space_part11.cpp \
	src/server/standard_address_space_part13.cpp \
	src/server/tcp_server.cpp \
	src/server/tcp_server.h \
	src/server/timer.h \
	src/server/xml_address_space_loader.cpp \
	src/server/xml_address_space_loader.h \
	src/server/xml_address_space_addon.cpp \
	src/server/xml_processor.h

libopcuaserver_la_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuaserver_la_LIBADD = libopcuaprotocol.la libopcuacore.la
libopcuaserver_la_LDFLAGS = -lpthread -ldl -lboost_thread -lboost_system -lboost_filesystem $(GCOV_LIBS) -lxml2 -Wl,--no-undefined


####################################################
# OPCUA server executable
####################################################

opcuaserver_SOURCES = \
	src/serverapp/daemon.cpp \
	src/serverapp/daemon_lin.cpp \
	src/serverapp/daemon.h \
	src/serverapp/server_main.cpp \
	src/serverapp/server_options.cpp \
	src/serverapp/server_options.h

opcuaserver_LDADD = libopcuaserver.la libopcuaprotocol.la libopcuacore.la
opcuaserver_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 -DCONFIG_PATH=\"$(sysconfdir)/opcua/server\"
opcuaserver_LDFLAGS = -ldl -lpthread -lboost_filesystem -lboost_program_options -lboost_system -Wl,--no-undefined


test_opcuaserver_SOURCES = \
	tests/server/address_space_registry_test.h \
	tests/server/address_space_ut.cpp \
	tests/server/builtin_server.h \
	tests/server/builtin_server_addon.h \
	tests/server/builtin_server_factory.cpp \
	tests/server/builtin_server_impl.cpp \
	tests/server/builtin_server_impl.h \
	tests/server/builtin_server_test.h \
	tests/server/common.h \
	tests/server/endpoints_services_test.cpp \
	tests/server/endpoints_services_test.h \
	tests/server/model_object_ut.cpp \
	tests/server/model_object_type_ut.cpp \
	tests/server/model_variable_ut.cpp \
	tests/server/opcua_protocol_addon_test.cpp \
	tests/server/opcua_protocol_addon_test.h \
	tests/server/services_registry_test.h \
	tests/server/test_server_options.cpp \
	src/serverapp/server_options.cpp \
	src/serverapp/server_options.h
#tests/server/standard_namespace_test.h \ #completely outdated
#tests/server/standard_namespace_ut.cpp \ #completely outdated

# temporary disabled until new format is discussed.
#	tests/server/xml_addressspace_ut.cpp
#	tests/server/xml_address_space_addon_ut.cpp


test_opcuaserver_LDADD = libopcuaserver.la libopcuaprotocol.la libopcuacore.la libopcuaclient.la
test_opcuaserver_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 -I$(top_srcdir)/src/server $(GTEST_INCLUDES) $(GMOCK_INCLUDES) \
                            -DDYNAMIC_ADDON_PATH="\"${DYNAMIC_ADDON_PATH}\"" -DTEST_CORE_CONFIG_PATH="\"${TEST_CORE_CONFIG_PATH}\""
test_opcuaserver_LDFLAGS =  -ldl -lpthread $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) $(GCOV_LIBS) -lboost_program_options

####################################################
# Protocol library
####################################################


include/opc/ua/protocol/object_ids.h: schemas/NodeIds.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx object_ids > $@

src/protocol/string_utils_objectid_tostring.cpp: schemas/NodeIds.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx object_ids_tostring > $@

include/opc/ua/protocol/status_codes.h: schemas/StatusCode.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx status_codes > $@

src/protocol/string_utils_statuscode_tostring.cpp: schemas/StatusCode.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx status_codes_tostring > $@

include/opc/ua/protocol/attribute_ids.h: schemas/AttributeIds.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx attribute_ids > $@

src/client/opcua_options_attribute_ids.h: schemas/AttributeIds.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx attribute_ids_getoptionvalue > $@

src/protocol/string_utils_attributeid_tostring.cpp: schemas/AttributeIds.csv schemas/codegen.py
	python $(top_srcdir)/schemas/codegen.py cxx attribute_ids_tostring > $@

binaryinclude_HEADERS = \
  include/opc/ua/protocol/binary/common.h \
  include/opc/ua/protocol/binary/stream.h

protocolinclude_HEADERS = \
  include/opc/ua/protocol/enums.h \
  include/opc/ua/protocol/protocol.h \
  include/opc/ua/protocol/protocol_auto.h \
  include/opc/ua/protocol/attribute_ids.h \
  include/opc/ua/protocol/channel.h \
  include/opc/ua/protocol/data_value.h \
  include/opc/ua/protocol/datetime.h \
  include/opc/ua/protocol/describe.h \
  include/opc/ua/protocol/endpoints.h \
  include/opc/ua/protocol/expanded_object_ids.h \
  include/opc/ua/protocol/extension_identifiers.h \
  include/opc/ua/protocol/guid.h \
  include/opc/ua/protocol/input_from_buffer.h \
  include/opc/ua/protocol/message_identifiers.h \
  include/opc/ua/protocol/monitored_items.h \
  include/opc/ua/protocol/node_management.h \
  include/opc/ua/protocol/nodeid.h \
  include/opc/ua/protocol/object_ids.h \
  include/opc/ua/protocol/reference_ids.h \
  include/opc/ua/protocol/secure_channel.h \
  include/opc/ua/protocol/session.h \
  include/opc/ua/protocol/status_codes.h \
  include/opc/ua/protocol/strings.h \
  include/opc/ua/protocol/string_utils.h \
  include/opc/ua/protocol/types.h \
  include/opc/ua/protocol/types_manual.h \
  include/opc/ua/protocol/variable_access_level.h \
  include/opc/ua/protocol/variant.h \
  include/opc/ua/protocol/variant_visitor.h \
  include/opc/ua/protocol/utils.h \
  include/opc/ua/protocol/view.h

libopcuaprotocol_la_SOURCES = \
  src/protocol/deserialize_auto.cpp \
  src/protocol/serialize_auto.cpp \
  src/protocol/rawsize_auto.cpp \
  src/protocol/protocol.cpp \
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
  src/protocol/input_from_buffer.cpp \
  src/protocol/monitored_items.cpp \
  src/protocol/nodeid.cpp \
  src/protocol/session.cpp \
  src/protocol/subscriptions.cpp \
  src/protocol/status_codes.cpp \
  src/protocol/string_utils.cpp \
  src/protocol/string_utils_attributeid_tostring.cpp \
  src/protocol/string_utils_objectid_tostring.cpp \
  src/protocol/string_utils_statuscode_tostring.cpp \
  src/protocol/constructors_auto.cpp


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
  include/opc/ua/connection_listener.h \
  include/opc/ua/errors.h \
  include/opc/ua/event.h \
  include/opc/ua/global.h \
  include/opc/ua/node.h \
  include/opc/ua/model.h \
  include/opc/ua/socket_channel.h \
  include/opc/ua/subscription.h \
  include/opc/ua/server_operations.h

servicesinclude_HEADERS = \
  include/opc/ua/services/attributes.h \
  include/opc/ua/services/endpoints.h \
  include/opc/ua/services/method.h \
  include/opc/ua/services/node_management.h \
  include/opc/ua/services/services.h \
  include/opc/ua/services/subscriptions.h \
  include/opc/ua/services/view.h

commondir = $(opcincludedir)/common
common_HEADERS = \
                  include/opc/common/class_pointers.h \
                  include/opc/common/errors.h \
                  include/opc/common/exception.h \
                  include/opc/common/interface.h \
                  include/opc/common/modules.h \
                  include/opc/common/thread.h \
                  include/opc/common/uri_facade.h

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
                  src/core/common/thread.cpp \
                  src/core/common/addons_core/addon_manager.cpp \
                  src/core/common/addons_core/config_file.cpp \
                  src/core/common/addons_core/errors_addon_manager.cpp \
                  src/core/common/addons_core/dynamic_addon_factory.cpp \
                  src/core/common/addons_core/dynamic_library_lin.cpp \
                  src/core/common/addons_core/dynamic_library.h \
                  src/core/common/common_errors.cpp \
                  src/core/common/exception.cpp \
                  src/core/common/uri_facade_lin.cpp \
                  src/core/common/value.cpp \
                  src/core/event.cpp \
                  src/core/model_impl.h \
                  src/core/model_node.cpp \
                  src/core/model_object.cpp \
                  src/core/model_object_type.cpp \
                  src/core/model_server.cpp \
                  src/core/model_variable.cpp \
                  src/core/node.cpp \
                  src/core/opcua_errors.cpp \
                  src/core/socket_channel.cpp \
                  src/core/subscription.cpp \
		  src/core/server_operations.cpp

libopcuacore_la_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuacore_la_LIBADD = libopcuaprotocol.la
libopcuacore_la_LDFLAGS = -lpthread -ldl -lboost_thread -lboost_filesystem -lboost_system -lxml2 $(GCOV_LIBS) -Wl,--no-undefined

test_dynamic_addon.o: tests/core/test_dynamic_addon.cpp
	$(CC) -std=c++0x -I$(top_srcdir)/include -fPIC -g -c -Wall tests/core/test_dynamic_addon.cpp


libtest_dynamic_addon.so: test_dynamic_addon.o
	$(CC) -std=c++0x -g -Wall -ldl -shared -o libtest_dynamic_addon.so test_dynamic_addon.o


common_test_SOURCES = \
                  tests/core/common/error_test.cpp \
                  tests/core/common/main.cpp


common_test_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_test_LDADD = libopcuacore.la libopcuaprotocol.la
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

common_gtest_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 $(GTEST_INCLUDES) $(GMOCK_INCLUDES) -DDYNAMIC_ADDON_PATH="\"${DYNAMIC_ADDON_PATH}\"" -DTEST_CORE_CONFIG_PATH="\"${TEST_CORE_CONFIG_PATH}\""
common_gtest_LDADD = libopcuacore.la libopcuaprotocol.la
common_gtest_LDFLAGS = -ldl -lpthread -lxml2 $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIB) -no-undefined


#########################################################
# OPCUA client library
#########################################################

clientinclude_HEADERS = \
  include/opc/ua/client/addon.h \
  include/opc/ua/client/binary_client.h \
  include/opc/ua/client/client.h \
  include/opc/ua/client/remote_connection.h

libopcuaclient_la_SOURCES = \
  src/client/client.cpp \
  src/client/binary_client_addon.cpp \
  src/client/binary_client.cpp \
  src/client/binary_connection.cpp

libopcuaclient_la_CPPFLAGS =  -I$(top_srcdir)/include -I/usr/include/libxml2 $(GCOV_FLAGS)
libopcuaclient_la_LIBADD = libopcuaprotocol.la libopcuacore.la
libopcuaclient_la_LDFLAGS = -ldl -lpthread -lxml2

#########################################################
# OPCUA commandline client
#########################################################

opcua_SOURCES = \
  src/clientapp/opcua_main.cpp \
  src/clientapp/opcua_options.cpp \
  src/clientapp/opcua_options_attribute_ids.h \
  src/clientapp/opcua_options.h

opcua_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2 -DCONFIG_PATH=\"$(sysconfdir)/opcua/client\"
opcua_LDADD = libopcuaclient.la libopcuaprotocol.la libopcuacore.la
opcua_LDFLAGS =  -ldl -lboost_program_options -lpthread -lxml2

#########################################################
# OPCUA examples
#########################################################

noinst_PROGRAMS = example_client
example_client_SOURCES = \
  src/examples/example_client.cpp

example_client_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2
example_client_LDADD = libopcuaclient.la libopcuaprotocol.la libopcuacore.la
example_client_LDFLAGS = -ldl -lpthread -lxml2 -lboost_system

noinst_PROGRAMS += example_server
example_server_SOURCES = \
  src/examples/example_server.cpp

example_server_CPPFLAGS = -I$(top_srcdir)/include -I/usr/include/libxml2
example_server_LDADD = libopcuaserver.la libopcuaprotocol.la libopcuacore.la
example_server_LDFLAGS = -ldl -lpthread -lxml2 -lboost_system



#############################################################
# Extra configs and sources have to be in the distribution.
#############################################################

EXTRA_DIST = \
  src/clientapp/configs/opc_tcp.conf \
  src/serverapp/configs/server.conf \
  src/serverapp/configs/opcuaserver \
  src/serverapp/configs/opcuaserver.conf \
  tests/coreapp/configs \
  tests/gtest \
  tests/gmock \
  tests/server/common.cpp \
  tests/server/common.h \
  tests/server/configs \
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

install-data-local:
	install -d $(DESTDIR)/$(localstatedir)/log/opcua

tests/gtest/lib/.libs/libgmock.a:
	$(MAKE) -C $(top_srcdir)/tests/gmock

tests/gtest/lib/.libs/libgtest.a:
	$(MAKE) -C $(top_srcdir)/tests/gtest

distclean-local:
	$(MAKE) -C $(top_srcdir)/tests/gmock distclean
	$(MAKE) -C $(top_srcdir)/tests/gtest distclean


if ENABLE_PYTHON
python-dist:
	$(MAKE) -C $(top_srcdir)/python python-dist
endif

