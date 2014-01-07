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
uaserverincludedir = $(opcuaincludedir)/server
addonsincludedir = $(uaserverincludedir)/addons


COMMON_INCLUDES = \
  -Iinclude \
  -I/usr/include/libxml2 \
  -I$(top_srcdir) \
  -I$(top_srcdir)/address_space/include \
  -I$(top_srcdir)/services_registry/include \
  $(UAMAPPINGS_INCLUDES) \
  $(OPC_CORE_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) -I.

lib_LTLIBRARIES = libxml_ns_addon.la

addonsinclude_HEADERS = \
  include/opc/ua/server/addons/xml_ns.h

libxml_ns_addon_la_SOURCES = \
  src/xml_address_space_addon.cpp \
  src/xml_address_space_loader.cpp \
  src/xml_address_space_loader.h \
  src/xml_address_space_module.cpp \
  src/xml_processor.h

libxml_ns_addon_la_CPPFLAGS = $(COMMON_INCLUDES)
libxml_ns_addon_la_LDFLAGS = -lpthread -ldl -lxml2  $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS)

EXTRA_DIST = \
  tests/base_node.xml \
  tests/empty.xml \
  tests/invalid_root.xml \
  tests/invalid_version.xml \
  tests/no_version.xml \
  tests/predefined_references.xml

TESTS = xml_address_space_ut

check_PROGRAMS = $(TESTS)

xml_address_space_ut_SOURCES = \
  ../address_space/src/address_space_addon.cpp \
  ../address_space/src/address_space_internal.cpp \
  ../services_registry/src/services_registry_factory.cpp \
  ../services_registry/src/services_registry_factory.h \
  ../services_registry/src/services_registry_impl.cpp \
  ../services_registry/src/services_registry_impl.h \
  src/xml_address_space_addon.cpp \
  src/xml_address_space_loader.cpp \
  tests/xml_addressspace_ut.cpp \
  tests/xml_address_space_addon_ut.cpp

xml_address_space_ut_CPPFLAGS = $(TESTINCLUDES)  $(COMMON_INCLUDES)
xml_address_space_ut_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl -lxml2 $(URI_LIBS) $(OPC_CORE_LIBS) $(UAMAPPINGS_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,--no-undefined

