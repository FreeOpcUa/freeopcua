# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

INCLUDES= -I$(top_srcdir)/src -I$(top_srcdir)/include  $(URI_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CORE_INCLUDES)
TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES) $(top_srcdir)/tests


lib_LTLIBRARIES = libxml_ns_addon.la
libxml_ns_addon_la_SOURCES = \
  xml_addressspace_addon.cpp \
  xml_addressspace_loader.cpp \
  xml_addressspace_loader.h

libxml_ns_addon_la_CPPFLAGS = $(INCLUDES) -I/usr/include/libxml2
libxml_ns_addon_la_LDFLAGS = -lpthread -ldl -lxml2  $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) -no-undefined


TESTS = xml_address_space_ut

check_PROGRAMS = $(TESTS)

xml_address_space_ut_SOURCES = \
  xml_addressspace_loader.cpp \
  tests/xml_addressspace_ut.cpp

xml_address_space_ut_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES) -I/usr/include/libxml2
xml_address_space_ut_LDFLAGS = -lboost_program_options -lboost_thread -lpthread -ldl -lxml2 $(URI_LIBS) $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -no-undefined

