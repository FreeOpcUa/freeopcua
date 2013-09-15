# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $


Bindings.h:
	wsdl2h Bindings.wsdl -o Bindings.h || test true
	soapcpp2 -2 -j -S -w -x Bindings.h || test true

soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp: Bindings.h
soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp: Bindings.h
soapBasicHttpBinding_USCOREISessionEndpointService.cpp: Bindings.h
soapC.cpp: Bindings.h
src/discovery_service.cpp: Bindings.h

opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua
uaclientincludedir = $(opcuaincludedir)/client
uaserverincludedir = $(opcuaincludedir)/server
addonsincludedir = $(uaserverincludedir)/addons

configdir = $(sysconfdir)/opcua

INCLUDES= \
  -Iinclude \
  -I$(top_srcdir)/endpoint_services/include \
  -I$(top_srcdir)/services_registry/include \
  $(OPC_CORE_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CLIENT_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)

lib_LTLIBRARIES = libopcua_soap_server.la

libopcua_soap_server_la_SOURCES = \
  src/discovery_service.cpp \
  src/discovery_service.h \
  src/endpoint_service.cpp \
  src/endpoint_service.h \
  src/registration_service.cpp \
  src/registration_service.h \
  src/soap_addon.cpp \
  src/soap_addon.h \
  src/soap_addon_factory.h \
  src/soap_module.cpp \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointService.cpp \
  soapC.cpp

libopcua_soap_server_la_CPPFLAGS = $(INCLUDES) -DWITH_PURE_VIRTUAL
libopcua_soap_server_la_LDFLAGS = -lpthread -ldl  $(OPC_CORE_LIBS) -lgsoap -lgsoap++ -Wl,--no-undefined

CLEANFILES= \
  Bindings.h \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.h \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.h \
  soapBasicHttpBinding_USCOREISessionEndpointService.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointService.h \
  soapC.cpp \
  soapH.h \
  soapStub.h \
  *.xml \
  *.nsmap

#TESTS = test_tcp_server test_tcp_server_addon

#TESTS_ENVIRONMENT = 

#check_PROGRAMS = $(TESTS)

#test_tcp_server_SOURCES = \
#  src/tcp_server.cpp \
#  src/tcp_server_addon_impl.cpp \
#  src/tcp_server_addon_impl.h \
#  tests/tcp_server_test.cpp

#test_tcp_server_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
#test_tcp_server_LDFLAGS = -lboost_thread -lpthread -ldl $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(OPC_CLIENT_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,--no-undefined 

#test_tcp_server_addon_SOURCES = \
#  src/tcp_server.cpp \
#  src/tcp_server_addon_impl.cpp \
#  src/tcp_server_addon_impl.h \
#  tests/tcp_server_addon_test.cpp

#test_tcp_server_addon_CPPFLAGS = $(INCLUDES) $(TESTINCLUDES)
#test_tcp_server_addon_LDFLAGS = -lboost_thread -lpthread -ldl $(UA_MAPPING_LIBS) $(URI_LIBS) $(OPC_CORE_LIBS) $(OPC_CLIENT_LIBS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS) -Wl,--no-undefined

