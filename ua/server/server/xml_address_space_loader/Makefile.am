# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

INCLUDES= \
  -Iinclude \
  -I$(top_srcdir)/common/include \
  -I$(top_srcdir)/server/address_space/include \
  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) -I.


lib_LTLIBRARIES = libxml_ns_addon.la
libxml_ns_addon_la_SOURCES = \
  src/xml_address_space_addon.cpp \
  src/xml_address_space_loader.cpp \
  src/xml_address_space_loader.h \
  src/xml_address_space_module.cpp

libxml_ns_addon_la_CPPFLAGS = $(INCLUDES) -I/usr/include/libxml2 -I$(top_srcdir) 
libxml_ns_addon_la_LDFLAGS = -lpthread -ldl -lxml2  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined


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

xml_address_space_ut_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES) -I/usr/include/libxml2 -I$(top_srcdir)/server/services_registry/include
xml_address_space_ut_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl -lxml2 $(URI_LIBS) $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

